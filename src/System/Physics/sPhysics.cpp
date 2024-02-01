#include <System/Physics/sPhysics.h>

#include <chrono>
using namespace std::chrono;
        
namespace Hop::System::Physics
{

    void sPhysics::step
    (
        EntityComponentSystem * m, 
        sCollision * collisions,
        AbstractWorld * world
    )
    {
        for (unsigned k = 0 ; k < subSamples; k++)
        {

            collisions->update(m, world);
            gravityForce(m);
            update(m);
        }
    }

    void sPhysics::update(EntityComponentSystem * m, ThreadPool * workers)
    {

        /*
        
            The algorithm we employ - https://www.tandfonline.com/doi/abs/10.1080/00268976.2012.760055
                
                It is verlet integration (velocity free) that correctly handles drag and noise (Langevin dynamics)
                
                Noise is currently unapplied, translation/rotational drag is which can both be 0 without issue,
                where the algorithm becomes verlet as we know it.
                
                This models an object moving within a fluid.

                Soft bodies perform their own integration due to the nature of their movable mesh points.
        
        */

        ComponentArray<cCollideable> & collideables = m->getComponentArray<cCollideable>();
        ComponentArray<cPhysics> & physics = m->getComponentArray<cPhysics>();
        ComponentArray<cTransform> & transforms = m->getComponentArray<cTransform>();

        double nx, ny, ntheta, at, bt, ct, r, dx, dy, d, ar, br, cr;

        energy = 0.0;

        bool rigid = false;

        for (auto it = objects.begin(); it != objects.end(); it++)
        {
            r = 0.0;
            cTransform & dataT = transforms.get(*it);
            cPhysics & dataP = physics.get(*it);

            if (collideables.hasComponent(*it))
            {
                cCollideable & data = collideables.get(*it);
                rigid = data.mesh.getIsRigid();
            }

            if (!dataP.isMoveable)
            {
                dataP.fx = 0.0;
                dataP.fy = 0.0;
                dataP.vx = 0.0;
                dataP.vy = 0.0;
            }
            else if (rigid)
            {
                
                ct = dataP.translationalDrag*dt/(2.0*dataP.mass);
                bt = 1.0/(1.0+ct);
                at = (1.0-ct)*bt;

                nx = 2.0*bt*dataT.x - at*dataP.lastX + bt*dataP.fx*dtdt/dataP.mass;
                ny = 2.0*bt*dataT.y - at*dataP.lastY + bt*dataP.fy*dtdt/dataP.mass;

                if (r > 0)
                {
                    dx = nx - dataT.x;
                    dy = ny - dataT.y;

                    d = std::sqrt(dx*dx+dy*dy);

                    if (d > r*movementLimitRadii)
                    {
                        nx = dataT.x + r*movementLimitRadii * dx / d;
                        ny = dataT.y + r*movementLimitRadii * dy / d;
                    }
                }

                dataP.vx = (nx-dataP.lastX)/(dt*2.0);
                dataP.vy = (ny-dataP.lastY)/(dt*2.0);

                energy += dataP.vx * dataP.vx + dataP.vy * dataP.vy;

                dataP.lastX = dataT.x;
                dataP.lastY = dataT.y;

                dataP.fx = 0.0;
                dataP.fy = 0.0;

                dataP.omega += dataP.tau;

                cr = dt * dataP.rotationalDrag / (2.0*dataP.momentOfInertia);
                br = 1.0/(1.0+cr);
                ar = (1.0-cr)*br;

                ntheta = 2.0*br*dataT.theta-ar*dataP.lastTheta+br*dataP.omega*dtdt/dataP.momentOfInertia;

                dataP.phi = (ntheta-dataP.lastTheta)/(2.0*dt);

                dataP.lastTheta = dataT.theta;
                
                dataP.x = nx;
                dataP.y = ny;

                dataT.x = nx;
                dataT.y = ny;
                dataT.theta = ntheta;

            }

            if (collideables.hasComponent(*it))
            {
                cCollideable & data = collideables.get(*it);
                data.updateMesh(
                    dataT,
                    dataP,
                    dt
                );
                dataP.momentOfInertia = data.mesh.momentOfInertia(dataT.x, dataT.y, dataP.mass);
                energy += data.mesh.energy();
            }

            dataP.fx = 0.0;
            dataP.fy = 0.0;
            dataP.tau = 0.0;
            dataP.omega = 0.0;
        }
    }

    void sPhysics::gravityForce
    (
        EntityComponentSystem * m
    )
    {

        double fx = ngx*gravity; double fy = ngy*gravity;

        ComponentArray<cCollideable> & collideables = m->getComponentArray<cCollideable>();

        for (auto it = objects.begin(); it != objects.end(); it++)
        {

            if (collideables.hasComponent(*it))
            {
                cCollideable & data = collideables.get(*it);
                if (!data.mesh.getIsRigid())
                {   
                    data.mesh.applyForce(fx, fy, true);
                    continue;
                }
            }
            cPhysics & dataP = m->getComponent<cPhysics>(it->id);

            dataP.fx += fx;
            dataP.fy += fy;
            
        }

    }

    void sPhysics::applyForce(
        EntityComponentSystem * m,
        Id & i,
        double fx,
        double fy,
        bool global
    )
    {

        cPhysics & dataP = m->getComponent<cPhysics>(i);
        ComponentArray<cCollideable> & collideables = m->getComponentArray<cCollideable>();

        if (collideables.hasComponent(i))
        {
            cCollideable & data = collideables.get(i);
            if (!data.mesh.getIsRigid())
            {   
                data.mesh.applyForce(fx, fy, global);
            }
            else
            {
                dataP.fx += fx;
                dataP.fy += fy;
            }
        }
        else
        {
            dataP.fx += fx;
            dataP.fy += fy;
        }

    }

    void sPhysics::applyTorque(
        EntityComponentSystem * m,
        Id & i,
        double tau
    )
    {
        cPhysics & dataP = m->getComponent<cPhysics>(i);

        if (m->hasComponent<cCollideable>(i))
        {
            cCollideable & c = m->getComponent<cCollideable>(i);

            if (c.mesh.getIsRigid())
            {
                dataP.tau += tau * dataP.momentOfInertia;
            }
            else
            {
                dataP.tau += tau;
            }
        }
        else
        {
            dataP.tau += tau;
        }

    }

    void sPhysics::applyForce(
        EntityComponentSystem * m,
        double fx,
        double fy,
        bool global
    )
    {

        ComponentArray<cCollideable> & collideables = m->getComponentArray<cCollideable>();

        for (auto it = objects.begin(); it != objects.end(); it++)
        {

            if (collideables.hasComponent(*it))
            {
                cCollideable & data = collideables.get(*it);
                if (!data.mesh.getIsRigid())
                {   
                    data.mesh.applyForce(fx, fy, global);
                    continue;
                }
            }

            cPhysics & dataP = m->getComponent<cPhysics>(*it);

            dataP.fx += fx;
            dataP.fy += fy;
        }
    }

    void sPhysics::stabaliseObjectParameters(EntityComponentSystem * m)
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

#include <System/Physics/LuaBindings/lua_physics.cpp>