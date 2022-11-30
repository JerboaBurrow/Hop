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
#include <texturedQuad.h>
#include <World/world.h>
#include <Object/object.h>
#include <Object/particle.h>

const int resX = 1000;
const int resY = 1000;
const float MAX_SPEED = 1.0/60.0;

// for smoothing delta numbers
uint8_t frameId = 0;
double deltas[60];

int main(){

  Particle o(0.5,0.5,1.0/256.0);
  std::cout << o.id << ", " << o.id.hash() << "\n";

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

  World map(2,camera.getVP());

  float posX = 0.0;
  float posY = 0.0;

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
    map.draw();
    o.drawDebug(camera.getVP());

    deltas[frameId] = clock.getElapsedTime().asSeconds();
    frameId = (frameId+1) % 60;

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
        "update time: " << fixedLengthNumber(udt,6);

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
