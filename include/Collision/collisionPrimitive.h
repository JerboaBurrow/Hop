#ifndef COLLISIONPRIMITIVE_H
#define COLLISIONPRIMITIVE_H

#include <cstdint>
#include <cmath>
#include <iostream>

#include <Maths/vertex.h>
#include <Maths/rectangle.h>
#include <Maths/transform.h>
#include <Component/cPhysics.h>

namespace Hop::System::Physics
{

    using Hop::Object::Component::cPhysics;
    using Hop::Maths::Vertex;

    const uint64_t LAST_INSIDE_COUNTER_MAX = 60;

    struct CollisionPrimitive 
    {

        static constexpr double RIGID = 1e6;

        CollisionPrimitive() = default;

        CollisionPrimitive
        (
            double x, 
            double y, 
            double r, 
            uint64_t t = 0,
            double k = CollisionPrimitive::RIGID,
            double d = 1.0,
            double m = 1.0
        )
        : x(x),y(y),r(r),lastInside(0),
          ox(x),oy(y),fx(0),fy(0),
          xp(x),yp(y),
          vx(0),vy(0),
          roxp(0.0), royp(0.0),
          stiffness(k), effectiveMass(m), damping(d), tag(t)
        {}

        virtual ~CollisionPrimitive() = default;
        // x position, y position, radius (model space)
        //  of a collision point
        double x, y, r;
        uint64_t lastInside;

        double ox, oy;

        double fx, fy, xp, yp, vx, vy, roxp, royp;

        double stiffness = CollisionPrimitive::RIGID;
        double effectiveMass, damping;

        uint64_t tag;

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

    struct RectanglePrimitive : public CollisionPrimitive 
    {
        RectanglePrimitive()
        : RectanglePrimitive(0.0, 0.0,
                    0.0, 0.0,
                    0.0, 0.0,
                    0.0, 0.0)
        {
            stiffness = CollisionPrimitive::RIGID;
        }

        RectanglePrimitive
        (        
            double llx, double lly,
            double ulx, double uly,
            double urx, double ury,
            double lrx, double lry,
            uint64_t t = 0,
            double k = CollisionPrimitive::RIGID
        )
        :
          llx(llx), lly(lly),
          ulx(ulx), uly(uly),
          urx(urx), ury(ury),
          lrx(lrx), lry(lry)
        {
            tag = t;

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

        Hop::Maths::Rectangle getRect()
        {
            return Hop::Maths::Rectangle
            (
                Vertex(llx, lly),
                Vertex(ulx, uly),
                Vertex(urx, ury),
                Vertex(lrx, lry),
                Vertex(axis1x, axis1y),
                Vertex(axis2x, axis2y)
            );
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

        void rotateClockWise(double cosine, double sine)
        {
            Hop::Maths::rotateClockWise<double>(llx, lly, cosine, sine);
            Hop::Maths::rotateClockWise<double>(ulx, uly, cosine, sine);
            Hop::Maths::rotateClockWise<double>(urx, ury, cosine, sine);
            Hop::Maths::rotateClockWise<double>(lrx, lry, cosine, sine);

            Hop::Maths::rotateClockWise<double>(x,y,cosine,sine);

            resetAxes();
        }

        void scale(double s)
        {
            llx *= s;
            lly *= s;

            ulx *= s;
            uly *= s;

            urx *= s;
            ury *= s;

            lrx *= s;
            lry *= s;

            x *= s;
            y *= s;

            r *= s;
        }

        void translate(double x, double y)
        {
            llx += x;
            lly += y;

            ulx += x;
            uly += y;

            urx += x;
            ury += y;

            lrx += x;
            lry += y;

            x += x;
            y += y;
        }

        double llx, lly;
        double ulx, uly;
        double urx, ury;
        double lrx, lry;
        double axis1x, axis1y;
        double axis2x, axis2y;
    };

    std::ostream & operator<<(std::ostream & o, RectanglePrimitive const & r);

}

#endif /* COLLISIONPRIMITIVE_H */
