#include <Collision/collisionMesh.h>

namespace Hop::System::Physics
{
// Decorate a polygon defined by edges with circles of radius r
//  by placing circles inside the polygon that just touch the
//  perimeter
//      - tight corners will be poorly handled
//      - some circles will overlap (redundancy)
//      + quick
//      + generative
//      + all circles touch edges from inside (do not overextend past edges)
//      + smaller radius alleviates the -'s

    // CollisionMesh::CollisionMesh(std::vector<Vertex> v, double r)
    // {
    //     for (int i = 0; i < v.size(); i++)
    //     {

    //         Vertex n = v[(i+1)%v.size()];
    //         Vertex nt(n.y,-n.x);

    //         double d = norm(nt);
    //         nt = nt / d;

    //         if (d > 0)
            
    //         {
    //             int dm = std::floor(d/(2.0*r));

    //             double m = 0.0;
    //             for (int j = 0; j < dm+1; j++)
    //             {
    //                 Vertex c = v[i]+n/d*m;
    //                 if (1 < j && j < dm+1)
    //                 {
    //                     // not a corner, move inwards
    //                     c += nt*r;
    //                 }
    //                 else if (m==0.0)
    //                 {
    //                     // move circle so that it is tangent to both
    //                     // edges simultaneously
    //                     int va = i-1 < 0 ? v.size()-1 : i-1;
    //                     int vb = i;
    //                     int vc = (i+1)%v.size();
    //                     Vertex ray = angleBisectorRay(v[va],v[vb],v[vc]);
    //                     double theta = angle(v[va],v[vb],v[vc]);
    //                     c += ray/norm(ray)*(r/std::sin(theta/2.0));
    //                 }
    //                 else if (j==dm)
    //                 {
    //                     // move circle so that it is tangent to both
    //                     // edges simultaneously
    //                     int va = i;
    //                     int vb = (i+1)%v.size();
    //                     int vc = (i+2)&v.size();
    //                     Vertex ray = angleBisectorRay(v[va],v[vb],v[vc]);
    //                     double theta = angle(v[va],v[vb],v[vc]);
    //                     c += ray/norm(ray)*(r/std::sin(theta/2.0));
    //                 }

    //                 vertices.push_back(
    //                     CollisionVertex(
    //                         c.x,c.y,r
    //                     )
    //                 );
                    
    //                 m += 2.0*r;
    //             }
    //         }
    //     }
    // }

    void CollisionMesh::updateWorldMesh(
        double x,
        double y,
        double theta, 
        double scale
    )
    {
        this->x = x;
        this->y = y;
        this->theta = theta;
        this->scale = scale;

        double c = std::cos(theta);
        double s = std::sin(theta);
        std::vector<uint8_t> inside(worldVertices.size());

        for (unsigned i = 0; i < worldVertices.size(); i++){
            inside[i] = worldVertices[i]->lastInside;
            *(worldVertices[i].get()) = *(vertices[i].get());
        }

        for (int i = 0; i < vertices.size(); i++)
        {
            worldVertices[i]->x = (vertices[i]->x * c + vertices[i]->y*s)*scale+x;
            worldVertices[i]->y = (vertices[i]->y*c-vertices[i]->x*s)*scale+y;
            worldVertices[i]->r = vertices[i]->r*scale;
            worldVertices[i]->lastInside = inside[i]; 

            Rectangle * lw = dynamic_cast<Rectangle*>(worldVertices[i].get());
            Rectangle * lv = dynamic_cast<Rectangle*>(vertices[i].get());

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
                Hop::Maths::scale(lw, scale*2.0);
                Hop::Maths::translate(lw, x, y);

            }
        }

        computeRadius();
    }

    void CollisionMesh::computeRadius()
    {
        double mx, Mx, my, My;
        for (int i = 0; i < vertices.size(); i++)
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

    double CollisionMesh::momentOfInertia()
    {

        double m = 0.0;
        // apply composite area method
        // assume non-overlapping
        // assume unit mass for each piece
        for (unsigned i = 0; i < size(); i++)
        {
            std::shared_ptr<CollisionPrimitive> c = worldVertices[i];
            Rectangle * r = dynamic_cast<Rectangle*>(c.get());

            if (r == nullptr)
            {
                m += 0.5*c->r*c->r;
            }
            else
            {  
                double h = r->height();
                double w = r->width();

                m += 0.08333333333333333 * (h*h+w*w);
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