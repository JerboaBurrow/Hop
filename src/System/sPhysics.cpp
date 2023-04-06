#include <System/sPhysics.h>

#include <chrono>
using namespace std::chrono;

#include <thread>

namespace Hop::System::Physics
{

    void sPhysics::processThreaded(ObjectManager * m, size_t threadId)
    {
        double nx, ny, ntheta, ar, br, cr, at, bt, ct;
        double DT_OVER_TWICE_MASS = dt / (2.0*PARTICLE_MASS);

        for (auto it = threadJobs[threadId].begin(); it != threadJobs[threadId].end(); it++)
        {

            cTransform & dataT = m->getComponent<cTransform>(*it);
            cPhysics & dataP = m->getComponent<cPhysics>(*it);
            
            dataP.fy += -gravity*PARTICLE_MASS;

            ct = dataP.translationalDrag*DT_OVER_TWICE_MASS;
            bt = 1.0/(1.0+ct);
            at = (1.0-ct)*bt;

            nx = 2.0*bt*dataT.x - at*dataP.lastX + bt*dataP.fx*dtdt/PARTICLE_MASS;
            ny = 2.0*bt*dataT.y - at*dataP.lastY + bt*dataP.fy*dtdt/PARTICLE_MASS;

            dataP.vx = (nx-dataP.lastX)/(dt*2.0);
            dataP.vy = (ny-dataP.lastY)/(dt*2.0);

            dataP.lastX = dataT.x;
            dataP.lastY = dataT.y;

            dataP.fx = 0.0;
            dataP.fy = 0.0;

            cr = dt * dataP.rotationalDrag / (2.0*dataP.momentOfInertia);
            br = 1.0/(1.0+cr);
            ar = (1.0-cr)*br;

            ntheta = 2.0*br*dataT.theta-ar*dataP.lastTheta+br*dataP.omega*dtdt/dataP.momentOfInertia;

            dataP.phi = (ntheta-dataP.lastTheta)/2.0;

            dataP.lastTheta = dataT.theta;

            dataP.omega = 0.0;

            dataT.x = nx;
            dataT.y = ny;
            dataT.theta = ntheta;

            if (m->hasComponent<cCollideable>(*it))
            {
                cCollideable & data = m->getComponent<cCollideable>(*it);
                data.mesh.updateWorldMesh(
                    dataT.x,
                    dataT.y,
                    dataT.theta,
                    dataT.scale
                );
            }
        }
    }

    void sPhysics::updateThreaded(ObjectManager * m)
    {

        for (int j = 0; j < threadJobs.size(); j++)
        {
            m->postJob(
                std::bind(
                    &sPhysics::processThreaded,
                    this,
                    m,
                    j
                )
            );
        }
        m->waitForJobs();
    }

    void sPhysics::update(ObjectManager * m)
    {

        if (m->isThreaded())
        {
            updateThreaded(m);
            return;
        }

        double nx, ny, ntheta, ar, br, cr, at, bt, ct;
        unsigned k = 0;

        double DT_OVER_TWICE_MASS = dt / (2.0*PARTICLE_MASS);

        for (auto it = objects.begin(); it != objects.end(); it++)
        {
            cTransform & dataT = m->getComponent<cTransform>(*it);
            cPhysics & dataP = m->getComponent<cPhysics>(*it);

            dataP.fy += -gravity*PARTICLE_MASS;

            ct = dataP.translationalDrag * DT_OVER_TWICE_MASS;
            bt = 1.0/(1.0+ct);
            at = (1.0-ct)*bt;

            nx = 2.0*bt*dataT.x - at*dataP.lastX + bt*dataP.fx*dtdt/PARTICLE_MASS;
            ny = 2.0*bt*dataT.y - at*dataP.lastY + bt*dataP.fy*dtdt/PARTICLE_MASS;

            dataP.vx = (nx-dataP.lastX)/(dt*2.0);
            dataP.vy = (ny-dataP.lastY)/(dt*2.0);

            dataP.lastX = dataT.x;
            dataP.lastY = dataT.y;

            dataP.fx = 0.0;
            dataP.fy = 0.0;

            cr = dt * dataP.rotationalDrag / (2.0*dataP.momentOfInertia);
            br = 1.0/(1.0+cr);
            ar = (1.0-cr)*br;

            ntheta = 2.0*br*dataT.theta-ar*dataP.lastTheta+br*dataP.omega*dtdt/dataP.momentOfInertia;

            dataP.phi = (ntheta-dataP.lastTheta)/2.0;

            dataP.lastTheta = dataT.theta;

            dataP.omega = 0.0;

            dataT.x = nx;
            dataT.y = ny;
            dataT.theta = ntheta;

            if (m->hasComponent<cCollideable>(*it))
            {
                cCollideable & data = m->getComponent<cCollideable>(*it);
                data.mesh.updateWorldMesh(
                    dataT.x,
                    dataT.y,
                    dataT.theta,
                    dataT.scale
                );
            }
        }
    }

    void sPhysics::applyForce(
        ObjectManager * m,
        Id i,
        double x,
        double y,
        double fx,
        double fy
    )
    {
        cPhysics & dataP = m->getComponent<cPhysics>(i);
        cTransform & dataT = m->getComponent<cTransform>(i);

        dataP.fx += fx;
        dataP.fy += fy;

        if (dataT.x != x || dataT.y != y)
        {
            double rx = x-dataT.x;
            double ry = y-dataT.y;
            double tau = rx*fy-ry*fx;
            double r2 = rx*rx+ry*ry;
            dataP.omega -= tau/(PARTICLE_MASS*r2); 
        }
    }

    void sPhysics::applyForce(
        ObjectManager * m,
        double fx,
        double fy
    )
    {
        for (auto it = objects.begin(); it != objects.end(); it++)
        {
            cPhysics & dataP = m->getComponent<cPhysics>(*it);

            dataP.fx += fx;
            dataP.fy += fy;
        }
    }

    void sPhysics::stabaliseObjectParameters(ObjectManager * m)
    {
        for (auto it = objects.begin(); it != objects.end(); it++)
        {
            cTransform & dataT = m->getComponent<cTransform>(*it);
            cPhysics & dataP = m->getComponent<cPhysics>(*it);

            dataP.translationalDrag = 1.1*stableDragUnderdampedLangevinWithGravityUnitMass(
                dt,
                gravity,
                dataT.scale
            );
        }
    }

    /*

        Given particle of unit mass, under gravity, with a given simulation 
        timestep, what drag coefficient is the minimum that is numerically stable?

        I.e EQ of M for y(t), (Underdamped Langevin equation)
        
        mass*a(t) = -drag*v(t) + mass*gravity.

        Solve with integration factor to get

        v(t) = exp(-drag/mass * t) * (Const. + mass*gravity/drag) - mass*gravity/drag.

        A simulation (with collisions) is stable if the change in position is 
        in one timestep less than or equal to the object size.

        The max speed under free fall is |v(t)| as t -> infinity, which is

        |v(t)| -> mass*gravity/drag, as t -> infinity

        For a time step dt, and object size r, we need to satisfy

        dt*(mass*gravity/drag) ~ r,

        and for a unit mass
        
        drag ~ dt*gravity/r.

    */
    double sPhysics::stableDragUnderdampedLangevinWithGravityUnitMass(
        double dt,
        double gravity,
        double radius
    )
    {
        return (dt*1.05)*gravity/radius;
    }

}