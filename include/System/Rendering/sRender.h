#ifndef SRENDER_H
#define SRENDER_H

#include <System/system.h>
#include <System/Rendering/sSpriteRender.h>
#include <Shader/shaders.h>
#include <Object/entityComponentSystem.h>
#include <World/world.h>
#include <jLog/jLog.h>

namespace Hop::Object
{
    class EntityComponentSystem ;
}

namespace Hop::System::Rendering
{
    using Hop::Object::EntityComponentSystem;
    using jLog::Log;
    using Hop::World::AbstractWorld;

    class sRender : public System
    {
    public:

        sRender()
        : staleShaders(false), accumulatedTime(0.0)
        {
            shaderPool.defaultShaders(log);
        }

        void setProjection(glm::mat4 proj) { shaderPool.setProjection(proj); }

        double draw
        (
            EntityComponentSystem * ecs = nullptr, 
            AbstractWorld * world = nullptr
        );

        void refreshShaders(){staleShaders = true;}

    private:

        bool staleShaders;

        double accumulatedTime;
        std::chrono::time_point<std::chrono::high_resolution_clock> clock;

        Shaders shaderPool;

        Log log;

        void update(EntityComponentSystem * ecs);

    };
}

#endif /* SRENDER_H */
