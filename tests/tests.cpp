#include <stdint.h>
#include <iostream>
const double tol = 1e-6;

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

template <class T>
T pointLineSegmentDistanceSquared(
    T px, T py,
    T ax, T ay,
    T bx, T by
){

    T rx = bx-ax; T ry = by-ay;

    T length2 = rx*rx+ry*ry;

    T pMINUSaDOTrOVERlength2 = ((px-ax)*rx + (py-ay)*ry)/length2;

    pMINUSaDOTrOVERlength2 = std::max(static_cast<T>(0.0),std::min(static_cast<T>(1.0),pMINUSaDOTrOVERlength2));

    T tx = ax + pMINUSaDOTrOVERlength2 * rx;
    T ty = ay + pMINUSaDOTrOVERlength2 * ry;

    T dx = px-tx;
    T dy = py-ty;

    return dx*dx+dy*dy;
}

SCENARIO("Point Line Distance", "[geometry]"){
    GIVEN("A point (5,5) and a line (5,-2),(5,2)"){
        float x = 5; float y = 5;
        float ax,ay,bx,by;
        ax = 5; ay = -2; bx = 5; by = 2;
        THEN("The point line segement distance is 3"){
            float d = pointLineSegmentDistanceSquared(
                x,y,
                ax,ay,
                bx,by
            );


            REQUIRE(d == 9);
        }
    }
}