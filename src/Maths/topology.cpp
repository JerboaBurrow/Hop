#include <Maths/topology.h>

namespace Hop::Maths
{
    Vertex lineLineSegementIntersection(Vertex p, Vertex q, Vertex u, Vertex v)
    {
        Vertex a = p - u;
        Vertex b = v - u;
        Vertex c = Vertex(-q.y,q.x);

        double d = dot(b, c);
        double s = cross(b, a) / d;

        if (s < 0.0)
        {
            return Vertex(0.0, 0.0);
        }

        double t = dot(a, c)/d;

        if (0.0 <= t && t <= 1.0)
        {
            return p + s*q;
        }

        return Vertex(0.0, 0.0);
    }

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

        if (t >= 0 && (u+t <= 1)) 
        {
            return true;
        }

        return false;
    }

    Vertex findEar(const Polygon & p, const Polygon & q, unsigned i)
    {
        std::vector<Vertex> v = q.getVertices();

        if (isEar(p, v[i]))
        {
            return v[i];
        }

        Line diagonal = findDiagonal(q, i);

        int j = -1;

        for (unsigned k = 0; k < v.size(); k++)
        {
            if (v[k] == diagonal.b)
            {
                j = k;
                break;
            }
        }

        if (i == j)
        {
            return v[i];
        }

        Polygon gsp = goodSubPolygon(p, q, i, j);

        return findEar(p, gsp, unsigned(std::floor(float(gsp.length())/2.0f)));
    }

    bool isEar(const Polygon & p, unsigned i)
    {
        if (p.angleSign(i) == HAND::LEFT) { return false; }

        Triangle triVertices = p.centeredTriangle(i);
        Triangle tri = Triangle(triVertices.a, triVertices.b, triVertices.c);
        std::vector<Vertex> vertices = p.getVertices();

        for (Vertex v : vertices)
        {
            if (v != tri.a && v != tri.b && v != tri.c)
            {
                if (pointInTriangle(v,tri))
                {
                    return false;
                }
            }
        }

        return true;
    }

    bool isEar(const Polygon & p, const Vertex & v)
    {
        std::vector<Vertex> vertices = p.getVertices();

        for (unsigned i = 0; i < vertices.size(); i++)
        {
            if (v == vertices[i])
            {
                return isEar(p, i);
            }
        }
        return false;
    }

    Line findDiagonal(const Polygon & p, unsigned i)
    {
        unsigned n = p.length();
        Triangle abc = p.centeredTriangle(i);

        Vertex a = abc.a;
        Vertex b = abc.b;
        Vertex c = abc.c;

        Vertex ray = angleBisectorRay(a, b, c);

        if (p.angleSign(i) == HAND::LEFT)
        {
            ray = -1.0 * ray;
        }

        std::vector<Line> pe = p.edges();
        unsigned index = p.next(i);
        Line edge = pe[index];

        unsigned seen = 0;

        Vertex intersection = lineLineSegementIntersection(b, ray, edge.a, edge.b);

        if (intersection == Vertex(0.0, 0.0))
        {
            seen += 1;
            index = p.next(index);
        }

        while (seen < pe.size() && intersection == Vertex(0.0, 0.0))
        {
            edge = pe[index];
            intersection = lineLineSegementIntersection(b, ray, edge.a, edge.b);
            if (intersection == Vertex(0.0, 0.0))
            {
                seen += 1;
                index = p.next(index);
            }
            else
            {
                break;
            }
        }

        const std::vector<Vertex> & pv = p.getVertices();

        for (const Vertex & v : pv)
        {
            if (v == intersection)
            {
                return Line(pv[i], v);
            }
        }

        Vertex pk = edge.a;
        Vertex pk1 = edge.b;

        std::vector<unsigned> R;
        unsigned s = p.next(index+1);

        Triangle tri(b, intersection, pk1);

        while (s != i)
        {
            Vertex point = pv[s];
            if (point != b && point != intersection && point != pk1)
            {
                if (pointInTriangle(point, tri))
                {
                    R.push_back(s);
                }
            }
            s = p.next(s);
        }

        if (R.size() == 0)
        {
            if (pk1 != a)
            {
                return Line(pv[i], pv[p.next(index)]);
            }
        }
        else
        {
            std::vector<double> thetas(R.size(), 0.0);
            for (unsigned j = 0; j < R.size(); j++)
            {
                thetas[j] = angle(intersection, b, pv[R[j]]);
            }

            unsigned argMin = 0;
            double min = thetas[0];
            for (unsigned j = 0; j < R.size(); j++)
            {
                if (thetas[j] < min)
                {
                    min = thetas[j];
                    argMin = j;
                }
            }
            double z = R[argMin];

            if (z != p.last(i))
            {
                return Line(pv[i], pv[z]);
            }
        }

        tri = Triangle(b, intersection, pk);

        Vertex z = a;
        std::vector<unsigned> S;
        s = p.next(i);

        while (s != index)
        {
            Vertex point = pv[s];
            if (point != b && point != intersection && point != pk)
            {
                if (pointInTriangle(point, tri))
                {
                    S.push_back(s);
                }
            }
            s = p.next(s);
        }

        if (S.size() == 0)
        {
            if (pk != c)
            {
                return Line(pv[i], pv[index]);
            }
        }
        else
        {
            std::vector<double> thetas(S.size(), 0.0);
            for (unsigned j = 0; j < S.size(); j++)
            {
                thetas[j] = angle(intersection, b, pv[S[j]]);
            }

            unsigned argMin = 0;
            double min = thetas[0];

            for (unsigned j = 0; j < S.size(); j++)
            {
                if (thetas[j] < min)
                {
                    min = thetas[j];
                    argMin = j;
                }
            }

            unsigned w = S[argMin];

            return Line(pv[i], pv[w]);
        }
        // should not be reached...
        return Line(pv[i], pv[i]);

    }

    bool isGood(const Polygon & p, const Polygon & q)
    {
        unsigned diff = 0;
        std::vector<Line> qe = q.edges();
        std::vector<Line> pe = p.edges();

        for (const Line & e : pe)
        {
            if (std::find(pe.begin(), pe.end(), e) == pe.end())
            {
                diff++;
            }

            if (diff > 1){ return false; }
        }

        if (diff <= 1)
        {
            return true;
        }

        return false;
    }

    Polygon goodSubPolygon(const Polygon & p, const Polygon & q, unsigned i, unsigned j)
    {
        std::vector<Vertex> vertices = q.getVertices();
        std::vector<Vertex> gspVertices;

        unsigned k = i;
        while (k != j)
        {
            gspVertices.push_back(vertices[k]);
            k = q.next(k);
        }
        gspVertices.push_back(vertices[j]);

        Polygon gsp(gspVertices);

        if (isGood(p, gsp))
        {
            return gsp;
        }

        gspVertices.clear();
        k = j;
        while (k != i)
        {
            gspVertices.push_back(vertices[k]);
            k = q.next(k);
        }
        gspVertices.push_back(vertices[i]);

        return Polygon(gspVertices);
    }
}
