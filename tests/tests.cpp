#include <stdint.h>
#include <iostream>
#include <random>
const double tol = 1e-6;
#include <cmath>
#include <World/mapFile.h>

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