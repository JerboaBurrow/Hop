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

    /**
     * @brief A primitive shape (circle) for collisions.
     * 
     */
    struct CollisionPrimitive 
    {

        static constexpr double RIGID = 1e6;

        CollisionPrimitive() = default;

        /**
         * @brief Construct a new Collision Primitive (circle)
         * 
         * @param x centre x coordinate
         * @param y centre y coordinate
         * @param r radius
         * @param t tag (for sub meshing)
         * @param k stiffness (default is rigid)
         * @param d damping (only for soft meshes)
         * @param m mass (only for soft meshes)
         */
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

        /**
         * @brief Applied a force generating the require torque.
         * 
         * @param omega torque.
         * @param cx reference point x.
         * @param cy reference point y.
         */
        void applyTorque
        (
            double omega,
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
                - tau * rcy,
                  tau * rcx
            );
        }

        /**
         * @brief Apply internal (spring/drag) forces to this primitive in a soft meshes.
         * 
         * @param dt timestep.
         * @param dtdt timestep^2.
         * @param translationalDrag translational drag coefficient.
         * @param nox new x coordinate.
         * @param noy new y coordinate.
         */
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

        /**
         * @brief Apply external forces to this mesh point in a soft mesh.
         * 
         * @param dt timestep.
         * @param dtdt timestep^2.
         * @param physics physics component.
         * @param gx global force in x.
         * @param gy global force in y.
         * @param dx resultant change in x.
         * @param dy resultant change in y.
         */
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

    /**
     * @brief A rectangular primitive collision shape.
     * 
     */
    struct RectanglePrimitive : public CollisionPrimitive 
    {
        RectanglePrimitive()
        : CollisionPrimitive(0.0,0.0,0.0,0,CollisionPrimitive::RIGID,0.0,0.0)
        {}

        /**
         * @brief Construct a new Rectangle Primitive from vertices.
         * 
         * @param llx lower left x.
         * @param lly lower left y.
         * @param ulx upper left x.
         * @param uly upper left y.
         * @param urx upper right x.
         * @param ury upper right y.
         * @param lrx lower right x.
         * @param lry lower right y.
         * @param t tag (for sub meshing)
         * @param k stiffness (default is rigid).
         */
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

        /**
         * @brief Convert to a Rectangle.
         * 
         * @return Hop::Maths::Rectangle 
         */
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

        /**
         * @brief Recalculate axes (vector for length and width).
         * 
         */
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

            double u = llx-ulx;
            double v = lly-uly;

            return std::sqrt(u*u+v*v);

        }

        double width()
        {

            double u = llx-lrx;
            double v = lly-lry;

            return std::sqrt(u*u+v*v);

        }

        /**
         * @brief Rotate clockwise from precomputed cosine and sine.
         * 
         * @param cosine cos(\theta)
         * @param sine sin(\theta)
         */
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

        void translate(double dx, double dy)
        {
            llx += dx;
            lly += dy;

            ulx += dx;
            uly += dy;

            urx += dx;
            ury += dy;

            lrx += dx;
            lry += dy;

            x += dx;
            y += dy;
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
