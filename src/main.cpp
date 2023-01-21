#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

#include <gl.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <orthoCam.h>
#include <Text/textRenderer.h>

#include <time.h>
#include <random>
#include <math.h>
#include <vector>

#include <Text/typeUtils.h>
#include <World/perlinWorld.h>

#include <Object/objectManager.h>

#include <System/sRender.h>

#include <log.h>

#include <logo.h>

#include <chrono>
using namespace std::chrono;


const int resX = 1000;
const int resY = 1000;
const float MAX_SPEED = 1.0/60.0;

// for smoothing delta numbers
uint8_t frameId = 0;
double deltas[60];

Shaders shaderPool;

int main(){

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

  uint8_t debug = 0;

  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_POINT_SPRITE);

  // for freetype rendering
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glEnable( GL_BLEND );
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);

  glm::mat4 defaultProj = glm::ortho(0.0,double(resX),0.0,double(resY),0.1,100.0);
  glm::mat4 textProj = glm::ortho(0.0,double(resX),0.0,double(resY));

  // must be initialised before so the shader is in use..?
  TextRenderer textRenderer(textProj);

  Type OD("resources/fonts/","OpenDyslexic-Regular.otf",48);

  OrthoCam camera(resX,resY,glm::vec2(0.0,0.0));

  glViewport(0,0,resX,resY);

  sf::Clock clock;

  sf::Clock timer;
  timer.restart();

  PerlinWorld map(2,camera.getVP(),16,16*3);

  float posX = 0.0;
  float posY = 0.0;

  ObjectManager manager(0);

  shaderPool.makeShader(marchingQuadVertexShader,marchingQuadFragmentShader,"mapShader");
  shaderPool.makeShader(objectVertexShader,circleObjectFragmentShader,"circleObjectShader");

  std::uniform_real_distribution<double> U;
  std::default_random_engine e;
  std::normal_distribution normal;
  int n = 1;

  sf::Clock timer2;
  double t1 = 0.0;
  double t2 = 0.0;
  timer.restart();
  Id pid;
  for (int i = 0; i < n; i++){
    timer2.restart();
    pid = manager.createObject();
    t1 += timer2.getElapsedTime().asSeconds();

    double x = 0.2;
    double y = 0.72;

    timer2.restart();

    manager.addComponent<cTransform>(
      pid,
      cTransform(
        x,y,0.0,0.1*map.worldUnitLength()
      )
    );

    manager.addComponent<cRenderable>(
      pid,
      cRenderable(
       "circleObjectShader",
       200.0/255.0,200.0/255.0,250.0/255.0,1.0
      )
    );

    manager.addComponent<cPhysics>(
      pid,
      cPhysics(
        x,y,0.0
      )
    );

    manager.addComponent<cCollideable>(
      pid,
      cCollideable(
        std::vector<CollisionVertex>{CollisionVertex(0.0,0.0,1.0)}
      )
    );

    t2 += timer2.getElapsedTime().asSeconds();
  }
  double ct = timer.getElapsedTime().asSeconds();
  Log & log = manager.getLog();

  INFO("object creation time/ per object " + std::to_string(ct) + ", " + std::to_string(ct/float(n))) >> log;
  INFO("createObject time "+std::to_string(t1/float(n))) >> log;
  INFO("addComponents time "+std::to_string(t2/float(n))) >> log;

  sRender & rendering = manager.getSystem<sRender>();
  sPhysics & physics = manager.getSystem<sPhysics>();
  sCollision & collisions = manager.getSystem<sCollision>();

  auto cellList = std::make_unique<CellList>(64,tupled(-1.0,1.0),tupled(-1.0,1.0));
  auto res = std::make_unique<SpringDashpot>(1.0/6.0,0.5,0.0);
  collisions.setDetector(std::move(cellList));
  collisions.setResolver(std::move(res));

  rendering.update(&manager, &shaderPool, true);

  while (window.isOpen()){

    sf::Event event;
    while (window.pollEvent(event)){
      if (event.type == sf::Event::Closed){
        return 0;
      }
      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape){
        return 0;
      }
      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F1){
        debug = !debug;
      }
      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::M){
        manager.addThread();
      }

      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::L){
        manager.releaseThread();
      }
      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space){
        posX = 0.0; posY = 0.0;
      }
    }

    if ( sf::Keyboard::isKeyPressed(sf::Keyboard::W) ){
      posY += MAX_SPEED;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
      posY -= MAX_SPEED;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
      posX -= MAX_SPEED;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
      posX += MAX_SPEED;
    }


    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    timer.restart();

    // TODO this uncoupling is horrible...
    map.updateRegion(posX,posY);
    std::pair<float,float> p = map.getPos();

    camera.setPosition(p.first,p.second);
    
    double udt = timer.getElapsedTime().asSeconds();

    map.draw(*shaderPool.get("mapShader").get());

    timer.restart();

    shaderPool.setProjection(camera.getVP());
    
    rendering.update(&manager, &shaderPool,false);
    collisions.update(&manager, &map);
    
    physics.update(&manager,1.0/600.0);
    
    double rudt = timer.getElapsedTime().asSeconds();
    timer.restart();
    rendering.draw(&shaderPool);
    double rdt = timer.getElapsedTime().asSeconds();

    deltas[frameId] = clock.getElapsedTime().asSeconds();
    frameId = (frameId+1) % 60;

    if (frameId == 59){
      std::cout << manager.getLog();
    }

    if (debug){
      double delta = 0.0;
      for (int n = 0; n < 60; n++){
        delta += deltas[n];
      }
      delta /= 60.0;
      std::stringstream debugText;

      sf::Vector2i mouse = sf::Mouse::getPosition(window);

      float cameraX = camera.getPosition().x;
      float cameraY = camera.getPosition().y;

      glm::vec4 world = camera.screenToWorld(mouse.x,mouse.y);
      Tile h;
      
      float x0, y0, s;
      
      map.worldToTileData(
        world[0],world[1],h,x0,y0,s
      );

      debugText << "Delta: " << fixedLengthNumber(delta,6) <<
        " (FPS: " << fixedLengthNumber(1.0/delta,4) << ")" <<
        "\n" <<
        "Mouse (" << fixedLengthNumber(mouse.x,4) << "," << fixedLengthNumber(mouse.y,4) << ")" <<
        "\n" <<
        "Mouse [world] (" << fixedLengthNumber(world[0],4) << "," << fixedLengthNumber(world[1],4) << ")" <<
        "\n" <<
        "Mouse cell (" << fixedLengthNumber(x0,4) << ", " << fixedLengthNumber(y0,4) << ", " << h <<
        "\n" <<
        "Camera [world] (" << fixedLengthNumber(cameraX,4) << ", " << fixedLengthNumber(cameraY,4) << ")" <<
        "\n" << 
        "update time: " << fixedLengthNumber(udt,6) <<
        "\n" <<
        "sRender update/draw time: " << fixedLengthNumber(rudt,6) << "/" << fixedLengthNumber(rdt,6);

      textRenderer.renderText(
        OD,
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
