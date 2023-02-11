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

SCENARIO("MapFile compression", "[io]"){
    GIVEN("MapData"){
        
        MapData m(0);
        MapFile f;

        int n = 32;
        for (int i = -n; i <= n; i++){
            for (int j = -n; j <= n; j++){
                m.insert(ivec2(i,j),U(e));
            }
        }

        WHEN("Saving"){
            f.save("test",m);

            AND_THEN("loading"){

                MapData m2(0);
                f.load("test",m2);

                REQUIRE(m==m2);

            }
        }
    }
}