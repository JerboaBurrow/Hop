#ifndef COLLISIONMESH
#define COLLISIONMESH

#include <vector>
#include <cmath>
#include <limits>
#include <cstdint>
#include <memory>
#include <iostream>

#include <Maths/vertex.h>
#include <Maths/transform.h>
#include <Component/cTransform.h>

using Hop::Maths::Vertex;
using Hop::Object::Component::cTransform;

namespace Hop::System::Physics
{

    const uint8_t LAST_INSIDE_COUNTER_MAX = 60;

    struct MeshPoint
    {
        MeshPoint(double x, double y, double r)
        : x(x), y(y), r(r)
        {}

        MeshPoint()
        : x(0.0), y(0.0), r(0.0)
        {}

        virtual ~MeshPoint() = default;

        double x; double y; double r;
    };

    struct MeshRectangle : public MeshPoint
    {

        MeshRectangle()
        : MeshRectangle(0.0, 0.0,
                    0.0, 0.0,
                    0.0, 0.0,
                    0.0, 0.0)
        {}

        MeshRectangle
        (        
            double llx, double lly,
            double ulx, double uly,
            double urx, double ury,
            double lrx, double lry
        )
        : llx(llx), lly(lly),
          ulx(ulx), uly(uly),
          urx(urx), ury(ury),
          lrx(lrx), lry(lry)
        {
            x = (llx+ulx+urx+lrx)/4.0;
            y = (lly+uly+ury+lry)/4.0;

            double dx = llx-x;
            double dy = lly-y;

            r = std::sqrt(dx*dx+dy*dy);
        }

        double llx, lly, ulx, uly, urx, ury, lrx, lry;
    };

    struct CollisionPrimitive 
    {

        static constexpr double RIGID = 1e6;

        CollisionPrimitive() = default;

        CollisionPrimitive
        (
            double x, 
            double y, 
            double r, 
            double k = CollisionPrimitive::RIGID,
            double d = 1.0,
            double m = 1.0
        )
        : x(x),y(y),r(r),lastInside(0),
          ox(x),oy(y),fx(0),fy(0),
          xp(x),yp(y),
          vx(0),vy(0),
          stiffness(k), mass(m), damping(d)
        {}

        virtual ~CollisionPrimitive() = default;
        // x position, y position, radius (model space)
        //  of a collision point
        double x, y, r;
        uint8_t lastInside;

        double ox, oy;

        double fx, fy, xp, yp, vx, vy;

        double stiffness = CollisionPrimitive::RIGID;
        double mass, damping;

        void setRecentlyInside(int i){ lastInside =  i; }
        bool recentlyInside() const { return lastInside > 0; }

        bool isRigid() { return stiffness >= RIGID; }

        void applyForce(double x, double y)
        {
            fx+=x;
            fy+=y;
        }

        void setOrigin
        (
            double nox, 
            double noy
        )
        {
            ox = nox;
            oy = noy;
            x = ox;
            y = oy;
            xp = ox;
            yp = oy;
        }

        void step
        (
            double dt,
            double nox,
            double noy
        )
        {

            ox = nox;
            oy = noy;

            double rox = x-ox;
            double roy = y-oy;

            // spring with relaxed state at ox, oy;
            double ax = (fx-stiffness*rox-damping*vx)/mass;
            double ay = (fy-stiffness*roy-damping*vy)/mass;

            fx = 0.0;
            fy = 0.0;

            double xtp = x;
            double ytp = y;

            x = 2.0*x - xp + ax * dt*dt;
            y = 2.0*y - yp + ay * dt*dt;

            vx = (x-xtp)/(2.0*dt);
            vy = (y-ytp)/(2.0*dt);

            xp = xtp;
            yp = ytp;
        }

    };

    struct Rectangle : public CollisionPrimitive 
    {
        Rectangle()
        : Rectangle(0.0, 0.0,
                    0.0, 0.0,
                    0.0, 0.0,
                    0.0, 0.0)
        {
            stiffness = CollisionPrimitive::RIGID;
        }

        Rectangle
        (        
            double llx, double lly,
            double ulx, double uly,
            double urx, double ury,
            double lrx, double lry,
            double k = CollisionPrimitive::RIGID
        )
        :
          llx(llx), lly(lly),
          ulx(ulx), uly(uly),
          urx(urx), ury(ury),
          lrx(lrx), lry(lry)
        {
            x = (llx+ulx+urx+lrx)/4.0;
            y = (lly+uly+ury+lry)/4.0;

            stiffness = k;

            double dx = llx-x;
            double dy = lly-y;

            r = std::sqrt(dx*dx+dy*dy);

            lastInside = 0;

            resetAxes();

            damping = 0.0;
            stiffness = k;
            mass = 1.0;
            xp = x;
            yp = y;
            ox = x;
            oy = y;
            vx = 0.0;
            vy = 0.0;
            fx = 0.0;
            fy = 0.0;
        }

        void resetAxes()
        {
            axis1x = llx-lrx;
            axis1y = lly-lry;

            x = (llx+ulx+urx+lrx)/4.0;
            y = (lly+uly+ury+lry)/4.0;

            double dx = llx-x;
            double dy = lly-y;

            r = std::sqrt(dx*dx+dy*dy);

            double d = std::sqrt(axis1x*axis1x+axis1y*axis1y);

            axis1x /= d;
            axis1y /= d;

            axis2x = ulx-llx;
            axis2y = uly-lly;

            d = std::sqrt(axis2x*axis2x+axis2y*axis2y);

            axis2x /= d;
            axis2y /= d;
        }
        
        double height()
        {

            double u = llx-lrx;
            double v = lly-lry;

            return std::sqrt(u*u+v*v);

        }

        double width()
        {

            double u = ulx-llx;
            double v = uly-lly;

            return std::sqrt(u*u+v*v);

        }

        double llx, lly;
        double ulx, uly;
        double urx, ury;
        double lrx, lry;
        double axis1x, axis1y;
        double axis2x, axis2y;
    };

    std::ostream & operator<<(std::ostream & o, Rectangle const & r);

    struct CollisionMesh 
    {
        CollisionMesh()
        {}
        // construct a mesh around a model space polygon 
        //   with vertices v with each mesh vertex having 
        //   radius r in model space
        //CollisionMesh(std::vector<Vertex> v, double r = 0.01);

        // construct a mesh from given points
        CollisionMesh
        (
            std::vector<std::shared_ptr<CollisionPrimitive>> v,
            double x,
            double y, 
            double theta, 
            double scale,
            double stiffness = CollisionPrimitive::RIGID,
            double damping = 1.0,
            double mass = 1.0
        )
        : CollisionMesh(std::move(v), stiffness, damping, mass)
        {
            cTransform transform(x,y,theta,scale);
            updateWorldMesh(transform, 0.0, true);
        }

        CollisionMesh
        (
            std::vector<std::shared_ptr<CollisionPrimitive>> v,
            double stiffness = CollisionPrimitive::RIGID,
            double damping = 1.0,
            double mass = 1.0
        )
        {
            if (stiffness >= CollisionPrimitive::RIGID)
            {
                rigid = true;
            }
            else 
            {
                rigid = false;
            }
            worldVertices.clear();
            for (unsigned i = 0; i < v.size(); i++)
            {
                CollisionPrimitive * c = (v[i].get());
                Rectangle * l = dynamic_cast<Rectangle*>(c);

                std::shared_ptr<CollisionPrimitive> p;

                if (l != nullptr)
                {
                    vertices.push_back
                    (
                        std::move
                        (
                            std::make_shared<MeshRectangle>
                            (
                                l->llx, l->lly,
                                l->ulx, l->uly,
                                l->urx, l->ury,
                                l->lrx, l->lry
                            )
                        )
                    );

                    p = std::make_shared<Rectangle>
                    (
                        Rectangle
                        (
                            l->llx, l->lly,
                            l->ulx, l->uly,
                            l->urx, l->ury,
                            l->lrx, l->lry,
                            stiffness
                        )
                    );

                }
                else
                {
                    vertices.push_back
                    (
                        std::move
                        (
                            std::make_shared<MeshPoint>
                            (
                                c->x, c->y, c->r
                            )
                        )
                    );
                    p = std::make_shared<CollisionPrimitive>
                    (
                        CollisionPrimitive(c->x,c->y,c->r, stiffness, damping, mass)
                    );
                }

                worldVertices.push_back(std::move(p));
 
            }
            computeRadius();
        }
        
        size_t size(){return vertices.size();}

        std::shared_ptr<MeshPoint> getModelVertex(size_t i)
        {
            return vertices[i];
        }

        std::shared_ptr<CollisionPrimitive> operator[](size_t i) 
        {
            return worldVertices[i];
        }

        void updateWorldMesh(
            cTransform & transform,
            double dt,
            bool init = false
        )
        {
            if (isRigid())
            {
                return updateWorldMeshRigid(transform, dt, init);
            }
            else 
            {
                return updateWorldMeshSoft(transform, dt, init);
            }
        }

        void updateWorldMeshRigid(
            const cTransform & transform,
            double dt,
            bool init = false
        );

        void updateWorldMeshSoft(
            cTransform & transform,
            double dt,
            bool init = false
        );

        double bestAngle(double x, double y, double scale);
        void centerOfMassWorld(double & cx, double & cy);
        void modelToCenterOfMassFrame();

        double momentOfInertia(double x, double y);
        void computeRadius();
        double getRadius(){return radius;}

        bool isRigid(){ return rigid; }

        void applyForce(double fx, double fy)
        {
            for (auto w : worldVertices)
            {
                w->applyForce(fx, fy);
            }
        }
        
    private:

        std::vector<std::shared_ptr<MeshPoint>> vertices;
        std::vector<std::shared_ptr<CollisionPrimitive>> worldVertices;

        double radius;

        bool rigid = true;
    };

}

#endif /* COLLISIONMESH */
