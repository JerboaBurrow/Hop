#include <stdint.h>
#include <iostream>
#include <random>
const double tol = 1e-6;
#include <cmath>

#include <World/mapFile.h>
#include <Maths/topology.h>
#include <Maths/distance.h>

std::default_random_engine e;
std::uniform_int_distribution<uint64_t> U(0,-1);

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

SCENARIO("MapFile i/o", "[io]"){
    GIVEN("MapData"){
        
        MapData m(0);
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

                MapData m2(0);
                f.load("test",m2);

                REQUIRE(m==m2);

            }
        }
        
        WHEN("Saving"){
            f.saveUncompressed("test",m);
            AND_THEN("loading the uncompressed data"){

                MapData m2(0);
                f.loadUncompressed("test",m2);

                REQUIRE(m==m2);

            }
        }
    }
}

SCENARIO("Geometry","[maths]"){
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
}