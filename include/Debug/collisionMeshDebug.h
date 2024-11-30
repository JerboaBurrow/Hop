#ifndef COLLISIONMESHDEBUG_H
#define COLLISIONMESHDEBUG_H

#include <Collision/collisionPrimitive.h>
#include <Collision/collisionMesh.h>
#include <Object/entityComponentSystem.h>
#include <Component/cRenderable.h>

#include <Shader/meshDebug.shader>

#include <jGL/jGL.h>
#include <jLog/jLog.h>
#include <jGL/OpenGL/glShapeRenderer.h>

namespace Hop::Object
{
    class EntityComponentSystem;
}

namespace Hop::Debugging
{
    using Hop::Object::EntityComponentSystem;
    using Hop::Object::Component::cCollideable;
    using Hop::System::Physics::CollisionPrimitive;
    using Hop::System::Physics::MeshPoint;
    using Hop::System::Physics::MeshRectangle;
    using Hop::Object::Component::cRenderable;
    using Hop::Object::Component::cTransform;

    class CollisionMeshDebug
    {

    public:

        CollisionMeshDebug(std::shared_ptr<jGL::jGLInstance> jgl)
        : refresh(true),
          circles(jgl->createShapeRenderer(256)),
          rectangles(jgl->createShapeRenderer(256)),
          circleShader(std::make_shared<jGL::GL::glShader>
           (
              jGL::GL::glShapeRenderer::shapeVertexShader,
              Hop::System::Rendering::collisionPrimitiveFragmentShader
           )
          ),
          rectangleShader(std::make_shared<jGL::GL::glShader>
            (
                jGL::GL::glShapeRenderer::shapeVertexShader,
                jGL::GL::glShapeRenderer::rectangleFragmentShader
            )
          )
        {}

        ~CollisionMeshDebug()
        {
        }

        void drawMeshes(EntityComponentSystem * m, glm::mat4 proj);

        void refreshMeshes() { refresh = true; }

    private:

        bool refresh;

        std::shared_ptr<jGL::ShapeRenderer> circles, rectangles;

        std::shared_ptr<jGL::Shader> circleShader, rectangleShader;

        std::vector<cTransform> circlePos, rectanglePos;

    };

}

#endif /* COLLISIONMESHDEBUG_H */
