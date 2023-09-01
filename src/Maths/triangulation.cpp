#include <Maths/triangulation.h>

namespace Hop::Maths
{
    Triangulation::Triangulation(const Polygon & p)
    {
        Polygon q(p);

        if (q.getHandedness() == HAND::LEFT){ q.reverse(); }

        triangles.clear();

        while (q.length() > 3)
        {
            Vertex ear = findEar(q, q, 0);

            unsigned earId = 0;

            std::vector<Vertex> newVertices;

            for (unsigned i = 0; i < q.length(); i++)
            {
                if (q.vertices[i] != ear)
                {
                    newVertices.push_back(q.vertices[i]);
                }
                else
                {
                    earId = i;
                }
            }

            triangles.push_back(q.centeredTriangle(earId));

            q.vertices = newVertices;
        }

        if (q.vertices.size() == 3)
        {
            triangles.push_back(Triangle(q.vertices[0], q.vertices[1], q.vertices[2]));
        }

    }
}