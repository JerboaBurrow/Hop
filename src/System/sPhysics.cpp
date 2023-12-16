#include <System/sPhysics.h>

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

        ComponentArray<cCollideable> & collideables = m->getComponentArray<cCollideable>();
        ComponentArray<cPhysics> & physics = m->getComponentArray<cPhysics>();
        ComponentArray<cTransform> & transforms = m->getComponentArray<cTransform>();

        double nx, ny, ntheta, at, bt, ct, sticktion, r, dx, dy, d, ar, br, cr;

        energy = 0.0;

        double DT_OVER_TWICE_MASS = dt / (2.0*PARTICLE_MASS);

        bool rigid = false;

        for (auto it = objects.begin(); it != objects.end(); it++)
        {
            r = 0.0;
            cTransform & dataT = transforms.get(*it);
            cPhysics & dataP = physics.get(*it);

            if (collideables.hasComponent(*it))
            {
                cCollideable & data = collideables.get(*it);
                rigid = data.mesh.isRigid();
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
                
                ct = dataP.translationalDrag*DT_OVER_TWICE_MASS;
                bt = 1.0/(1.0+ct);
                at = (1.0-ct)*bt;

                sticktion = std::sqrt(dataP.fx*dataP.fx+dataP.fy*dataP.fy);

                if (sticktion > 0.0){

                    nx = 2.0*bt*dataT.x - at*dataP.lastX + bt*dataP.fx*dtdt/PARTICLE_MASS;
                    ny = 2.0*bt*dataT.y - at*dataP.lastY + bt*dataP.fy*dtdt/PARTICLE_MASS;

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

                }
                else{
                    nx = dataT.x;
                    ny = dataT.y;
                    dataP.lastX = dataT.x;
                    dataP.lastY = dataT.y;
                }

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

                dataP.fx = 0.0;
                dataP.fy = 0.0;
                dataP.tau = 0.0;
                dataP.omega = 0.0;

            }

            if (collideables.hasComponent(*it))
            {
                cCollideable & data = collideables.get(*it);
                data.updateMesh(
                    dataT,
                    dataP,
                    dt
                );
                dataP.momentOfInertia = data.mesh.momentOfInertia(dataT.x, dataT.y)*PARTICLE_MASS;
                energy += data.mesh.energy();
            }
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
                if (!data.mesh.isRigid())
                {   
                    data.mesh.applyForce(fx*data.mesh.getMass(), fy*data.mesh.getMass(), true);
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
        double fy
    )
    {

        cPhysics & dataP = m->getComponent<cPhysics>(i);
        ComponentArray<cCollideable> & collideables = m->getComponentArray<cCollideable>();

        if (collideables.hasComponent(i))
        {
            cCollideable & data = collideables.get(i);
            if (!data.mesh.isRigid())
            {   
                data.mesh.applyForce(fx, fy);
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

    void sPhysics::applyForce(
        EntityComponentSystem * m,
        double fx,
        double fy
    )
    {

        ComponentArray<cCollideable> & collideables = m->getComponentArray<cCollideable>();

        for (auto it = objects.begin(); it != objects.end(); it++)
        {

            if (collideables.hasComponent(*it))
            {
                cCollideable & data = collideables.get(*it);
                if (!data.mesh.isRigid())
                {   
                    data.mesh.applyForce(fx, fy);
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