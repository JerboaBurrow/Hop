#ifndef PERLINSOURCE_H
#define PERLINSOURCE_H

#include <vector>
#include <random>

#include <World/mapSource.h>

namespace Hop::World 
{

    class PerlinSource : public MapSource 
    {
        
    public:
        
        PerlinSource(
            uint64_t seed,
            float turbulence,
            float xPeriod,
            float yPeriod,
            uint64_t repeat = 256,
            float detailThreshold = 0.5
        );

        ~PerlinSource(){}

        void setThreshold(float t){threshold=t;}
        void setSize(uint64_t s){size = s;}
        
        uint64_t getAtCoordinate(int ix, int iy);

    private:

        uint64_t seed;
        float turbulence;
        float xPeriod;
        float yPeriod;
        uint64_t repeat;
        float detailThreshold;
        float threshold;
        uint64_t size;

        std::vector<std::vector<uint64_t>> tables;

        void gradient(uint64_t value, float & cx, float & cy);
        float smooth(float x) {return ((6.0*x-15.0)*x+10.0)*x*x*x;}
        float lerp(float x,float a1, float a2) {return a1+x*(a2-a1);}
        float getValue(float x, float y, uint8_t t);
        float getTurbulence(float x, float y, uint64_t size, uint8_t table);

        std::default_random_engine generator;
    };

}
#endif /* PERLINSOURCE_H */
