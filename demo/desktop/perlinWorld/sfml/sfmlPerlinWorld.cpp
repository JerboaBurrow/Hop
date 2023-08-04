#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

#include <gl.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <time.h>
#include <random>
#include <math.h>
#include <vector>

#include <chrono>
using namespace std::chrono;

#include <logo.h>

#include <Display/display.h>
#include <orthoCam.h>

#include <Text/textRenderer.h>

#include <Object/entityComponentSystem.h>

#include <System/sPhysics.h>
#include <System/sRender.h>
#include <System/sCollision.h>

#include <World/world.h>
#include <World/marchingWorld.h>
#include <World/tileWorld.h>

#include <Console/console.h>

#include <Util/util.h>
#include <log.h>

using namespace std::chrono;

const int resX = 1000;
const int resY = 1000;
const float MAX_SPEED = 1.0/60.0;
const unsigned MAX_THREADS = 0;

// for smoothing delta numbers
uint8_t frameId = 0;
double deltas[60];

bool debug = false;

const double deltaPhysics = 1.0/(900.0);
const unsigned subSamples = 1;

using Hop::Object::Component::cTransform;
using Hop::Object::Component::cPhysics;
using Hop::Object::Component::cRenderable;
using Hop::Object::Component::cCollideable;
using Hop::Object::EntityComponentSystem;
using Hop::Object::Id;

using Hop::System::Rendering::OrthoCam;
using Hop::System::Rendering::Type;
using Hop::System::Rendering::TextRenderer;
using Hop::System::Rendering::sRender;
using Hop::System::Rendering::Shaders;

using Hop::System::Physics::CollisionDetector;
using Hop::System::Physics::CollisionResolver;
using Hop::System::Physics::sPhysics;
using Hop::System::Physics::sCollision;

using Hop::System::Signature;

using Hop::World::MapSource;
using Hop::World::Boundary;
using Hop::World::AbstractWorld;
using Hop::World::TileWorld;
using Hop::World::MarchingWorld;

using Hop::Logging::INFO;
using Hop::Logging::WARN;

using Hop::Util::fixedLengthNumber;

int main(int argc, char ** argv)
{

  sf::ContextSettings contextSettings;
  contextSettings.depthBits = 24;
  contextSettings.antialiasingLevel = 0;

  sf::RenderWindow window(
    sf::VideoMode(resX,resY),
    "Jerboa",
    sf::Style::Close|sf::Style::Titlebar,
    contextSettings);

  //window.setVerticalSyncEnabled(true);
  window.setFramerateLimit(60);
  window.setActive();

  sf::Image icon;
  icon.loadFromMemory(&LOGO[0],sizeof(LOGO));

  window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

  glewInit();

  OrthoCam camera(resX, resY, glm::vec2(0.0,0.0));

  EntityComponentSystem manager;

  Hop::Console console;

  Hop::Logging::Log log;

  ThreadPool workers(MAX_THREADS);

  TextRenderer textRenderer(glm::ortho(0.0,double(resX),0.0,double(resY)));
  Type font(48);

  Shaders shaderPool;
  shaderPool.defaultShaders(log);

  std::unique_ptr<AbstractWorld> world;

  float posX = 0.0;
  float posY = 0.0;

  Hop::World::Boundary * bounds;
  Hop::World::MapSource * source;

  Hop::World::FiniteBoundary mapBounds(0,0,16,16);
  Hop::World::FixedSource mapSource;
  mapSource.load("tile",false);

  Hop::World::InfiniteBoundary pBounds;
  Hop::World::PerlinSource perlin(2,0.07,5.0,5.0,256);
  perlin.setThreshold(0.2);
  perlin.setSize(64*3+1);

  if (argc > 1 && argv[1] == std::string("map"))
  {

      world = std::make_unique<TileWorld>
      (
          2,
          &camera,
          16,
          1,
          &mapSource,
          &mapBounds  
      );
  }
  else
  {
      world = static_cast<std::unique_ptr<AbstractWorld>>
      (
          std::make_unique<MarchingWorld>
          (
              2,
              &camera,
              64,
              1,
              &perlin,
              &pBounds  
          )
      );
  }

  sRender & rendering = manager.getSystem<sRender>();

  // setup physics system
  sPhysics & physics = manager.getSystem<sPhysics>();
  physics.setTimeStep(deltaPhysics);

  sCollision & collisions = manager.getSystem<sCollision>();

  auto det = std::make_unique<Hop::System::Physics::CellList>(world.get());

  auto res = std::make_unique<Hop::System::Physics::SpringDashpot>
  (
      deltaPhysics*10.0,
      0.5,
      0.0
  );

  collisions.setDetector(std::move(det));
  collisions.setResolver(std::move(res));

  high_resolution_clock::time_point t0, t1, tp0, tp1, tr0, tr1;

  Hop::LuaExtraSpace luaStore;

  luaStore.ecs = &manager;
  luaStore.world = world.get();

  console.luaStore(&luaStore);

  console.runFile("tests/rectangles.lua");
  std::string status = console.luaStatus();
  if (status != "LUA_OK") { WARN(status) >> log; }

  //physics.stabaliseObjectParameters(&manager);

  bool refreshObjectShaders = true;

  bool paused = true;

  while (window.isOpen())
  {

    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
      {
        return 0;
      }
      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
      {
        return 0;
      }
      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F1)
      {
        debug = !debug;
      }
      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::M)
      {
        workers.createThread();
        INFO("Thread: "+std::to_string(workers.size())) >> log;
      }

      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::L)
      {
        workers.joinThread();
        INFO("Thread: "+std::to_string(workers.size())) >> log;
      }

      if (event.type == sf::Event::MouseWheelScrolled)
      {
        double z = event.mouseWheelScroll.delta;
        camera.incrementZoom(z);
      }
      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
      {
        paused = !paused;
      }
    }

    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {

        posY += MAX_SPEED/camera.getZoomLevel();
      
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {

        posY -= MAX_SPEED/camera.getZoomLevel();
      
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {

        posX -= MAX_SPEED/camera.getZoomLevel();
      
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {

        posX += MAX_SPEED/camera.getZoomLevel();
      
    }

    t0 = high_resolution_clock::now();

    world->updateRegion(posX,posY);

    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    tp0 = high_resolution_clock::now();

    collisions.centreOn(world.get()->getMapCenter());
    
    if (!paused)
    {
      for (unsigned k = 0 ; k < subSamples; k++)
      {
        if (workers.size() > 1)
        {
          collisions.update(&manager, world.get(),&workers);
        }
        else
        {
          collisions.update(&manager, world.get());
        }

        physics.gravityForce(&manager,9.81,0.0,-1.0);

        physics.update(&manager);
      }
    }

    tp1 = high_resolution_clock::now();

    tr0 = high_resolution_clock::now();

    shaderPool.setProjection(camera.getVP());

    world.get()->draw(*shaderPool.get("worldShader").get());

    rendering.update(&manager, &shaderPool, refreshObjectShaders);
    refreshObjectShaders = false;

    rendering.draw(&shaderPool); 

    // auto objects = manager.getObjects();  

    // auto citer = objects.cbegin();
    // auto cend = objects.cend();

    // while (citer != cend)
    // {
    //   if (manager.hasComponent<cCollideable>(citer->first))
    //   {
        
    //     cCollideable & c = manager.getComponent<cCollideable>(citer->first);

    //     c.mesh.drawDebug(camera.getVP());
    //   }
    //   citer++;
    // }

    tr1 = high_resolution_clock::now();

    if (debug)
    {
      double delta = 0.0;
      for (int n = 0; n < 60; n++)
      {
          delta += deltas[n];
      }
      delta /= 60.0;
      std::stringstream debugText;

      double pdt = duration_cast<duration<double>>(tp1 - tp0).count();
      double rdt = duration_cast<duration<double>>(tr1 - tr0).count();

      sf::Vector2i mouse = sf::Mouse::getPosition(window);

      float cameraX = camera.getPosition().x;
      float cameraY = camera.getPosition().y;

      glm::vec4 worldPos = camera.screenToWorld(mouse.x,mouse.y);

      Hop::World::TileData tile = world->getTileData(worldPos[0],worldPos[1]);

      debugText << "Delta: " << fixedLengthNumber(delta,6) <<
          " (FPS: " << fixedLengthNumber(1.0/delta,4) << ")" <<
          "\n" <<
          "Mouse (" << fixedLengthNumber(mouse.x,4) << "," << fixedLengthNumber(mouse.y,4) << ")" <<
          "\n" <<
          "Mouse [world] (" << fixedLengthNumber(worldPos[0],4) << "," << fixedLengthNumber(worldPos[1],4) << ")" <<
          "\n" <<
          "Mouse cell (" << fixedLengthNumber(tile.x,4) << ", " << fixedLengthNumber(tile.y,4) << ", " << tile.tileType <<
          "\n" <<
          "Camera [world] (" << fixedLengthNumber(cameraX,4) << ", " << fixedLengthNumber(cameraY,4);// << ")" <<
          // "\n" << 
          // "update time: " << fixedLengthNumber(pdt+rdt,6) <<
          // "\n" <<
          // "Phys update / draw time: " << fixedLengthNumber(pdt,6) << "/" << fixedLengthNumber(rdt,6);

      textRenderer.renderText(
          font,
          debugText.str(),
          64.0f,resY-64.0f,
          0.5f,
          glm::vec3(0.0f,0.0f,0.0f)
      );
    }

    if (frameId == 30)
    {
      if (log.size() > 0)
      {
        std::cout << log.get() << "\n";
      }
    }

    window.display();

    t1 = high_resolution_clock::now();

    deltas[frameId] = duration_cast<duration<double>>(t1 - t0).count();
    frameId = (frameId+1) % 60;

  }
  return 0;
}