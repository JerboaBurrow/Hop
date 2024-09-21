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

        if (sprites != nullptr)
        {
            sprites->setProjection(projection);
            sprites->draw();
        }

        auto t = std::chrono::high_resolution_clock::now();
        accumulatedTime += std::chrono::duration_cast<duration<double>>(t-clock).count();
        clock = t;

        return accumulatedTime;
    }

    void sRender::updateObjects() {}

    void sRender::update(EntityComponentSystem * ecs)
    {
        if (sprites != nullptr)
        {
            ComponentArray<cSprite> & spriteComponents = ecs->getComponentArray<cSprite>();
            ComponentArray<cRenderable> & renderables = ecs->getComponentArray<cRenderable>();
            ComponentArray<cTransform> & transforms = ecs->getComponentArray<cTransform>();

            for (auto & id : toRemove)
            {
                if
                (
                    renderables.hasComponent(id) &&
                    spriteComponents.hasComponent(id) &&
                    transforms.hasComponent(id)
                )
                {
                    sprites->remove(to_string(id));
                }
                objects.erase(id);
            }
            toRemove.clear();

            for (auto & id : toAdd)
            {
                if (renderables.hasComponent(id) && spriteComponents.hasComponent(id) && transforms.hasComponent(id))
                {
                    cSprite & oSprite = spriteComponents.get(id);
                    cRenderable & oRenderable = renderables.get(id);
                    cTransform & oTransform = transforms.get(id);
                    std::string sid = to_string(id);
                    sprites->add
                    (
                        jGL::Sprite
                        (
                            &oTransform,
                            &oSprite.textureRegion,
                            textures->get(oSprite.texturePath).get(),
                            &oRenderable.colour
                        ),
                        sid,
                        oRenderable.priority
                    );
                }
                objects.insert(id);
            }
            toAdd.clear();
        }
    }
}