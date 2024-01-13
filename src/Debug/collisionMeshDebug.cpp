#include <Debug/collisionMeshDebug.h>
#include <iostream>
namespace Hop::Debugging
{

    void CollisionMeshDebug::drawMeshes(EntityComponentSystem * m, glm::mat4 proj)
    {

        auto objects = m->getObjects();  

        auto citer = objects.cbegin();
        auto cend = objects.cend();

        double theta, scale;

        shapes->setProjection(proj);

        if (refresh)
        {
            refresh = false;
            while (citer != cend)
            {
                if (m->hasComponent<cCollideable>(citer->first))
                {
                    
                    cCollideable & c = m->getComponent<cCollideable>(citer->first);
                    cRenderable & ren = m->getComponent<cRenderable>(citer->first);
                    cTransform & trans = m->getComponent<cTransform>(citer->first);

                    theta = trans.theta;
                    scale = trans.scale;

                    for (unsigned i = 0; i < c.mesh.size(); i++)
                    {
                        CollisionPrimitive * cp = (c.mesh[i].get());
                        MeshPoint * cpmodel = c.mesh.getModelVertex(i).get();
                        //Rectangle * r = dynamic_cast<Rectangle*>(cp);

                        // if (r != nullptr)
                        // {
                        //     // TODO jGL needs to be able to draw rects
                        // }
                        // else
                        // {
                        //     // TODO jGL needs to be able to draw rects
                        // }

                        std::string sid = to_string(citer->first)+"-"+std::to_string(i);
                        if (!shapes->hasId(sid))
                        {
                            shapes->add(
                                std::make_shared<jGL::Shape>
                                (
                                    jGL::Transform(cp->x, cp->y, theta, scale*2.0*cpmodel->r),
                                    glm::vec4(ren.r, ren.g, ren.b, ren.a)
                                ),
                                sid
                            );
                        }
                    }
                }
                citer++;
            }
        }

        citer = objects.cbegin();

        while (citer != cend)
        {
          if (m->hasComponent<cCollideable>(citer->first))
          {
            
            cCollideable & c = m->getComponent<cCollideable>(citer->first);
            cRenderable & ren = m->getComponent<cRenderable>(citer->first);
            cTransform & trans = m->getComponent<cTransform>(citer->first);

            theta = trans.theta;
            scale = trans.scale;

            for (unsigned i = 0; i < c.mesh.size(); i++)
            {
                CollisionPrimitive * cp = (c.mesh[i].get());
                MeshPoint * cpmodel = c.mesh.getModelVertex(i).get();
                //Rectangle * r = dynamic_cast<Rectangle*>(cp);

                // if (r != nullptr)
                // {
                //     // TODO jGL needs to be able to draw rects
                // }
                // else
                // {
                //     // TODO jGL needs to be able to draw rects
                // }

                std::string sid = to_string(citer->first)+"-"+std::to_string(i);
                shapes->getShape(sid)->update
                (
                    jGL::Transform(cp->x, cp->y, theta, scale*2.0*cpmodel->r),
                    glm::vec4(ren.r, ren.g, ren.b, ren.a)
                );

            }

          }
          citer++;
        }

        shapes->draw(circleShader);

    }

}