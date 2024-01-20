#ifndef COLLISIONMESH_H
#define COLLISIONMESH_H

#include <vector>
#include <cmath>
#include <limits>
#include <cstdint>
#include <memory>
#include <iostream>

#include <Maths/vertex.h>
#include <Maths/transform.h>
#include <Component/cTransform.h>
#include <Component/cPhysics.h>

using Hop::Maths::Vertex;
using Hop::Object::Component::cTransform;
using Hop::Object::Component::cPhysics;

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

        bool operator==(const MeshPoint & rhs)
        {
            return x == rhs.x && y == rhs.y && r == rhs.r;
        }
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

        bool operator==(const MeshRectangle & rhs)
        {
            return llx == rhs.llx && 
                   lly == rhs.lly &&
                   ulx == rhs.ulx &&
                   uly == rhs.uly &&
                   urx == rhs.urx &&
                   ury == rhs.ury &&
                   lrx == rhs.lrx &&
                   lry == rhs.lry;
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
          roxp(0.0), royp(0.0),
          stiffness(k), effectiveMass(m), damping(d)
        {}

        virtual ~CollisionPrimitive() = default;
        // x position, y position, radius (model space)
        //  of a collision point
        double x, y, r;
        uint8_t lastInside;

        double ox, oy;

        double fx, fy, xp, yp, vx, vy, roxp, royp;

        double stiffness = CollisionPrimitive::RIGID;
        double effectiveMass, damping;

        void setRecentlyInside(int i){ lastInside =  i; }
        bool recentlyInside() const { return lastInside > 0; }

        bool isRigid() { return stiffness >= RIGID; }

        void applyForce(double x, double y)
        {
            fx+=x;
            fy+=y;
        }

        double energy()
        {
            return effectiveMass*(vx*vx+vy*vy);
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

        void rotationalDamping
        (
            double omega,
            double damp,
            double cx,
            double cy
        )
        {
            /*
            
                Force fx, fy from a given torque T = [0,0,tau].

                Assume perpendicular then,

                    [rx, ry, 0] X [fx, fy, 0] X [rx, ry, 0] = 
                         r             F             r
                  - r X r X F =

                  - [(r.F)r-(r.r)F] = |r|^2 F

                So the force is F = T X r / |r|^2

                This is [-tau ry, tau rx, 0]

                Angular acceleration omega = tau * I = tau * mass * (radius^2 + |r|^2)

            */

            double rcx = x-cx;
            double rcy = y-cy;
            double rc2 = rcx*rcx + rcy*rcy;
            double tau = omega * effectiveMass * (0.5*r*r + rc2);

            applyForce
            (
                -damp * tau * rcy,
                 damp * tau * rcx
            );
        }

        void step
        (
            double dt,
            double dtdt,
            double translationalDrag,
            double nox,
            double noy
        )
        {

            double ct = translationalDrag*dt / (2.0*effectiveMass);
            double bt = 1.0/(1.0+ct);
            double at = (1.0-ct)*bt;

            ox = nox;
            oy = noy;

            double rox = x-ox;
            double roy = y-oy;

            // spring with relaxed state at ox, oy;
            double ax = (fx-stiffness*rox-damping*vx)/effectiveMass;
            double ay = (fy-stiffness*roy-damping*vy)/effectiveMass;

            fx = 0.0;
            fy = 0.0;

            double xtp = x;
            double ytp = y;

            x = 2.0*bt*x - at*xp + bt*ax*dtdt;
            y = 2.0*bt*y - at*yp + bt*ay*dtdt;

            xp = xtp;
            yp = ytp;

            vx = (rox-roxp)/(dt);
            vy = (roy-royp)/(dt);

            roxp = rox;
            royp = roy;

        }

        void stepGlobal
        (
            double dt,
            double dtdt,
            const cPhysics & physics,
            double gx,
            double gy,
            double & dx,
            double & dy
        )
        {

            double ct = physics.translationalDrag*dt / (2.0*physics.mass);
            double bt = 1.0/(1.0+ct);
            double at = (1.0-ct)*bt;
            // not impacted by damping forces on mesh

            double xtp = x;
            double ytp = y;

            double ax = gx/physics.mass;
            double ay = gy/physics.mass;

            x = 2.0*bt*x - at*xp + bt*ax*dtdt;
            y = 2.0*bt*y - at*yp + bt*ay*dtdt;
            dx = x-xtp;
            dy = y-ytp;
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
            effectiveMass = 1.0;
            xp = x;
            yp = y;
            ox = x;
            oy = y;
            vx = 0.0;
            vy = 0.0;
            fx = 0.0;
            fy = 0.0;
            roxp=0.0;
            royp=0.0;
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
            double scale
        )
        : CollisionMesh(std::move(v))
        {
            cTransform transform(x,y,theta,scale);
            cPhysics phys(x,y,theta);
            updateWorldMesh(transform, phys, 0.0);
        }

        CollisionMesh
        (
            std::vector<std::shared_ptr<CollisionPrimitive>> v
        )
        : totalEffectiveMass(1.0), gx(0.0), gy(0.0)
        {
            worldVertices.clear();
            for (unsigned i = 0; i < v.size(); i++)
            {
                add(v[i]);
            }
            computeRadius();
        }

        void transform(cTransform t)
        {
            needsInit = true;
            cPhysics phys(t.x,t.y,t.theta);
            updateWorldMesh(t, phys, 0.0);
        }
        
        void add(std::shared_ptr<CollisionPrimitive> c)
        {

            auto circ = std::make_shared<MeshPoint>
            (
                c->x, c->y, c->r
            );

            for (auto c : vertices)
            {
                if (*c.get() == *circ.get())
                {
                    return;
                }
            }

            Rectangle * l = dynamic_cast<Rectangle*>(c.get());

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
                        l->stiffness
                    )
                );

                someRectangles = true;

            }
            else
            {
                vertices.push_back
                (
                    std::move
                    (
                        circ
                    )
                );
                p = std::make_shared<CollisionPrimitive>
                (
                    CollisionPrimitive(c->x,c->y,c->r, c->stiffness, c->damping, c->effectiveMass)
                );
            }

            worldVertices.push_back(std::move(p));

            calculateIsRigid();
            calculateTotalEffectiveMass();

            needsInit = true;
        }

        void remove(size_t i)
        {
            vertices.erase(vertices.begin()+i);
            worldVertices.erase(worldVertices.begin()+i);
            needsInit = true;

            calculateIsRigid();
            calculateTotalEffectiveMass();
        }

        int clicked(float x, float y)
        {

            for (int j = 0; j < int(worldVertices.size()); j++)
            {
                double rx = worldVertices[j]->x - x;
                double ry = worldVertices[j]->y - y;
                double d2 = rx*rx+ry*ry;

                if (d2 < worldVertices[j]->r*worldVertices[j]->r)
                {
                    return j;
                }
            }

            return -1;
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
            cPhysics & physics,
            double dt
        )
        {
            kineticEnergy = 0.0;
            if (isRigid)
            {
                return updateWorldMeshRigid(transform, dt);
            }
            else 
            {
                return updateWorldMeshSoft(transform, physics, dt);
            }
        }

        void updateWorldMeshRigid(
            const cTransform & transform,
            double dt
        );

        void updateWorldMeshSoft(
            cTransform & transform,
            cPhysics & physics,
            double dt
        );

        double bestAngle(double x, double y, double scale);
        void centerOfMassWorld(double & cx, double & cy);
        void modelToCenterOfMassFrame();

        double momentOfInertia(double x, double y, double mass);
        void computeRadius();
        double getRadius(){return radius;}

        bool getIsRigid(){ return isRigid; }

        void calculateIsRigid() 
        {
            for (auto v : worldVertices)
            {
                if (v->stiffness < CollisionPrimitive::RIGID)
                {
                    isRigid = false;
                    return;
                }
            }
            isRigid = true;
        }

        void applyForce(double fx, double fy, bool global = false)
        {
            if (global)
            {
                gx += fx;
                gy += fy;
            }
            else 
            {
                for (auto w : worldVertices)
                {
                    w->applyForce(fx, fy);
                }
            }

        }

        double getEffectiveMass() const { return totalEffectiveMass; }

        void calculateTotalEffectiveMass() 
        {
            totalEffectiveMass = 0.0;
            for (auto v : worldVertices)
            {
                totalEffectiveMass += v->effectiveMass;
            }

            if (totalEffectiveMass <= 0.0)
            {
                totalEffectiveMass = 1.0;
            }
        }

        double energy()
        {
            return kineticEnergy;
        }
        
        bool areSomeRectangles() const { return someRectangles; }
        
    private:

        std::vector<std::shared_ptr<MeshPoint>> vertices;
        std::vector<std::shared_ptr<CollisionPrimitive>> worldVertices;

        double totalEffectiveMass;

        double radius;

        double gx, gy, kineticEnergy;

        bool isRigid = true;
        bool needsInit = false;
        bool someRectangles = false;
    };

}

#endif /* COLLISIONMESH_H */
