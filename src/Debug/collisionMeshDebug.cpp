#include <Debug/collisionMeshDebug.h>
#include <iostream>
namespace Hop::Debugging
{

    void CollisionMeshDebug::drawMeshes(EntityComponentSystem * m, glm::mat4 proj)
    {

        auto objects = m->getObjects();

        auto citer = objects.cbegin();
        auto cend = objects.cend();

        uint64_t s = 0;
        while (citer != cend)
        {
            if (m->hasComponent<cCollideable>(citer->first))
            {
                cCollideable & c = m->getComponent<cCollideable>(citer->first);
                s += c.mesh.size();
            }
            citer++;
        }

        double theta, scale;

        circles->setProjection(proj);
        rectangles->setProjection(proj);
        citer = objects.cbegin();

        circles->clear();
        rectangles->clear();
        circlePos.clear();
        circlePos.reserve(s);
        rectanglePos.clear();
        rectanglePos.reserve(s);
        while (citer != cend)
        {
            uint64_t p = 0;
            if (m->hasComponent<cCollideable>(citer->first))
            {

                cCollideable & c = m->getComponent<cCollideable>(citer->first);
                cRenderable & ren = m->getComponent<cRenderable>(citer->first);
                cTransform & trans = m->getComponent<cTransform>(citer->first);

                theta = trans.theta;
                scale = std::max(trans.scaleX, trans.scaleY);

                for (unsigned i = 0; i < c.mesh.size(); i++)
                {
                    CollisionPrimitive * cp = (c.mesh[i].get());
                    MeshPoint * cpmodel = c.mesh.getModelVertex(i).get();
                    MeshRectangle * r = dynamic_cast<MeshRectangle*>(cpmodel);

                    if (r != nullptr)
                    {
                        std::string sid = to_string(citer->first)+"-"+std::to_string(i);
                        rectanglePos.push_back(jGL::Transform(cp->x, cp->y, theta, r->width()*scale, r->height()*scale));
                        rectangles->add(
                            {
                                &rectanglePos.back(),
                                &ren.colour
                            },
                            sid,
                            ren.priority
                        );
                    }
                    else
                    {
                        std::string sid = to_string(citer->first)+"-"+std::to_string(i);
                        circlePos.push_back(jGL::Transform(cp->x, cp->y, theta, scale*2.0*cpmodel->r));
                        circles->add(
                            {
                                &circlePos.back(),
                                &ren.colour
                            },
                            sid,
                            ren.priority
                        );
                    }
                    
                    p++;
                }
            }
            citer++;
        }

        circles->draw(circleShader);
        rectangles->draw(rectangleShader);

    }

}