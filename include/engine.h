#ifndef ENGINE_H
#define ENGINE_H

#include <memory>
#include <chrono>
using namespace std::chrono;

#include <gl.h>

#include <orthoCam.h>
#include <Text/textRenderer.h>

#include <Object/objectManager.h>

#include <System/sPhysics.h>
#include <System/sRender.h>
#include <System/sCollision.h>

#include <World/world.h>
#include <World/marchingWorld.h>
#include <World/tileWorld.h>

#include <Util/util.h>

struct PhysicsOptions
{
    PhysicsOptions(double dt, double g, double c, bool s)
    : deltaTime(dt), gravity(g), coefficientResititution(c), stabilise(s)
    {}

    double deltaTime;
    double gravity;
    double coefficientResititution;
    bool stabilise;
};

struct WorldOptions 
{
    WorldOptions(uint64_t s, uint64_t r, uint8_t d, bool m)
    : seed(s), regionSize(r), dynamicsShell(d), marching(m)
    {}

    uint64_t seed;
    uint64_t regionSize;
    uint8_t dynamicsShell;
    bool marching;
};

/*

    Main user facing type

*/
namespace Hop 
{

    using Hop::System::Rendering::OrthoCam;
    using Hop::System::Rendering::Type;
    using Hop::System::Rendering::TextRenderer;
    using Hop::System::Rendering::sRender;
    using Hop::System::Rendering::Shaders;

    using Hop::Object::ObjectManager;
    using Hop::Object::Id;

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
    using Hop::World::TileData;

    class Engine
    {
    public:

        Engine(
            unsigned resX, 
            unsigned resY,
            MapSource * map,
            Boundary * bounds,
            WorldOptions worldOptions,
            PhysicsOptions physicsOptions,
            unsigned maxThreads = 1
        );

        // World 

        std::pair<float,float> getWorldPosition(){return world->getPos();}
        
        bool tryMoveWorld(float x, float y){return world->updateRegion(x,y);}

        TileData getTileData(float x, float y){return world->getTileData(x,y);}

        // i/o

        // object management

        Id createObject(std::string handle="")
        {
            if (handle == "")
            {
                return manager.createObject();
            }
            else
            {
                return manager.createObject(handle);
            }
            needToRefreshRenderer = true;
        }

        void remove(Id id){manager.remove(id); needToRefreshRenderer = true;}
        void remove(std::string handle){manager.remove(handle); needToRefreshRenderer = true;}

        void optimiseJobAllocation(){manager.optimiseJobAllocation();}

        // component management

        template <class T>
        void registerComponent(){manager.registerComponent<T>(); }

        template <class T>
        void addComponent(Id i, T component){manager.addComponent<T>(i,component); needToRefreshRenderer = true;}

        template <class T>
        void removeComponent(Id i){manager.removeComponent<T>(i); needToRefreshRenderer = true;}

        // system management

        template<class T>
        void registerSystem(){manager.registerSystem<T>();}

        template<class T>
        void setSystemSignature(Signature signature){manager.setSystemSignature<T>(signature);}

        template <class T>
        T & getSystem(){return manager.getSystem<T>();}

        // Physics

        void setCollisionDetector
        (
            std::unique_ptr<CollisionDetector> detector
        );

        void setCollisionResolver
        (
            std::unique_ptr<CollisionResolver> resolver
        );

        void applyForce(
            Id i,
            double x,
            double y,
            double fx,
            double fy
        );

        void applyForceToAll(
            double fx,
            double fy
        );

        void stepPhysics
        (
            double overrideDelta = 0.0,
            bool detectCollisions = true
        );

        // Rendering

        void render
        (
            bool refreshObjectShaders = false
        );

        void renderText
        (
            std::string text,
            float x,
            float y,
            float scale,
            glm::vec3 colour,
            float alpha = 1.0f,
            bool centre = false
        )
        {
            textRenderer.renderText
            (
                font,text,x,y,scale,colour,alpha,centre
            );
        }

        // Logging

        template <class T>
        void log(std::string msg){T(msg) >> manager.getLog();}

        void outputLog(std::ostream & o){o << manager.getLog();}

        // Thread

        void requestAddThread(){manager.addThread();}
        void requestDeleteThread(){manager.releaseThread();}
        void requestDeleteAllThreads(){manager.releaseAllThreads();}

        // Misc 

        double getCollisionPrimitiveMaxSize(){return world.get()->worldMaxCollisionPrimitiveSize();}

        // Camera

        const OrthoCam & getCamera() const {return camera;}
        void incrementZoom(double z){camera.incrementZoom(z);}

    private:

        OrthoCam camera;

        ObjectManager manager;

        std::unique_ptr<AbstractWorld> world;

        glm::mat4 defaultProj, textProj;

        Shaders shaderPool;

        bool needToRefreshRenderer;

        unsigned frame;
        double collisionTimeOO, collisionTimeOW;

        TextRenderer textRenderer;
        Type font;

    };

}

#endif /* ENGINE_H */
