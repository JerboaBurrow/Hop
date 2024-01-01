#include <System/Rendering/sRender.h>

namespace Hop::System::Rendering
{
        double sRender::draw
        (
            std::shared_ptr<jGL::jGLInstance> jgl,
            EntityComponentSystem * ecs, 
            AbstractWorld * world
        )
        {

            if (world != nullptr)
            {
                world->draw();
            }

            if (drawCollisionMeshPoints)
            {
                if (collisionMeshDebug == nullptr)
                {
                    collisionMeshDebug = std::move(std::make_unique<CollisionMeshDebug>(jgl));
                }
                collisionMeshDebug->refreshMeshes();
                collisionMeshDebug->drawMeshes(ecs, projection);
            }

            auto t = std::chrono::high_resolution_clock::now();
            accumulatedTime += std::chrono::duration_cast<duration<double>>(t-clock).count();
            clock = t;

            return accumulatedTime;
        }

        void sRender::update(EntityComponentSystem * ecs)
        {
        }
}