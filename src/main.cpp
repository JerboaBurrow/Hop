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
#include <World/world.h>

#include <Object/objectManager.h>

#include <System/sRender.h>
#include <log.h>

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

  PerlinWorld map(2,camera.getVP());

  float posX = 0.0;
  float posY = 0.0;

  ObjectManager manager;

  // Shader mapShader(marchingQuadVertexShader,marchingQuadFragmentShader);
  // Shader circleObjectShader(objectVertexShader,circleObjectFragmentShader);
  shaderPool.makeShader(marchingQuadVertexShader,marchingQuadFragmentShader,"mapShader");
  shaderPool.makeShader(objectVertexShader,circleObjectFragmentShader,"circleObjectShader");

  std::uniform_real_distribution<double> U;
  std::default_random_engine e;
  std::normal_distribution normal;
  int n = 1000;
  sf::Clock timer2;
  double t1 = 0.0;
  double t2 = 0.0;
  double t3 = 0.0;
  timer.restart();
  for (int i = 0; i < n; i++){
    std::string name = "p"+std::to_string(i);
    timer2.restart();
    manager.createObject(name);
    t1 += timer2.getElapsedTime().asSeconds();

    timer2.restart();
    double x = U(e);
    double y = U(e);
    t3 += timer2.getElapsedTime().asSeconds();

    timer2.restart();
    Id pid = manager.idFromHandle(name);

    manager.addComponent<cTransform>(
      pid,
      cTransform(
        x,y,0.01,0.0
      )
    );

    manager.addComponent<cRenderable>(
      pid,
      cRenderable(
       "circleObjectShader"
      )
    );

    manager.addComponent<cPhysics>(
      pid,
      cPhysics(
        x,y,0.0
      )
    );

    t2 += timer2.getElapsedTime().asSeconds();
  }
  double ct = timer.getElapsedTime().asSeconds();
  std::cout << "object creation time/ per object " << ct << ", " << ct/float(n) << "\n";
  std::cout << "createObject time " << t1/float(n) << "\n";
  std::cout << "addComponent time " << t2/float(n) << "\n";
  std::cout << "random numbers " << t3/float(n) << "\n";

  sRender & rendering = manager.getSystem<sRender>();
  sPhysics & physics = manager.getSystem<sPhysics>();

  rendering.update(&manager, &shaderPool,true);

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
    map.updateRegion(posX,posY);
    camera.setPosition(posX,posY);
    double udt = timer.getElapsedTime().asSeconds();

    //map.draw(*shaderPool.get("mapShader").get());

    timer.restart();
    rendering.update(&manager, &shaderPool,false);
    shaderPool.setProjection(camera.getVP());
    double D = std::sqrt(2.0*0.1*60.0);
    for (auto it = physics.objects.begin(); it != physics.objects.end(); it++){
        cPhysics & data = manager.getComponent<cPhysics>(*it);
        cTransform & dataT = manager.getComponent<cTransform>(*it);

        data.fx += 1.0/600.0 * std::cos(dataT.theta)*dataT.scale;
        data.fy += 1.0/600.0 * std::sin(dataT.theta)*dataT.scale;
        data.omega += D*normal(e);
    }
    physics.update(&manager,1.0/60.0);
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

      debugText << "Delta: " << fixedLengthNumber(delta,6) <<
        " (FPS: " << fixedLengthNumber(1.0/delta,4) << ")" <<
        "\n" <<
        "Mouse (" << fixedLengthNumber(mouse.x,4) << "," << fixedLengthNumber(mouse.y,4) << ")" <<
        "\n" <<
        "Camera [world] (" << fixedLengthNumber(cameraX,4) << ", " << fixedLengthNumber(cameraY,4) << ")" <<
        "\n" <<
        "pos " << fixedLengthNumber(posX,4) << ", " << fixedLengthNumber(posY,4) <<
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
