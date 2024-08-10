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

            sprites->draw();

            auto t = std::chrono::high_resolution_clock::now();
            accumulatedTime += std::chrono::duration_cast<duration<double>>(t-clock).count();
            clock = t;

            return accumulatedTime;
        }

        void sRender::update(EntityComponentSystem * ecs)
        {
            ComponentArray<cSprite> & spriteComponents = ecs->getComponentArray<cSprite>();
            ComponentArray<cRenderable> & renderables = ecs->getComponentArray<cRenderable>();
            for (const auto & object : objects)
            {
                if (renderables.hasComponent(object) && spriteComponents.hasComponent(object))
                {
                    const cSprite & oSprite = spriteComponents.get(object);
                    const cRenderable & oRenderable = renderables.get(object);
                    std::string sid = to_string(object);
                    if (sprites->hasId(sid))
                    {
                        auto & sprite = sprites->getSprite(sid);
                        auto tex = textures->get(oSprite.texturePath);
                        if (!(tex->getId() == sprite.texture->getId()))
                        {
                            sprites->remove(sid);
                            sprites->add(sprite, sid, oRenderable.priority);
                        }
                        sprites->updatePriority(sid, oRenderable.priority);
                        sprite.setAlpha(oRenderable.a);
                        sprite.setTextureRegion
                        (
                            jGL::TextureRegion
                            (
                                oSprite.tx,
                                oSprite.ty,
                                oSprite.lx,
                                oSprite.ly
                            )
                        );
                    }
                }
            }
        }
}