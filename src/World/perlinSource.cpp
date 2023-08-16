#include <World/perlinSource.h>

namespace Hop::World 
{

    std::vector<uint64_t> generateTable(
        uint64_t length, 
        std::default_random_engine gen
    )
    {
        std::vector<uint64_t> ret = std::vector<uint64_t>(length);
        std::vector<uint64_t> table = std::vector<uint64_t>(length);

        for (unsigned i = 0; i < length; i++)
        {
            table[i] = i;
        }

        int i = 0;
        while (table.size() > 0)
        {
            std::uniform_int_distribution<uint64_t> U(0,table.size()-1);
            uint64_t idx = U(gen);
            ret[i] = table[idx];
            i++;
            table.erase(table.begin()+idx);
        }

        return ret;
    }

    PerlinSource::PerlinSource(
        uint64_t seed,
        float turbulence,
        float xPeriod,
        float yPeriod,
        uint64_t repeat,
        float detailThreshold
    )
    {
        this->seed = seed;
        generator.seed(seed);
        this->xPeriod = xPeriod;
        this->yPeriod = yPeriod;
        this->repeat = repeat;
        this->turbulence = turbulence;
        this->detailThreshold = detailThreshold;

        tables = {
            generateTable(repeat,generator),
            generateTable(repeat,generator)
        };

    }

    void PerlinSource::gradient(uint64_t value, float & cx, float & cy)
    {
        uint8_t h = value % 4;
        switch (h)
        {
            case 0:
                cx = 1.; cy=1.; break;
            case 1:
                cx = -1.; cy=1.; break;
            case 2:
                cx = -1.; cy = -1.; break;
            case 3:
                cx = 1.; cy = -1.; break;
        }
    }

    float PerlinSource::getValue(float x, float y, uint8_t t)
    {

        float xf = std::floor(x);
        float yf = std::floor(y);

        int X = int(xf)%repeat;
        int Y = int(yf)%repeat;
        X < 0 ? X += repeat : 0;
        Y < 0 ? Y += repeat : 0;

        xf = x-xf;
        yf = y-yf;

        float trX = xf-1.0;
        float trY = yf-1.0;

        float tlX = xf;
        float tlY = yf-1.0;

        float brX = xf-1.0;
        float brY = yf;

        float blX = xf;
        float blY = yf;

        std::vector<uint64_t> table = tables[t];

        uint64_t vtr = table[(table[(X+1)%repeat]+Y+1)%repeat];
        uint64_t vtl = table[(table[X%repeat]+(Y+1))%repeat];
        uint64_t vbr = table[(table[(X+1)%repeat]+Y)%repeat];
        uint64_t vbl = table[(table[X%repeat]+Y)%repeat];

        float gx, gy;

        gradient(vtr,gx,gy);
        float dtr = trX*gx+trY*gy;
        gradient(vtl,gx,gy);
        float dtl = tlX*gx+tlY*gy;
        gradient(vbr,gx,gy);
        float dbr = brX*gx+brY*gy;
        gradient(vbl,gx,gy);
        float dbl = blX*gx+blY*gy;

        float u = smooth(xf);
        float v = smooth(yf);

        return lerp(
            u,
            lerp(v,dbl,dtl),
            lerp(v,dbr,dtr)
        )*0.5+0.5;
    }

    float PerlinSource::getTurbulence(float x, float y, uint64_t size, uint8_t table)
    {
        float t = 0.0;
        float scale = size;
        while (scale > 1.0)
        {
            t += std::abs(scale*getValue(x/scale,y/scale,table));
            scale /= 2.0;
        }
        return t;
    }

    uint64_t  PerlinSource::getAtCoordinate(int ix, int iy)
    {
        uint64_t value = data[ivec2(ix,iy)];
        if (value != MAP_DATA_NULL)
        {
            return value;
        }

        value = false;

        float u = ix*xPeriod / size;
        float v = iy*yPeriod / size;

        float t = u+v+turbulence*getTurbulence(ix,iy,size,0);

        bool a = std::sin(t) > threshold;
        if (!a){value = false; return value;}

        float s = u+v+4.0*turbulence*getTurbulence(ix,iy,size,1);
        bool b = std::sin(s) > detailThreshold;
        
        value = a & (!b);
        return value;
    }

}