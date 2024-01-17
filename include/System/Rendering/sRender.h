#ifndef SRENDER_H
#define SRENDER_H

#include <System/system.h>
#include <Object/entityComponentSystem.h>
#include <World/world.h>
#include <Debug/collisionMeshDebug.h>

#include <jLog/jLog.h>

#include <jGL/shader.h>
#include <jGL/jGL.h>

#include <memory>

namespace Hop::Object
{
    class EntityComponentSystem;
}

namespace Hop::Debugging
{
    class CollisionMeshDebug;
}

namespace Hop::System::Rendering
{
    using Hop::Object::EntityComponentSystem;
    using jLog::Log;
    using Hop::World::AbstractWorld;
    using Hop::Debugging::CollisionMeshDebug;

    class sRender : public System
    {
    public:

        sRender()
        : accumulatedTime(0.0), 
          projection(0.0f), 
          drawCollisionMeshPoints(false),
          clock(std::chrono::high_resolution_clock::now()),
          collisionMeshDebug(nullptr)
        {}

        sRender(std::shared_ptr<jGL::jGLInstance> jgl)
        : accumulatedTime(0.0), 
          projection(0.0f), 
          drawCollisionMeshPoints(false), 
          clock(std::chrono::high_resolution_clock::now()),
          collisionMeshDebug(std::move(std::make_unique<CollisionMeshDebug>(jgl)))
        {}

        void setProjection(glm::mat4 proj) { projection = proj; }

        double draw
        (
            std::shared_ptr<jGL::jGLInstance> jgl,
            EntityComponentSystem * ecs = nullptr, 
            AbstractWorld * world = nullptr
        );

        void setDrawMeshes(bool b) { drawCollisionMeshPoints = b; }

    private:

        double accumulatedTime;
        glm::mat4 projection;

        bool drawCollisionMeshPoints;

        std::chrono::time_point<std::chrono::high_resolution_clock> clock;

        std::unique_ptr<CollisionMeshDebug> collisionMeshDebug;

        void update(EntityComponentSystem * ecs);

    };
}

#endif /* SRENDER_H */
