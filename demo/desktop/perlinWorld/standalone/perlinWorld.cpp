#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

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

const int resX = 1000;
const int resY = 1000;
const float MAX_SPEED = 1.0/60.0;
const unsigned MAX_THREADS = 2;

// for smoothing delta numbers
uint8_t frameId = 0;
double deltas[60];

bool debug = false;

const double deltaPhysics = 1.0/900.0;

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

    Hop::Display display(resX,resY,"Perlin World Demo");

    glewInit();

    OrthoCam camera(resX, resY, glm::vec2(0.0,0.0));

    EntityComponentSystem manager;

    Hop::Logging::Log log;

    Hop::Console console(log);

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
                0,
                &perlin,
                &pBounds  
            )
        );
    }

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
        0.66,
        0.0
    );

    collisions.setDetector(std::move(det));
    collisions.setResolver(std::move(res));

    Hop::LuaExtraSpace luaStore;

    luaStore.ecs = &manager;
    luaStore.world = world.get();

    console.luaStore(&luaStore);

    console.runFile("tests/circles.lua");
    std::string status = console.luaStatus();
    if (status != "LUA_OK") { WARN(status) >> log; }

    high_resolution_clock::time_point t0, t1, tp0, tp1, tr0, tr1;

    physics.stabaliseObjectParameters(&manager);

    bool refreshObjectShaders = true;

    while (display.isOpen())
    {

        t0 = high_resolution_clock::now();

        world->updateRegion(posX,posY);

        glClearColor(1.0f,1.0f,1.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        tp0 = high_resolution_clock::now();

        collisions.centreOn(world.get()->getMapCenter());
        
        physics.step(&manager, &collisions, world.get(), &workers);

        tp1 = high_resolution_clock::now();

        tr0 = high_resolution_clock::now();

        shaderPool.setProjection(camera.getVP());

        rendering.updateAndDraw(&manager, world.get(), &shaderPool, refreshObjectShaders); 
        refreshObjectShaders = false;

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

        display.loop();

        t1 = high_resolution_clock::now();

        deltas[frameId] = duration_cast<duration<double>>(t1 - t0).count();
        frameId = (frameId+1) % 60;
        
    }
    return 0;
}
