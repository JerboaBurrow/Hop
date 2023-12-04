#include <main.h>

int main(int argc, char ** argv)
{

  sf::ContextSettings contextSettings;
  contextSettings.depthBits = 24;
  contextSettings.antialiasingLevel = 0;

  sf::RenderWindow window(
    sf::VideoMode(resX,resY),
    "Soft Body Tetris",
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

  Hop::Logging::Log log;

  Hop::Console console(log);

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
  mapSource.load("bordered",false);

  world = std::make_unique<TileWorld>
  (
      2,
      &camera,
      16,
      1,
      &mapSource,
      &mapBounds  
  );

  sRender & rendering = manager.getSystem<sRender>();

  // setup physics system
  sPhysics & physics = manager.getSystem<sPhysics>();
  physics.setTimeStep(deltaPhysics);
  physics.setGravity(9.81, 0.0, -1.0);

  sCollision & collisions = manager.getSystem<sCollision>();

  auto det = std::make_unique<Hop::System::Physics::CellList>(world.get());

  auto res = std::make_unique<Hop::System::Physics::SpringDashpot>
  (
      deltaPhysics*10.0,
      0.5,
      0.0
  );

  uint64_t rootNCells = det->getRootNCells();

  collisions.setDetector(std::move(det));
  collisions.setResolver(std::move(res));

  high_resolution_clock::time_point t0, t1, tp0, tp1, tc0, tc1, tr0, tr1, tri0, tri1;

  Hop::LuaExtraSpace luaStore;

  luaStore.ecs = &manager;
  luaStore.world = world.get();
  luaStore.physics = &physics;
  luaStore.resolver = &collisions;

  console.luaStore(&luaStore);
  console.runFile("config.lua");
  console.runFile("tetris.lua");
  std::string status = console.luaStatus();
  if (status != "LUA_OK") { WARN(status) >> log; }

  Hop::Debugging::CollisionMeshDebug collisionMeshDebug;

  bool refreshObjectShaders = true;

  bool paused = true;
  bool meshes = true;

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
      if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F2)
      {
        meshes = !meshes;
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
      physics.step(&manager, &collisions, world.get());
    }

    tp1 = high_resolution_clock::now();

    tr0 = high_resolution_clock::now();

    rendering.setProjection(camera.getVP());
    if (refreshObjectShaders) { rendering.refreshShaders(); }
    rendering.draw(&manager, world.get()); 

    refreshObjectShaders = false;

    if (meshes)
    {
      collisionMeshDebug.debugCollisionMesh(&manager, camera.getVP());
    }

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
      double cdt = duration_cast<duration<double>>(tc1 - tc0).count();

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
          "Camera [world] (" << fixedLengthNumber(cameraX,4) << ", " << fixedLengthNumber(cameraY,4) << ")" <<
          "\n" << 
          "update time: " << fixedLengthNumber(pdt+rdt,6) <<
          "\n" <<
          "Col / Phys update / draw time: " << fixedLengthNumber(cdt,6) << "/" << fixedLengthNumber(pdt,6) << "/" << fixedLengthNumber(rdt,6);
          

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
