#include <engine.h>
#include <iostream>
namespace Hop
{

    Hop::Engine::Engine(
        unsigned resX, 
        unsigned resY,
        MapSource * map,
        Boundary * bounds,
        WorldOptions worldOptions,
        PhysicsOptions physicsOptions,
        unsigned maxThreads
    )
        :   camera(resX,resY,glm::vec2(0.0,0.0)),
            manager(maxThreads),
            textRenderer(glm::ortho(0.0,double(resX),0.0,double(resY))),
            font("resources/fonts/","OpenDyslexic-Regular.otf",48)

    {
        // make world
        if (worldOptions.marching)
        {
            world = static_cast<std::unique_ptr<AbstractWorld>>
            (
                std::make_unique<MarchingWorld>
                (
                    worldOptions.seed,
                    camera,
                    worldOptions.regionSize,
                    worldOptions.dynamicsShell,
                    map,
                    bounds  
                )
            );
        }
        else
        {
            world = std::make_unique<TileWorld>
            (
                worldOptions.seed,
                camera,
                worldOptions.regionSize,
                worldOptions.dynamicsShell,
                map,
                bounds  
            );
        }

        // default collisions setup

        auto det = std::make_unique<Hop::System::Physics::CellList>(world.get());

        auto res = std::make_unique<Hop::System::Physics::SpringDashpot>
            (
                physicsOptions.deltaTime*10.0,
                physicsOptions.coefficientResititution,
                0.0
            );

        setCollisionDetector(std::move(det));
        setCollisionResolver(std::move(res));

        // initialise default gl setup

        // for circular particles
        // glEnable(GL_PROGRAM_POINT_SIZE);
        // glEnable(GL_POINT_SPRITE);

        // projection matrix
        defaultProj = glm::ortho(0.0,double(resX),0.0,double(resY),0.1,100.0);
    
        // setup physics system
        sPhysics & physics = manager.getSystem<sPhysics>();
        physics.setTimeStep(physicsOptions.deltaTime);
        physics.setGravity(physicsOptions.gravity);

        if(physicsOptions.stabilise)
        {
            // will find parameters to stabilise the 
            // physics (essentially a velocity limiter)
            physics.stabaliseObjectParameters(&manager);
        }

        // setup default shaders

        shaderPool.makeShader
        (
            Hop::System::Rendering::marchingQuadVertexShader,
            Hop::System::Rendering::marchingQuadFragmentShader,
            "worldShader"
        );

        shaderPool.makeShader
        (
            Hop::System::Rendering::objectVertexShader,
            Hop::System::Rendering::circleObjectFragmentShader,
            "circleObjectShader"
        );

        // first pass to setup shaders
        needToRefreshRenderer = true;

        collisionTimeOO = 0.0;
        collisionTimeOW = 0.0;
        frame = 0;
    }

    void Engine::setCollisionDetector
    (
        std::unique_ptr<CollisionDetector> detector
    )
    {
        manager.getSystem<sCollision>().setDetector(std::move(detector));
    }

    void Engine::setCollisionResolver
    (
        std::unique_ptr<CollisionResolver> resolver
    )
    {
        manager.getSystem<sCollision>().setResolver(std::move(resolver));
    }

    void Engine::applyForce(
        Id i,
        double x,
        double y,
        double fx,
        double fy
    )
    {
        manager.getSystem<sPhysics>().applyForce
        (
            &manager,
            i,
            x,
            y,
            fx,
            fy
        );
    }

    void Engine::applyForceToAll(
        double fx,
        double fy
    )
    {
        manager.getSystem<sPhysics>().applyForce(
            &manager,
            fx,
            fy
        );
    }

    void Engine::stepPhysics
    (
        double overrideDelta,
        bool detectCollisions
    )
    {

        if (detectCollisions)
        {
            sCollision & collisions = manager.getSystem<sCollision>();

            collisions.centreOn(world.get()->getMapCenter());
            
            double oot, owt;
            collisions.update(&manager, world.get(), oot, owt);

            collisionTimeOO += oot;
            collisionTimeOW += owt;
            
            if (frame == 60)
            {
                frame = 0;
                std::string msg = "Collision time (o-o, o-w): " + std::to_string(collisionTimeOO/60.0) + ", " + std::to_string(collisionTimeOW/60.0);
                log<Hop::Logging::INFO>(msg);
                collisionTimeOO = 0.0;
                collisionTimeOW = 0.0;
            }
            frame++;
        }

        manager.getSystem<sPhysics>().update(&manager);
    }

    // Rendering

    void Engine::render
    (
        bool refreshObjectShaders
    )
    {

        if (needToRefreshRenderer){refreshObjectShaders=true;}

        sRender & rendering = manager.getSystem<sRender>();

        shaderPool.setProjection(camera.getVP());

        world.get()->draw(*shaderPool.get("worldShader").get());

        rendering.update(&manager, &shaderPool, refreshObjectShaders);

        rendering.draw(&shaderPool);

        needToRefreshRenderer = false;

    }

}