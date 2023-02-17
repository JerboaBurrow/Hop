#include <Collision/collisionMesh.h>

// Decorate a polygon defined by edges with circles of radius r
//  by placing circles inside the polygon that just touch the
//  perimeter
//      - tight corners will be poorly handled
//      - some circles will overlap (redundancy)
//      + quick
//      + generative
//      + all circles touch edges from inside (do not overextend past edges)
//      + smaller radius alleviates the -'s

CollisionMesh::CollisionMesh(std::vector<Vertex> v, double r)
{
    for (int i = 0; i < v.size(); i++)
    {

        Vertex n = v[(i+1)%v.size()];
        Vertex nt(n.y,-n.x);

        double d = norm(nt);
        nt = nt / d;

        if (d > 0)
        
        {
            int dm = std::floor(d/(2.0*r));

            double m = 0.0;
            for (int j = 0; j < dm+1; j++)
            {
                Vertex c = v[i]+n/d*m;
                if (1 < j && j < dm+1)
                {
                    // not a corner, move inwards
                    c += nt*r;
                }
                else if (m==0.0)
                {
                    // move circle so that it is tangent to both
                    // edges simultaneously
                    int va = i-1 < 0 ? v.size()-1 : i-1;
                    int vb = i;
                    int vc = (i+1)%v.size();
                    Vertex ray = angleBisectorRay(v[va],v[vb],v[vc]);
                    double theta = angle(v[va],v[vb],v[vc]);
                    c += ray/norm(ray)*(r/std::sin(theta/2.0));
                }
                else if (j==dm)
                {
                    // move circle so that it is tangent to both
                    // edges simultaneously
                    int va = i;
                    int vb = (i+1)%v.size();
                    int vc = (i+2)&v.size();
                    Vertex ray = angleBisectorRay(v[va],v[vb],v[vc]);
                    double theta = angle(v[va],v[vb],v[vc]);
                    c += ray/norm(ray)*(r/std::sin(theta/2.0));
                }

                vertices.push_back(
                    CollisionVertex(
                        c.x,c.y,r
                    )
                );
                
                m += 2.0*r;
            }
        }
    }
}

void CollisionMesh::updateWorldMesh(
    double x,
    double y,
    double theta, 
    double scale
)
{
    double c = std::cos(theta);
    double s = std::sin(theta);
    worldVertices = vertices;

    for (int i = 0; i < vertices.size(); i++)
    {
        worldVertices[i].x = (vertices[i].x * c + vertices[i].y*s)*scale+x;
        worldVertices[i].y = (vertices[i].y*c-vertices[i].x*s)*scale+y;
        worldVertices[i].r = vertices[i].r*scale;
    }
}

bool operator==(const CollisionVertex & lhs, const CollisionVertex & rhs)
{
    return lhs.x == rhs.x && lhs.x == rhs.y && lhs.r == rhs.r;
}