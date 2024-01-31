#include <main.h>

int main(int argc, char ** argv)
{

    jGL::DesktopDisplay::Config conf;

    conf.VULKAN = false;
    conf.COCOA_RETINA = false;

    jGL::DesktopDisplay display(glm::ivec2(resX,resY),"Soft Body Tetris", conf);

    display.setFrameLimit(60);

    glewInit();

    jGLInstance = std::move(std::make_shared<jGL::GL::OpenGLInstance>(display.getRes()));

    jGLInstance->setTextProjection(glm::ortho(0.0,double(resX),0.0,double(resY)));
    jGLInstance->setMSAA(1);

    jGL::OrthoCam camera(resX, resY, glm::vec2(0.0,0.0));

    EntityComponentSystem manager;

    jLog::Log log;

    Hop::Console console(log);

    std::unique_ptr<AbstractWorld> world;

    float posX = 0.0;
    float posY = 0.0;

    Hop::World::FiniteBoundary mapBounds(0,0,16,16,true,false,true,true);
    Hop::World::FixedSource mapSource;

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
    rendering.setDrawMeshes(true);

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

    collisions.setDetector(std::move(det));
    collisions.setResolver(std::move(res));

    Hop::LuaExtraSpace luaStore;

    luaStore.ecs = &manager;
    luaStore.world = world.get();
    luaStore.physics = &physics;
    luaStore.resolver = &collisions;

    console.luaStore(&luaStore);
    console.runFile("config.lua");
    std::string status = console.luaStatus();
    if (status != "LUA_OK") { WARN(status) >> log; }

    high_resolution_clock::time_point t0, t1, tp0, tp1, tr0, tr1;

    while (display.isOpen())
    {

        if (display.getEvent(GLFW_KEY_SPACE).type == jGL::EventType::PRESS) { paused = !paused; }

        if (!paused)
        {
            console.runFile("loop.lua");
        }

        jGLInstance->beginFrame();

            jGLInstance->clear();

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
            rendering.draw(jGLInstance, &manager, world.get()); 

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

                double mouseX, mouseY;
                display.mousePosition(mouseX,mouseY);

                float cameraX = camera.getPosition().x;
                float cameraY = camera.getPosition().y;

                glm::vec4 worldPos = camera.screenToWorld(mouseX,mouseY);

                Hop::World::TileData tile = world->getTileData(worldPos[0],worldPos[1]);

                debugText << "Delta: " << fixedLengthNumber(delta,6) <<
                    " (FPS: " << fixedLengthNumber(1.0/delta,4) << ")" <<
                    "\n" <<
                    "Mouse (" << fixedLengthNumber(mouseX,4) << "," << fixedLengthNumber(mouseY,4) << ")" <<
                    "\n" <<
                    "Mouse [world] (" << fixedLengthNumber(worldPos[0],4) << "," << fixedLengthNumber(worldPos[1],4) << ")" <<
                    "\n" <<
                    "Mouse cell (" << fixedLengthNumber(tile.x,4) << ", " << fixedLengthNumber(tile.y,4) << ", " << tile.tileType <<
                    "\n" <<
                    "Camera [world] (" << fixedLengthNumber(cameraX,4) << ", " << fixedLengthNumber(cameraY,4) << ")" <<
                    "\n" << 
                    "update time: " << fixedLengthNumber(pdt+rdt,6) <<
                    "\n" <<
                    "Phys update / draw time: " << fixedLengthNumber(pdt,6) << "/" << fixedLengthNumber(rdt,6) <<
                    "\n" <<
                    "Kinetic Energy: " << fixedLengthNumber(physics.kineticEnergy(),6);

                jGLInstance->text
                (
                    debugText.str(),
                    glm::vec2(64.0f,resY-64.0f),
                    0.5f,
                    glm::vec4(0.0f,0.0f,0.0f, 1.0f)
                );

            }

            if (frameId == 30)
            {
                if (log.size() > 0)
                {
                    std::cout << log.get() << "\n";
                }
            }

        jGLInstance->endFrame();

        display.loop();

        t1 = high_resolution_clock::now();

        deltas[frameId] = duration_cast<duration<double>>(t1 - t0).count();
        frameId = (frameId+1) % 60;
        
    }

    jGLInstance->finish();

    return 0;
}
