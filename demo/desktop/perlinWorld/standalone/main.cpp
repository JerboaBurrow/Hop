#include "main.h"

int main(int argc, char ** argv)
{

    jGL::DesktopDisplay display(glm::ivec2(resX,resY),"Perlin World");

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
                0,
                &perlin,
                &pBounds  
            )
        );
    }

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
        0.66,
        0.0
    );

    collisions.setDetector(std::move(det));
    collisions.setResolver(std::move(res));

    Hop::LuaExtraSpace luaStore;

    luaStore.ecs = &manager;
    luaStore.world = world.get();

    console.luaStore(&luaStore);

    console.runFile("mix.lua");
    std::string status = console.luaStatus();
    if (status != "LUA_OK") { WARN(status) >> log; }

    high_resolution_clock::time_point t0, t1, tp0, tp1, tr0, tr1;

    physics.stabaliseObjectParameters(&manager);

    std::vector<int> moveKeys = {GLFW_KEY_W, GLFW_KEY_S, GLFW_KEY_A, GLFW_KEY_D};
    std::vector<bool> moving = {false, false, false, false};

    while (display.isOpen())
    {        
        for (unsigned i = 0; i < moveKeys.size(); i++)
        {
            int key = moveKeys[i];
            std::vector<jGL::EventType> e  = display.getEventTypes(key);
            if 
            (
                std::find(e.begin(), e.end(), jGL::EventType::PRESS) != e.cend() ||
                std::find(e.begin(), e.end(), jGL::EventType::HOLD) != e.cend()
            )
            {
                moving[i] = true;
            }

            if (std::find(e.begin(), e.end(), jGL::EventType::RELEASE) != e.cend())
            {
                moving[i] = false;
            }
        }
        
        if (moving[0]) { posY += MAX_SPEED / camera.getZoomLevel(); }
        if (moving[1]) { posY -= MAX_SPEED / camera.getZoomLevel(); }
        if (moving[2]) { posX -= MAX_SPEED / camera.getZoomLevel(); }
        if (moving[3]) { posX += MAX_SPEED / camera.getZoomLevel(); }

        jGLInstance->beginFrame();

            jGLInstance->clear();

            t0 = high_resolution_clock::now();

            world->updateRegion(posX,posY);

            tp0 = high_resolution_clock::now();

            collisions.centreOn(world.get()->getMapCenter());
            
            physics.step(&manager, &collisions, world.get());

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
                    "Phys update / draw time: " << fixedLengthNumber(pdt,6) << "/" << fixedLengthNumber(rdt,6);

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
    return 0;
}
