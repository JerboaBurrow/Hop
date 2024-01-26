#ifndef COLLISIONMESHDEBUG_H
#define COLLISIONMESHDEBUG_H

#include <Collision/collisionPrimitive.h>
#include <Collision/collisionMesh.h>
#include <Object/entityComponentSystem.h>
#include <Component/cRenderable.h>

#include <Shader/meshDebug.shader>

#include <jGL/jGL.h>
#include <jLog/jLog.h>

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
    using Hop::Object::Component::cRenderable;
    using Hop::Object::Component::cTransform;

    class CollisionMeshDebug
    {

    public:

        CollisionMeshDebug(std::shared_ptr<jGL::jGLInstance> jgl)
        : refresh(true),
          shapes(jgl->createShapeRenderer(256)),
          circleShader(std::make_shared<jGL::GL::glShader>
           (
              Hop::System::Rendering::collisionPrimitiveVertexShader, 
              Hop::System::Rendering::collisionPrimitiveFragmentShader
           )
          ),
          rectangleShader(std::make_shared<jGL::GL::glShader>
            (
                Hop::System::Rendering::rectangleVertexShader, 
                Hop::System::Rendering::rectangleFragmentShader
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

        std::shared_ptr<jGL::ShapeRenderer> shapes;

        std::shared_ptr<jGL::Shader> circleShader, rectangleShader;

    };

}

#endif /* COLLISIONMESHDEBUG_H */
