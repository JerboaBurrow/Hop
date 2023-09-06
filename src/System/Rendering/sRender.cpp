#include <System/Rendering/sRender.h>

namespace Hop::System::Rendering
{
        double sRender::draw
        (
            EntityComponentSystem * ecs, 
            AbstractWorld * world
        )
        {

            if (ecs != nullptr)
            {
                sSpriteRender & sprite = ecs->getSystem<sSpriteRender>();
                update(ecs);
                sprite.draw(&shaderPool);
            }

            if (world != nullptr)
            {
                world->draw(*(shaderPool.get("worldShader").get()));
            }

            auto t = std::chrono::high_resolution_clock::now();
            accumulatedTime += std::chrono::duration_cast<duration<double>>(t-clock).count();
            clock = t;
        }

        void sRender::update(EntityComponentSystem * ecs)
        {
            sSpriteRender & sprite = ecs->getSystem<sSpriteRender>();

            sprite.update(ecs, &shaderPool, staleShaders);

            staleShaders = false;
        }
}