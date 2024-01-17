#include <stdint.h>
#include <iostream>
#include <random>
const double tol = 1e-6;
#include <cmath>

#include <World/mapFile.h>
#include <Maths/topology.h>
#include <Maths/distance.h>
#include <Maths/special.h>
#include <Maths/polygon.h>
#include <Maths/triangulation.h>
#include <Collision/collisionMesh.h>


namespace Hop::System::Physics
{
    std::ostream & operator<<(std::ostream & o, Hop::System::Physics::Rectangle const & r)
    {
        o << r.ulx << ", " << r.uly << "    " << r.urx << ", " << r.ury << "\n"
            << r.llx << ", " << r.lly << "    " << r.lrx << ", " << r.lry << "\n"
            << r.x   << ", " << r.y   << ", "   << r.r << "\n"
            << r.axis1x << ", " << r.axis1y << ", " << r.axis2x << ", " << r.axis2y << "\n";

        return o;
    }
}

using namespace Hop::Maths;
using namespace Hop::World;
using namespace Hop::System::Physics;

std::default_random_engine e;
std::uniform_int_distribution<uint64_t> U(0,-1);

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

SCENARIO("MapFile i/o", "[io]"){
    GIVEN("MapData"){
        
        MapData m;
        MapFile f;

        int n = 4;
        for (int i = -n; i <= n; i++){
            for (int j = -n; j <= n; j++){
                m.insert(ivec2(i,j),U(e));
            }
        }

        WHEN("Saving compressed"){
            f.save("test",m);
            AND_THEN("loading the compressed data"){

                MapData m2;
                f.load("test",m2);

                REQUIRE(m==m2);

            }
        }
        
        WHEN("Saving"){
            f.saveUncompressed("test",m);
            AND_THEN("loading the uncompressed data"){

                MapData m2;
                f.loadUncompressed("test",m2);

                REQUIRE(m==m2);

            }
        }
    }
}

SCENARIO("Distance","[maths]"){

    GIVEN("A point [0.,1.]"){
        double px = 0.0; double py = 1.0;
        AND_GIVEN("The line [0.5,0] -> [0.5,1.]"){
            double ax = 0.5; double ay = 0.0;
            double bx = 0.5; double by = 1.0;
            THEN("The point line distance is 0.5"){
                double d = pointLineSegmentDistanceSquared(px,py,ax,ay,bx,by);
                REQUIRE(d == 0.5*0.5);
            }
        }
    }

    GIVEN("A point [0.5,0.]"){
        double px = 0.5; double py = 0.0;
        AND_GIVEN("The line [0.0,0.5] -> [0.75,0.5]"){
            double ax = 0.0; double ay = 0.5;
            double bx = 0.75; double by = 0.5;
            THEN("The point line distance is 0.5"){
                double d = pointLineSegmentDistanceSquared(px,py,ax,ay,bx,by);
                REQUIRE(d == 0.5*0.5);
            }
        }
    }

    GIVEN("A point [0.5,1.]"){
        double px = 0.5; double py = 1.0;
        AND_GIVEN("The line [0.0,0] -> [1,1.]"){
            double ax = 0.0; double ay = 0.0;
            double bx = 1.0; double by = 1.0;
            THEN("The point line distance is 0.5"){
                double d = pointLineSegmentDistanceSquared(px,py,ax,ay,bx,by);
                REQUIRE(std::abs(d-0.35355339059327373*0.35355339059327373)<tol);
            }
        }
    }

    GIVEN("A line [0.0,0.0] -> [0.,1.0]")
    {
        double ax = 0.0; double ay = 0.0;
        double bx = 0.0; double by = 1.0; 
        AND_GIVEN("A point [0.5,0.5]")
        {
            double px = 0.5; double py = 0.5;
            THEN("The point line handedness is -1")
            {
                int h = pointLineHandedness<double>(
                    px,py,
                    ax,ay,
                    bx,by
                );
                REQUIRE(h == -1);
            }
        }
        AND_GIVEN("A point [-0.5,0.5]")
        {
            double px = -0.5; double py = 0.5;
            THEN("The point line handedness is 1")
            {
                int h = pointLineHandedness<double>(
                    px,py,
                    ax,ay,
                    bx,by
                );
                REQUIRE(h == 1);
            }
        }
    }

    GIVEN("A line [-0.5,0.5] -> [0.5,0.5]")
    {
        double ax = -0.5; double ay = 0.5;
        double bx = 0.5; double by = 0.5; 
        AND_GIVEN("A point [0.0,1.0]")
        {
            double px = 0.0; double py = 1.0;
            THEN("The point line handedness is 1")
            {
                int h = pointLineHandedness<double>(
                    px,py,
                    ax,ay,
                    bx,by
                );
                REQUIRE(h == 1);
            }
        }
        AND_GIVEN("A point [0.0,-1.0]")
        {
            double px = 0.0; double py = -1.0;
            THEN("The point line handedness is -1")
            {
                int h = pointLineHandedness<double>(
                    px,py,
                    ax,ay,
                    bx,by
                );
                REQUIRE(h == -1);
            }
        }
    }

    GIVEN("A rectangle primitive [-0.5,-1.0],[-0.5,1.0],[0.5,1.0],[0.5,-1.0]")
    {
        Hop::System::Physics::Rectangle r1
        (
            -0.5,-1.0,
            -0.5, 1.0,
             0.5, 1.0,
             0.5,-1.0
        );
        AND_GIVEN("A rectangle [-0.5,1.5],[1.5,1.5],[1.5,1.0],[-0.5,1.0]")
        {
            THEN("There is no intersection")
            {
                Hop::System::Physics::Rectangle r2
                (
                    -0.5,1.5,
                    1.5, 1.5,
                    1.5, 1.0,
                    -0.5,1.0
                );
                double nx, ny, s;
                bool c = rectangleRectangleCollided<double>(&r1,&r2,nx,ny,s);
                REQUIRE(!c);
            } 
        }
        AND_GIVEN("A rectangle [-0.5,1.5],[1.5,1.5],[1.5,0.9],[-0.5,0.9]")
        {
            THEN("There is an intersection")
            {
                Hop::System::Physics::Rectangle r2
                (
                    -0.5,1.5,
                    1.5, 1.5,
                    1.5, 0.9,
                    -0.5,0.9
                );
                double nx, ny, s;
                bool c = rectangleRectangleCollided<double>(&r1,&r2,nx,ny,s);
                REQUIRE(c);
                REQUIRE(std::abs(nx - 0.0) < tol);
                REQUIRE(std::abs(ny - 1.0) < tol);
                REQUIRE(std::abs(s - 0.1) < tol);
            }
        }
        AND_GIVEN("A rectangle [-0.5,-1.5],[1.5,-1.5],[1.5,-0.9],[-0.5,-0.9]")
        {
            THEN("There is an intersection")
            {
                Hop::System::Physics::Rectangle r2
                (
                    -0.5,1.5,
                    1.5, 1.5,
                    1.5, 0.9,
                    -0.5,0.9
                );
                double nx, ny, s;
                bool c = rectangleRectangleCollided<double>(&r1,&r2,nx,ny,s);
                REQUIRE(c);
                REQUIRE(std::abs(nx - 0.0) < tol);
                REQUIRE(std::abs(ny - 1.0) < tol);
                REQUIRE(std::abs(s - 0.1) < tol);
            }
        }
    }
    GIVEN("A rectangle primitive [-0.5,-1.0],[-0.5,1.0],[0.5,1.0],[0.5,-1.0]")
    {
        Hop::System::Physics::Rectangle r1
        (
            -0.5,-1.0,
            -0.5, 1.0,
             0.5, 1.0,
             0.5,-1.0
        );
        AND_GIVEN("A point [-0.5,-1.0]")
        {
            double px = -0.5;
            double py = -1.0;
            THEN("The SDF is 0")
            {
                double s = sdf(&r1,px,py);
                REQUIRE(std::abs(s) < tol);
            }
        }
        AND_GIVEN("A point [0.5,1.0]")
        {
            double px = 0.5;
            double py = 1.0;
            THEN("The SDF is 0")
            {
                double s = sdf(&r1,px,py);
                REQUIRE(std::abs(s) < tol);
            }
        }
        AND_GIVEN("A point [0.5,0.5]")
        {
            double px = 0.5;
            double py = 0.5;
            THEN("The SDF is 0")
            {
                double s = sdf(&r1,px,py);
                REQUIRE(std::abs(s) < tol);
            }
        }
        AND_GIVEN("A point [-0.5,-1.1]")
        {
            double px = -0.5;
            double py = -1.1;
            THEN("The SDF is 0.1")
            {
                double s = sdf(&r1,px,py);
                REQUIRE(std::abs(s-0.1)<tol);
            }
        }
        AND_GIVEN("A point [-0.45,-0.9]")
        {
            double px = -0.45;
            double py = -0.9;
            THEN("The SDF is -0.05")
            {
                double s = sdf(&r1,px,py);
                REQUIRE(std::abs(s- -0.05)<tol);
            }
        }
        AND_GIVEN("A point [0,0]")
        {
            double px = 0;
            double py = 0;
            THEN("The SDF is -0.5")
            {
                double s = sdf(&r1,px,py);
                REQUIRE(std::abs(s- -0.5)<tol);
            }
        }
    }
    GIVEN("A rectangle primitive [-1,0],[0,1],[1,0],[0,-1]")
    {
        Hop::System::Physics::Rectangle r1
        (
            -1.,0.,
            0.,1.,
            1.,0.,
            0.,-1.
        );
        AND_GIVEN("A point [-1.0,-0.0]")
        {
            double px = -1.0;
            double py = 0.0;
            THEN("The SDF is 0")
            {
                double s = sdf(&r1,px,py);
                REQUIRE(std::abs(s) < tol);
            }
        }
        AND_GIVEN("A point [0.0,0.0]")
        {
            double px = 0.0;
            double py = 0.0;
            THEN("The SDF is -1")
            {
                double s = sdf(&r1,px,py);
                REQUIRE(std::abs(s - -0.707107) < tol);
            }
        }
        AND_GIVEN("A point [0.51,0.51]")
        {
            double px = 0.51;
            double py = 0.51;
            THEN("The SDF is -1")
            {
                double s = sdf(&r1,px,py);
                REQUIRE(std::abs(s - 0.0141421356237309) < tol);
            }
        }
    }
}

SCENARIO("Triangle","[maths]")
{
    GIVEN("The triangle (-3,2),(-3,3),(-2,3)")
    {
        double ax = -3.0;
        double ay = 2.0;
        double bx = -3.0;
        double by = 3.0;
        double cx = -2.0;
        double cy = 3.0;
        THEN("its area is 0.5")
        {
            double a = triangleArea<double>
            (
                ax, ay,
                bx, by,
                cx, cy
            );
            REQUIRE(std::abs(a-0.5)<tol);
        }
    }
    GIVEN("The triangle (-4.3445039703337,1.5302054072105),(-3,3),(-2,3)")
    {
        double ax = -4.3445039703337;
        double ay = 1.5302054072105;
        double bx = -3.0;
        double by = 3.0;
        double cx = -2.0;
        double cy = 3.0;
        THEN("its area is 0.7348972963948")
        {
            double a = triangleArea<double>
            (
                ax, ay,
                bx, by,
                cx, cy
            );
            REQUIRE(std::abs(a-0.7348972963948)<tol);
        }
    }
}

SCENARIO("Special functions", "[maths]")
{
    GIVEN("The int x = 1")
    {
        int x = 1;
        THEN("sgn(x) == 1")
        {
            REQUIRE(sgn<int>(x)==1);
        }
    }
    GIVEN("The int x = -1")
    {
        int x = -1;
        THEN("sgn(x) == -1")
        {
            REQUIRE(sgn<int>(x)==-1);
        }
    }
    GIVEN("The int x = 0")
    {
        int x = 0;
        THEN("sgn(x) == 0")
        {
            REQUIRE(sgn<int>(x)==0);
        }
    }

    GIVEN("The float x = 1.0f")
    {
        float x = 1.0f;
        THEN("sgn(x) == 1.0f")
        {
            REQUIRE(sgn<float>(x)==1.0f);
        }
    }
    GIVEN("The float x = -1.0f")
    {
        float x = -1.0f;
        THEN("sgn(x) == -1.0f")
        {
            REQUIRE(sgn<float>(x)==-1.0f);
        }
    }
    GIVEN("The float x = 0.0f")
    {
        float x = 0.0f;
        THEN("sgn(x) == 0.0f")
        {
            REQUIRE(sgn<float>(x)==0.0f);
        }
    }
}

SCENARIO("Topology","[maths]")
{
    GIVEN("The rectangle 0.890625, 0.34375, 0.890625, 0.351562, 0.90625, 0.351562, 0.90625, 0.34375")
    {
        Hop::System::Physics::Rectangle r
        (
            0.890625, 0.34375, 0.890625, 0.351562,
            0.90625, 0.351562, 0.90625, 0.34375
        );

        AND_GIVEN("The point (0.898818, 0.350784)")
        {
            THEN("The point is inside the rectangle")
            {
                REQUIRE(pointInRectangle(0.898818, 0.350784, &r));
            }
        }
    }
    GIVEN("The rectangle (-1.,-1.),(-1,1),(1,1),(1,-1)")
    {
        Hop::System::Physics::Rectangle r
        (
            -1.,-1.,
            -1.,1.,
            1.,1.,
            1.,-1.
        );

        AND_GIVEN("The point (0,0)")
        {
            THEN("The point is inside the rectangle")
            {
                REQUIRE(pointInRectangle(0.0,0.0,&r));
            }
        }
        AND_GIVEN("The point (1,0)")
        {
            THEN("The point is inside the rectangle")
            {
                REQUIRE(pointInRectangle(1.0,0.0,&r));
            }
        }
        AND_GIVEN("The point (1,1)")
        {
            THEN("The point is inside the rectangle")
            {
                REQUIRE(pointInRectangle(1.,1.,&r));
            }
        }
        AND_GIVEN("The point (1.1,1)")
        {
            THEN("The point is outside the rectangle")
            {
                REQUIRE(!pointInRectangle(1.1,1.,&r));
            }
        }
    }
}

SCENARIO("Polygon","[maths]")
{
    GIVEN("A polygon with vertices [0,0], [1, 0], and [0, 1]")
    {
        Hop::Maths::Polygon p(std::vector<Vertex> {Vertex(0.0 ,0.0), Vertex(1.0, 0.0), Vertex(0.0, 1.0)});
        
        THEN("The Polygon is left handed")
        {
            REQUIRE(p.getHandedness() == HAND::LEFT);
            AND_THEN("After reversing the polygon is right handed")
            {
                p.reverse();
                REQUIRE(p.getHandedness() == HAND::RIGHT);
            }
        }
    }
    GIVEN("A polygon with vertices [0,0], [0, 1], and [1, 0]")
    {
        Hop::Maths::Polygon p(std::vector<Vertex> {Vertex(0.0 ,0.0), Vertex(0.0, 1.0), Vertex(1.0, 0.0)});
        
        THEN("The Polygon is right handed")
        {
            REQUIRE(p.getHandedness() == HAND::RIGHT);
            AND_THEN("After reversing the polygon is left handed")
            {
                p.reverse();
                REQUIRE(p.getHandedness() == HAND::LEFT);
            }
        }
    }
}

SCENARIO("Triangulation", "[maths]")
{
    GIVEN("A (right) polygon with vertices [0,0], [0, 1], and [1, 0]")
    {
        Hop::Maths::Polygon p(std::vector<Vertex> {Vertex(0.0 ,0.0), Vertex(0.0, 1.0), Vertex(1.0, 0.0)});

        WHEN("A triangulation is created from it")
        {
            Triangulation t(p);
            THEN("Its triangulation has 1 triangle")
            {
                REQUIRE(t.size() == size_t(1));
            }
            AND_THEN("The polygon is its own triangulation")
            {
                auto tris = t.getTriangles();
                auto pv = p.getVertices();

                REQUIRE(tris[0].a == pv[0]);
                REQUIRE(tris[0].b == pv[1]);
                REQUIRE(tris[0].c == pv[2]);
            }
        }
    }

    GIVEN("A polygon with vertices [0,0], [0, 1], and [1, 0], [0.5, 0.5]")
    {
        Hop::Maths::Polygon p
        (
            std::vector<Vertex> 
            {
                Vertex(0.0 ,0.0), 
                Vertex(0.0, 1.0),
                Vertex(1.0, 0.0),
                Vertex(0.5, 0.5)
            }
        );

        WHEN("A triangulation is created from it")
        {
            Triangulation t(p);
            THEN("Its triangulation has 2 triangles")
            {
                REQUIRE(t.size() == size_t(2));
            }
            AND_THEN("The polygon is [p.3, p.0, p.1], [p.1, p.2, p.3]")
            {
                auto tris = t.getTriangles();
                auto pv = p.getVertices();

                REQUIRE(tris[0].a == pv[3]);
                REQUIRE(tris[0].b == pv[0]);
                REQUIRE(tris[0].c == pv[1]);

                REQUIRE(tris[1].a == pv[1]);
                REQUIRE(tris[1].b == pv[2]);
                REQUIRE(tris[1].c == pv[3]);
            }
        }
    }

    GIVEN("A polygon with vertices [1.1916655596791519, -0.49360403663203056], [0.9575527878348344, 0.9575527878348342], and [1.1568622657126908, 0.47918804025586376], [1.4828907275871481, 0.0]")
    {
        Hop::Maths::Polygon p
        (
            std::vector<Vertex> 
            {
                Vertex(1.1916655596791519, -0.49360403663203056),
                Vertex(0.9575527878348344, 0.9575527878348342),
                Vertex(1.1568622657126908, 0.47918804025586376),
                Vertex(1.4828907275871481, 0.0)
            }
        );

        WHEN("A triangulation is created from it")
        {
            Triangulation t(p);
            THEN("Its triangulation has 2 triangles")
            {
                REQUIRE(t.size() == size_t(2));
            }
            AND_THEN("The tirangles are [p0, p1, p2], [p0, p2, p3]")
            {
                auto tris = t.getTriangles();
                auto pv = p.getVertices();
                REQUIRE(tris[0].a == pv[0]);
                REQUIRE(tris[0].b == pv[1]);
                REQUIRE(tris[0].c == pv[2]);
                REQUIRE(tris[1].a == pv[0]);
                REQUIRE(tris[1].b == pv[2]);
                REQUIRE(tris[1].c == pv[3]);
            }
        }

    }
}