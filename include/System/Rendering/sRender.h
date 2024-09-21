#ifndef SRENDER_H
#define SRENDER_H

#include <System/system.h>
#include <Object/entityComponentSystem.h>
#include <Component/componentArray.h>
#include <World/world.h>
#include <Debug/collisionMeshDebug.h>
#include <Util/assetStore.h>
#include <Component/cRenderable.h>
#include <Component/cSprite.h>

#include <jLog/jLog.h>

#include <jGL/shader.h>
#include <jGL/jGL.h>
#include <jGL/spriteRenderer.h>

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
    using Hop::Util::Assets::TextureAssetStore;
    using Hop::Object::Component::ComponentArray;
    using Hop::Object::Component::cSprite;
    using Hop::Object::Component::cRenderable;
    using Hop::Object::Component::cTransform;
    /**
     * @brief System to handle rendering.
     *
     */
    class sRender : public System
    {
    public:

        /**
         * @brief Construct a new Renderer.
         *
         */
        sRender()
        : accumulatedTime(0.0),
          projection(0.0f),
          drawCollisionMeshPoints(false),
          clock(std::chrono::high_resolution_clock::now()),
          collisionMeshDebug(nullptr),
          textures(nullptr)
        {}

        void setProjection(glm::mat4 proj) { projection = proj; }

        double draw
        (
            std::shared_ptr<jGL::jGLInstance> jgl,
            EntityComponentSystem * ecs = nullptr,
            AbstractWorld * world = nullptr
        );

        /**
         * @brief Draw debug meshes.
         *
         * @param b switch to draw debug messhes.
         */
        void setDrawMeshes(bool b) { drawCollisionMeshPoints = b; }

        /**
         * @brief Set the TextureAssetStore for the rendering system.
         *
         * @param textureStore the TextureAssetStore.
         * @remark draw will reference textures from here.
         */
        void setTextureAssetStore(std::shared_ptr<TextureAssetStore> textureStore){ textures = textureStore; }

        /**
         * @brief Set the SpriteRenderer for the rendering system.
         *
         * @param spriteRenderer a jGL::SpriteRenderer.
         * @remark draw will reference sprites from here.
         */
        void setSpriteRenderer(std::shared_ptr<jGL::SpriteRenderer> spriteRenderer) { sprites = spriteRenderer; }

        /**
         * @brief Update the renderer from the ecs.
         *
         * @param ecs EntityComponentSystem.
         * @remark Currently changes to Hop::Object::Component::cSprite, and Hop::Object::Component::cRenderable
         * require updating manually.
         * @remark Sprites already track Hop::Object::Component::cTransform.
         */
        void update(EntityComponentSystem * ecs);

    private:

        double accumulatedTime;
        glm::mat4 projection;
        bool drawCollisionMeshPoints;
        std::chrono::time_point<std::chrono::high_resolution_clock> clock;

        std::shared_ptr<jGL::SpriteRenderer> sprites = nullptr;
        std::unique_ptr<CollisionMeshDebug> collisionMeshDebug = nullptr;
        std::shared_ptr<TextureAssetStore> textures = nullptr;

        void updateObjects();
    };
}

#endif /* SRENDER_H */
