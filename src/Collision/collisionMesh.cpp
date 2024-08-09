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
                (vertices[i]->x*c + vertices[i]->y*s)*transform.scaleX + transform.x,
                (vertices[i]->y*c - vertices[i]->x*s)*transform.scaleY + transform.y
            );

            worldVertices[i]->r = vertices[i]->r*std::max(transform.scaleX, transform.scaleY);
            worldVertices[i]->lastInside = inside[i];
        }

        // break of into second loop to allow for vectorisation of the above loop
        // cf updateWorldMeshSoft
        if (someRectangles)
        {
            for (unsigned i = 0; i < vertices.size(); i++)
            {
                RectanglePrimitive * lw = dynamic_cast<RectanglePrimitive*>(worldVertices[i].get());
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

                    lw->rotateClockWise(c, s);
                    lw->scale(std::max(transform.scaleX, transform.scaleY)*2.0);
                    lw->translate(transform.x, transform.y);

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

    double CollisionMesh::bestAngle(double x, double y, double scaleX, double scaleY)
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
            refx = vertices[i]->x*scaleX + x;
            refy = vertices[i]->y*scaleY + y;
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
                    (vertices[i]->x*c + vertices[i]->y*s)*transform.scaleX + transform.x,
                    (vertices[i]->y*c - vertices[i]->x*s)*transform.scaleY + transform.y
                );
            }
        }

        double phi = bestAngle(transform.x, transform.y, transform.scaleX, transform.scaleY);

        physics.omega = Hop::Maths::angleDistanceAtan2<double>(transform.theta, phi);

        transform.theta = phi;

        double co = std::cos(phi);
        double so = std::sin(phi);

        double dtdt = dt*dt;

        physics.vx = -transform.x;
        physics.vy = -transform.y;

        std::vector<uint8_t> inside(worldVertices.size());

        if (needsInit)
        {
            for (unsigned i = 0; i < vertices.size(); i++)
            {
                worldVertices[i]->setOrigin
                (
                    (vertices[i]->x*c + vertices[i]->y*s)*transform.scaleX + transform.x,
                    (vertices[i]->y*c - vertices[i]->x*s)*transform.scaleY + transform.y
                );
                worldVertices[i]->r = vertices[i]->r*std::max(transform.scaleX, transform.scaleY);
                worldVertices[i]->lastInside = inside[i];
            }
        }

        if (!needsInit && physics.isMoveable)
        {
            // should be vectorisable, split with prior and next loop
            //  to make branchless. Indeed we got down to O(1e-7) from O(1e-6)
            //  on pure circles
            for (unsigned i = 0; i < vertices.size(); i++)
            {
                inside[i] = worldVertices[i]->lastInside;

                worldVertices[i]->applyTorque(physics.tau, transform.x, transform.y);

                worldVertices[i]->applyTorque(-physics.omega*physics.rotationalDrag, transform.x, transform.y);

                worldVertices[i]->step
                (
                    dt,
                    dtdt,
                    physics.translationalDrag,
                    (vertices[i]->x*co + vertices[i]->y*so)*transform.scaleX + transform.x,
                    (vertices[i]->y*co - vertices[i]->x*so)*transform.scaleY + transform.y
                );

                worldVertices[i]->r = vertices[i]->r*std::max(transform.scaleX, transform.scaleY);
                worldVertices[i]->lastInside = inside[i];
            }
        }

        if (someRectangles)
        {
            for (unsigned i = 0; i < vertices.size(); i++)
            {
                RectanglePrimitive * lw = dynamic_cast<RectanglePrimitive*>(worldVertices[i].get());
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

                    lw->rotateClockWise(c, s);
                    lw->scale(std::max(transform.scaleX, transform.scaleY)*2.0);
                    lw->translate(transform.x, transform.y);

                }
            }
        }

        centerOfMassWorld(transform.x, transform.y);

        if (physics.isMoveable)
        {
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

            physics.vx += transform.x;
            physics.vy += transform.y;

            physics.vx /= dt;
            physics.vy /= dt;
            kineticEnergy += (dx*dx+dy*dy)/(dt*dt);
        }

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
                    (vertices[i]->x*c + vertices[i]->y*s)*transform.scaleX + transform.x,
                    (vertices[i]->y*c - vertices[i]->x*s)*transform.scaleY + transform.y
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
        // assume homogeneous mass for each piece
        for (unsigned i = 0; i < size(); i++)
        {
            std::shared_ptr<CollisionPrimitive> c = worldVertices[i];
            RectanglePrimitive * r = dynamic_cast<RectanglePrimitive*>(c.get());

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

    std::ostream & operator<<(std::ostream & o, RectanglePrimitive const & r)
    {
        o << r.ulx << ", " << r.uly << "    " << r.urx << ", " << r.ury << "\n"
          << r.llx << ", " << r.lly << "    " << r.lrx << ", " << r.lry << "\n"
          << r.x   << ", " << r.y   << ", "   << r.r << "\n"
          << r.axis1x << ", " << r.axis1y << ", " << r.axis2x << ", " << r.axis2y << "\n";

        return o;
    }

}