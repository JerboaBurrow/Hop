#include <System/sPhysics.h>

#include <chrono>
using namespace std::chrono;

#include <thread>

namespace Hop::System::Physics
{

    void sPhysics::processThreaded(
        ComponentArray<cCollideable> & collideables,
        ComponentArray<cPhysics> & physics,
        ComponentArray<cTransform> & transforms,
        Id * jobs,
        unsigned njobs
    )
    {
        double nx, ny, ntheta, ar, br, cr, at, bt, ct;
        double DT_OVER_TWICE_MASS = dt / (2.0*PARTICLE_MASS);

        for (unsigned i = 0; i < njobs; i++)
        {

            Id id = jobs[i];

            cTransform & dataT = transforms.get(id);
            cPhysics & dataP = physics.get(id);

            if (!dataP.isMoveable)
            {
                dataP.fx = 0.0;
                dataP.fy = 0.0;
                dataP.vx = 0.0;
                dataP.vy = 0.0;
            }
            else
            {

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

            }
        
            if (collideables.hasComponent(id))
            {
                cCollideable & data = collideables.get(id);
                data.mesh.updateWorldMesh(
                    dataT.x,
                    dataT.y,
                    dataT.theta,
                    dataT.scale
                );
            }
        }
    }

    void sPhysics::updateThreaded(EntityComponentSystem * m,  ThreadPool * workers)
    {

        ComponentArray<cCollideable> & collideables = m->getComponentArray<cCollideable>();
        ComponentArray<cPhysics> & physics = m->getComponentArray<cPhysics>();
        ComponentArray<cTransform> & transforms = m->getComponentArray<cTransform>();

        unsigned nThreads = workers->size();
        unsigned jobsPerThread = std::floor(objects.size()/nThreads);
        Id jobs[nThreads][jobsPerThread];

        auto iter = objects.begin();
        for (unsigned t = 0; t < nThreads; t++)
        {
            for (unsigned j = t*jobsPerThread; j < (t+1)*jobsPerThread; j++)
            {
                jobs[t][j] = *iter;
                iter++;
            }
        }

        for (unsigned j = 0; j < nThreads; j++)
        {
            workers->queueJob(
                std::bind(
                    &sPhysics::processThreaded,
                    this,
                    collideables,
                    physics,
                    transforms,
                    &jobs[j][0],
                    jobsPerThread
                )
            );
        }
        workers->wait();
    }

    void sPhysics::update(EntityComponentSystem * m, ThreadPool * workers)
    {

        // if (workers != nullptr)
        // {
        //     updateThreaded(m, workers);
        //     return;
        // }

        ComponentArray<cCollideable> & collideables = m->getComponentArray<cCollideable>();
        ComponentArray<cPhysics> & physics = m->getComponentArray<cPhysics>();
        ComponentArray<cTransform> & transforms = m->getComponentArray<cTransform>();

        double nx, ny, ntheta, at, bt, ct, sticktion, k, r, dx, dy, d;

        double DT_OVER_TWICE_MASS = dt / (2.0*PARTICLE_MASS);

        for (auto it = objects.begin(); it != objects.end(); it++)
        {
            r = 0.0;
            cTransform & dataT = transforms.get(*it);
            cPhysics & dataP = physics.get(*it);

            if (!dataP.isMoveable)
            {
                dataP.fx = 0.0;
                dataP.fy = 0.0;
                dataP.vx = 0.0;
                dataP.vy = 0.0;
            }
            else
            {
                
                ct = dataP.translationalDrag*DT_OVER_TWICE_MASS;
                bt = 1.0/(1.0+ct);
                at = (1.0-ct)*bt;

                if (collideables.hasComponent(*it))
                {
                    cCollideable & data = collideables.get(*it);
                    r = data.mesh.getRadius();
                    dataP.tau -= dataP.phi * 50.0 * r*r;
                }

                sticktion = std::sqrt(dataP.fx*dataP.fx+dataP.fy*dataP.fy);

                if (sticktion > 0.0){

                    nx = 2.0*bt*dataT.x - at*dataP.lastX + bt*dataP.fx*dtdt/PARTICLE_MASS;
                    ny = 2.0*bt*dataT.y - at*dataP.lastY + bt*dataP.fy*dtdt/PARTICLE_MASS;

                    // nx = 2.0*dataT.x - dataP.lastX + dataP.fx*dtdt / PARTICLE_MASS;
                    // ny = 2.0*dataT.y - dataP.lastY + dataP.fy*dtdt / PARTICLE_MASS;

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

                // cr = dt * dataP.rotationalDrag / (2.0*dataP.momentOfInertia);
                // br = 1.0/(1.0+cr);
                // ar = (1.0-cr)*br;

                // std::cout << dataP.omega << "\n";
                // if (sticktion < 9.81){
                //     dataP.omega *= std::min((dataP.omega * dataP.omega), 1.0);
                // }

                ntheta = 2.0*dataT.theta - dataP.lastTheta + dtdt*dataP.omega/dataP.momentOfInertia;

                // ntheta = 2.0*br*dataT.theta-ar*dataP.lastTheta+br*dataP.omega*dtdt/dataP.momentOfInertia;

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
                data.mesh.updateWorldMesh(
                    dataT.x,
                    dataT.y,
                    dataT.theta,
                    dataT.scale
                );
                dataP.momentOfInertia = data.mesh.momentOfInertia()*PARTICLE_MASS;
            }
        }
    }

    void sPhysics::gravityForce
    (
        EntityComponentSystem * m,
        double g,
        double nx,
        double ny
    )
    {

        double fx = nx*g; double fy = ny*g;
        double rx, ry;

        for (auto it = objects.begin(); it != objects.end(); it++)
        {

            cPhysics & dataP = m->getComponent<cPhysics>(it->id);
            cTransform & dataT = m->getComponent<cTransform>(it->id);

            dataP.fx += fx;
            dataP.fy += fy;

            // if (m->hasComponent<cCollideable>(it->id))
            // {
            //     cCollideable & dataC = m->getComponent<cCollideable>(it->id);

            //     for (unsigned i = 0; i < dataC.mesh.size(); i++)
            //     {
            //         std::shared_ptr<CollisionPrimitive> p = dataC.mesh[i];
            //         Rectangle * r = dynamic_cast<Rectangle*>(p.get());

            //         if (r == nullptr)
            //         {
            //             dataP.fx += fx;
            //             dataP.fy += fy;
            //         }
            //         else
            //         {
            //             dataP.fx += fx;
            //             dataP.fy += fy;
            //         }
            //     }
            // }
        }

    }

    void sPhysics::applyForce(
        EntityComponentSystem * m,
        Id & i,
        double x,
        double y,
        double fx,
        double fy
    )
    {
        double rx, ry;

        cPhysics & dataP = m->getComponent<cPhysics>(i);
        cTransform & dataT = m->getComponent<cTransform>(i);

        dataP.fx += fx;
        dataP.fy += fy;

        rx = x - dataT.x;
        ry = y - dataT.y;

        dataP.omega += (rx*fy-ry*fx)/PARTICLE_MASS;

    }

    void sPhysics::applyForce(
        EntityComponentSystem * m,
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