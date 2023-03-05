#include <Maths/topology.h>

namespace Hop::Maths
{
    bool pointInTriangle(Vertex v, Triangle tri)
    {
        Vertex a = tri.c - tri.a;
        Vertex b = tri.b - tri.a;
        Vertex c = v - tri.a;

        float aDOTa = dot(a,a);
        float aDOTb = dot(a,b);
        float aDOTc = dot(a,c);
        float bDOTb = dot(b,b);
        float bDOTc = dot(b,c);

        float inv = 1.0 / (aDOTa*bDOTb-aDOTb*aDOTb);
        float u = (bDOTb*aDOTc-aDOTb*bDOTc)*inv;

        if (u < 0) 
        {
            return false;
        }

        float t = (aDOTa*bDOTc-aDOTb*aDOTc)*inv;

        if (t >= 0 && (u+t < 1)) 
        {
            return true;
        }

        return false;
    }
}
