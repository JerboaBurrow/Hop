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

#include <logo.h>

#include <engine.h>
#include <orthoCam.h>

using namespace std::chrono;

const int resX = 256;
const int resY = 512;
const float MAX_SPEED = 1.0/60.0;

// for smoothing delta numbers
uint8_t frameId = 0;
double deltas[60];

bool debug = false;

const double deltaPhysics = 1.0/900.0;

using Hop::Object::Component::cTransform;
using Hop::Object::Component::cPhysics;
using Hop::Object::Component::cRenderable;
using Hop::Object::Component::cCollideable;

using Hop::System::Physics::CollisionVertex;

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

  window.setVerticalSyncEnabled(true);
  window.setFramerateLimit(60);
  window.setActive();

  sf::Image icon;
  icon.loadFromMemory(&LOGO[0],sizeof(LOGO));

  window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

  glewInit();

  float posX = 0.0;
  float posY = 0.0;

  Hop::World::Boundary * bounds;
  Hop::World::MapSource * source;
  WorldOptions wOptions;
  PhysicsOptions phyOptions;

  Hop::World::FiniteBoundary mapBounds(0,0,16,16);
  Hop::World::FixedSource mapSource;
  mapSource.load("tile",false);

  WorldOptions mapWOptions(2,16,1,false);
  PhysicsOptions mapPhyOptions(deltaPhysics,9.81,0.66,true);

  Hop::World::InfiniteBoundary pBounds;
  Hop::World::PerlinSource perlin(2,0.07,5.0,5.0,256);
  perlin.setThreshold(0.2);
  perlin.setSize(64*3+1);

  WorldOptions pWOptions(2,64,1,true);
  PhysicsOptions pPhyOptions(deltaPhysics,9.81,0.66,true);

  std::cout << argc << "\n";
  if (argc > 1 && argv[1] == std::string("map"))
  {
    std::cout << "map\n";
    bounds = &mapBounds;
    source = &mapSource;

    wOptions = mapWOptions;
    phyOptions = mapPhyOptions;
 
  }
  else
  {
    bounds = &pBounds;
    source = &perlin;

    wOptions = pWOptions;
    phyOptions = pPhyOptions;
  }

  Hop::Engine hop
  (
    resX,
    resY,
    source,
    bounds,
    wOptions,
    phyOptions,
    0
  );

  sf::Clock clock;

  sf::Clock timer;
  timer.restart();

  std::uniform_real_distribution<double> U;
  std::default_random_engine e;
  std::normal_distribution normal;
  int n = 5000;

  sf::Clock timer2;
  double t1 = 0.0;
  double t2 = 0.0;
  timer.restart();

  Hop::Object::Id pid;

  double radius = 0.2*hop.getCollisionPrimitiveMaxSize();

  for (int i = 0; i < n; i++)
  {
    timer2.restart();
    pid = hop.createObject();
    t1 += timer2.getElapsedTime().asSeconds();

    double x = U(e);
    double y = U(e);

    timer2.restart();

    hop.addComponent<cTransform>(
      pid,
      cTransform(
        x,y,0.0,radius
      )
    );

    hop.addComponent<cRenderable>(
      pid,
      cRenderable(
       "circleObjectShader",
       200.0/255.0,200.0/255.0,250.0/255.0,1.0
      )
    );

    hop.addComponent<cPhysics>(
      pid,
      cPhysics(
        x,y,0.0
      )
    );

    hop.addComponent<cCollideable>(
      pid,
      cCollideable(
        std::vector<CollisionVertex>{CollisionVertex(0.0,0.0,1.0)},
        x,y,0.0,radius
      )
    );

    t2 += timer2.getElapsedTime().asSeconds();
  }
  double ct = timer.getElapsedTime().asSeconds();

  hop.log<INFO>("object creation time/ per object " + std::to_string(ct) + ", " + std::to_string(ct/float(n)));
  hop.log<INFO>("createObject time "+std::to_string(t1/float(n)));
  hop.log<INFO>("addComponents time "+std::to_string(t2/float(n)));

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
        hop.requestAddThread();
      }

      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::L)
      {
        hop.requestDeleteThread();
      }
      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
      {
        posX = 0.0; posY = 0.0;
      }

      if (event.type == sf::Event::MouseWheelScrolled)
      {
        double z = event.mouseWheelScroll.delta;
        hop.incrementZoom(z);
      }
    }

    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {

        posY += MAX_SPEED;
      
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {

        posY -= MAX_SPEED;
      
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {

        posX -= MAX_SPEED;
      
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {

        posX += MAX_SPEED;
      
    }

    hop.tryMoveWorld(posX,posY);

    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    timer.restart();

    hop.stepPhysics();

    double pdt = timer.getElapsedTime().asSeconds();

    timer.restart();

    hop.render();

    double rdt = timer.getElapsedTime().asSeconds();

    deltas[frameId] = clock.getElapsedTime().asSeconds();
    frameId = (frameId+1) % 60;

    if (frameId == 59)
    {
      hop.outputLog(std::cout);
    }

    if (debug)
    {
      double delta = 0.0;
      for (int n = 0; n < 60; n++)
      {
        delta += deltas[n];
      }
      delta /= 60.0;
      std::stringstream debugText;

      sf::Vector2i mouse = sf::Mouse::getPosition(window);

      const Hop::System::Rendering::OrthoCam & camera = hop.getCamera();

      float cameraX = camera.getPosition().x;
      float cameraY = camera.getPosition().y;

      glm::vec4 world = camera.screenToWorld(mouse.x,mouse.y);

      Hop::World::TileData tile = hop.getTileData(world[0],world[1]);

      debugText << "Delta: " << fixedLengthNumber(delta,6) <<
        " (FPS: " << fixedLengthNumber(1.0/delta,4) << ")" <<
        "\n" <<
        "Mouse (" << fixedLengthNumber(mouse.x,4) << "," << fixedLengthNumber(mouse.y,4) << ")" <<
        "\n" <<
        "Mouse [world] (" << fixedLengthNumber(world[0],4) << "," << fixedLengthNumber(world[1],4) << ")" <<
        "\n" <<
        "Mouse cell (" << fixedLengthNumber(tile.x,4) << ", " << fixedLengthNumber(tile.y,4) << ", " << tile.tileType <<
        "\n" <<
        "Camera [world] (" << fixedLengthNumber(cameraX,4) << ", " << fixedLengthNumber(cameraY,4) << ")" <<
        "\n" << 
        "update time: " << fixedLengthNumber(pdt+rdt,6) <<
        "\n" <<
        "Phys update / draw time: " << fixedLengthNumber(pdt,6) << "/" << fixedLengthNumber(rdt,6);

      hop.renderText(
        debugText.str(),
        64.0f,resY-64.0f,
        0.5f,
        glm::vec3(0.0f,0.0f,0.0f)
      );
    }

    clock.restart();

    window.display();
  }
  return 0;
}
