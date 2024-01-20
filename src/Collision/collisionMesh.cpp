#include <Collision/collisionMesh.h>


namespace Hop::System::Physics
{
    void CollisionMesh::updateWorldMeshRigid(
        const cTransform & transform,
        double dt
    )
    {
        double c = std::cos(transform.theta);
        double s = std::sin(transform.theta);

        std::vector<uint8_t> inside(worldVertices.size());

        for (unsigned i = 0; i < vertices.size(); i++)
        {
            inside[i] = worldVertices[i]->lastInside;
            worldVertices[i]->setOrigin
            (
                (vertices[i]->x*c + vertices[i]->y*s)*transform.scale + transform.x,
                (vertices[i]->y*c - vertices[i]->x*s)*transform.scale + transform.y
            );

            worldVertices[i]->r = vertices[i]->r*transform.scale;
            worldVertices[i]->lastInside = inside[i]; 
        }

        // break of into second loop to allow for vectorisation of the above loop
        // cf updateWorldMeshSoft
        if (someRectangles)
        {
            for (unsigned i = 0; i < vertices.size(); i++)
            {
                Rectangle * lw = dynamic_cast<Rectangle*>(worldVertices[i].get());
                MeshRectangle * lv = dynamic_cast<MeshRectangle*>(vertices[i].get());

                if (lw != nullptr && lv != nullptr)
                {
                    lw->llx = lv->llx;
                    lw->lly = lv->lly;

                    lw->ulx = lv->ulx;
                    lw->uly = lv->uly;

                    lw->urx = lv->urx;
                    lw->ury = lv->ury;

                    lw->lrx = lv->lrx;
                    lw->lry = lv->lry;

                    lw->x = lv->x;
                    lw->y = lv->y;
                    lw->r = lv->r;
                    
                    Hop::Maths::rotateClockWise(lw, c, s);
                    Hop::Maths::scale(lw, transform.scale*2.0);
                    Hop::Maths::translate(lw, transform.x, transform.y);

                }
            }
        }

        computeRadius();
        needsInit = false;
    }

    void CollisionMesh::centerOfMassWorld(double & cx, double & cy)
    {
        cx = 0.0;
        cy = 0.0;
        for (unsigned i = 0; i < worldVertices.size(); i++)
        {
            cx += worldVertices[i]->x;
            cy += worldVertices[i]->y;
        }
        cx /= double(worldVertices.size());
        cy /= double(worldVertices.size());
    }

    void CollisionMesh::modelToCenterOfMassFrame()
    {
        double cx = 0.0;
        double cy = 0.0;
        for (unsigned i = 0; i < vertices.size(); i++)
        {
            cx += vertices[i]->x;
            cy += vertices[i]->y;
        }
        cx /= double(vertices.size());
        cy /= double(vertices.size());

        for (unsigned i = 0; i < vertices.size(); i++)
        {
            vertices[i]->x -= cx;
            vertices[i]->y -= cy;
        }
    }

    double CollisionMesh::bestAngle(double x, double y, double scale)
    {
        double cx = 0.0;
        double cy = 0.0;
        centerOfMassWorld(cx, cy);
        double a = 0.0;
        double b = 0.0;
        double refx, refy;
        double rx, ry;

        for (unsigned i = 0; i < worldVertices.size(); i++)
        {
            refx = vertices[i]->x*scale + x;
            refy = vertices[i]->y*scale + y;
            rx = worldVertices[i]->x-cx;
            ry = worldVertices[i]->y-cy;

            b += rx*refx + ry*refy;
            a += rx*refy - ry*refx;
        }

        double omega = std::atan2(a, b);

        return omega;
    }

    void CollisionMesh::updateWorldMeshSoft(
        cTransform & transform,
        cPhysics & physics,
        double dt
    )
    {

        double c = std::cos(transform.theta);
        double s = std::sin(transform.theta);

        if (needsInit)
        {
            modelToCenterOfMassFrame();
            for (unsigned i = 0; i < vertices.size(); i++)
            {
                worldVertices[i]->setOrigin
                (
                    (vertices[i]->x*c + vertices[i]->y*s)*transform.scale + transform.x,
                    (vertices[i]->y*c - vertices[i]->x*s)*transform.scale + transform.y
                );
            }
        }

        double phi = bestAngle(transform.x, transform.y, transform.scale);

        // rotational speed
        physics.omega = (phi-transform.theta)/dt;
        transform.theta = phi;

        double co = std::cos(phi);
        double so = std::sin(phi);

        double dtdt = dt*dt;

        std::vector<uint8_t> inside(worldVertices.size());

        if (needsInit)
        {
            for (unsigned i = 0; i < vertices.size(); i++)
            {
                worldVertices[i]->setOrigin
                (
                    (vertices[i]->x*c + vertices[i]->y*s)*transform.scale + transform.x,
                    (vertices[i]->y*c - vertices[i]->x*s)*transform.scale + transform.y
                );
                worldVertices[i]->r = vertices[i]->r*transform.scale;
                worldVertices[i]->lastInside = inside[i]; 
            }
        }

        if (!needsInit)
        {
            // should be vectorisable, split with prior and next loop
            //  to make branchless. Indeed we got down to O(1e-7) from O(1e-6)
            //  on pure circles 
            for (unsigned i = 0; i < vertices.size(); i++)
            {
                inside[i] = worldVertices[i]->lastInside;

                worldVertices[i]->rotationalDamping(-physics.omega, physics.rotationalDrag, transform.x, transform.y);

                worldVertices[i]->step
                (
                    dt,
                    dtdt,
                    physics.translationalDrag,
                    (vertices[i]->x*co + vertices[i]->y*so)*transform.scale + transform.x,
                    (vertices[i]->y*co - vertices[i]->x*so)*transform.scale + transform.y
                );

                worldVertices[i]->r = vertices[i]->r*transform.scale;
                worldVertices[i]->lastInside = inside[i]; 
            }
        }

        if (someRectangles)
        {
            for (unsigned i = 0; i < vertices.size(); i++)
            {
                Rectangle * lw = dynamic_cast<Rectangle*>(worldVertices[i].get());
                MeshRectangle * lv = dynamic_cast<MeshRectangle*>(vertices[i].get());

                if (lw != nullptr && lv != nullptr)
                {
                    lw->llx = lv->llx;
                    lw->lly = lv->lly;

                    lw->ulx = lv->ulx;
                    lw->uly = lv->uly;

                    lw->urx = lv->urx;
                    lw->ury = lv->ury;

                    lw->lrx = lv->lrx;
                    lw->lry = lv->lry;

                    lw->x = lv->x;
                    lw->y = lv->y;
                    lw->r = lv->r;
                    
                    Hop::Maths::rotateClockWise(lw, c, s);
                    Hop::Maths::scale(lw, transform.scale*2.0);
                    Hop::Maths::translate(lw, transform.x, transform.y);

                }
            }
        }

        centerOfMassWorld(transform.x, transform.y);

        double dx = 0.0; 
        double dy = 0.0;
        for (unsigned i = 0; i < vertices.size(); i++)
        {
            worldVertices[i]->stepGlobal
            (
                dt, dtdt, physics, gx, gy, dx, dy
            );
        }

        gx = 0.0;
        gy = 0.0;

        transform.x += dx;
        transform.y += dy;

        kineticEnergy += (dx*dx+dy*dy)/(dt*dt);

        for (auto p : worldVertices)
        {
            kineticEnergy += p->vx*p->vx+p->vy*p->vy;
        }

        if (needsInit)
        {
            double c = std::cos(transform.theta);
            double s = std::sin(transform.theta);
            for (unsigned i = 0; i < vertices.size(); i++)
            {
                worldVertices[i]->setOrigin
                (
                    (vertices[i]->x*c + vertices[i]->y*s)*transform.scale + transform.x,
                    (vertices[i]->y*c - vertices[i]->x*s)*transform.scale + transform.y
                );
            }
        }

        computeRadius();
        needsInit = false;
    }

    void CollisionMesh::computeRadius()
    {
        double mx = 0.0;
        double Mx = 0.0;
        double my = 0.0;
        double My = 0.0;
        for (unsigned i = 0; i < vertices.size(); i++)
        {
            std::shared_ptr<CollisionPrimitive> p = worldVertices[i];
            if (i == 0){
                mx = p->x-p->r;
                Mx = p->x+p->r;
                my = p->y-p->r;
                My = p->y+p->r;
            }
            else
            {
                mx = std::min(p->x-p->r,mx);
                Mx = std::max(p->x+p->r,Mx);
                my = std::min(p->y-p->r,my);
                My = std::max(p->y+p->r,My);
            }
        }
        double x = Mx-mx;
        double y = My-my;
        this->radius = 0.5 * std::sqrt(x*x+y*y);
    }

    double CollisionMesh::momentOfInertia(double x, double y, double mass)
    {

        double m = 0.0;
        double dx = 0.0;
        double dy = 0.0;
        double me = mass/double(size());
        // apply composite area method
        // assume non-overlapping
        // assume unit effectiveMass for each piece
        for (unsigned i = 0; i < size(); i++)
        {
            std::shared_ptr<CollisionPrimitive> c = worldVertices[i];
            Rectangle * r = dynamic_cast<Rectangle*>(c.get());

            if (r == nullptr)
            {
                // an overestimate, ignore holes
                dx = c->x - x;
                dy = c->y - y; 
                m += me*0.5*c->r*c->r + me*(dx*dx+dy*dy);
            }
            else
            {  
                double h = r->height();
                double w = r->width();
                // an overestimate, ignore holes
                dx = c->x - x;
                dy = c->y - y; 
                m += me*0.08333333333333333 * (h*h+w*w) + me*(dx*dx+dy*dy);
            }
        }

        return m;
    }

    std::ostream & operator<<(std::ostream & o, Rectangle const & r)
    {
        o << r.ulx << ", " << r.uly << "    " << r.urx << ", " << r.ury << "\n"
          << r.llx << ", " << r.lly << "    " << r.lrx << ", " << r.lry << "\n"
          << r.x   << ", " << r.y   << ", "   << r.r << "\n"
          << r.axis1x << ", " << r.axis1y << ", " << r.axis2x << ", " << r.axis2y << "\n";

        return o;
    }
    

}