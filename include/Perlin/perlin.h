#ifndef PERLIN_H
#define PERLIN_H

#include <vector>
#include <random>

class Perlin {
public:
    
    Perlin(
        uint64_t seed,
        float turbulence,
        float xPeriod,
        float yPeriod,
        uint64_t repeat = 256,
        float detailThreshold = 0.5
    );
    
    void getAtCoordinate(int ix, int iy, float threshold, uint64_t size, bool & value);

private:

    uint64_t seed;
    float turbulence;
    float xPeriod;
    float yPeriod;
    uint64_t repeat;
    float detailThreshold;

    std::vector<std::vector<uint64_t>> tables;

    void gradient(uint64_t value, float & cx, float & cy);
    float smooth(float x) {return ((6.0*x-15.0)*x+10.0)*x*x*x;}
    float lerp(float x,float a1, float a2) {return a1+x*(a2-a1);}
    float getValue(float x, float y, uint8_t t);
    float getTurbulence(float x, float y, uint64_t size, uint8_t table);

    std::default_random_engine generator;
};

#endif /* PERLIN_H */
