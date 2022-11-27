#include <Perlin/perlin.h>

Perlin::Perlin(
    uint64_t seed,
    float turbulence,
    float xPeriod,
    float yPeriod,
    uint64_t repeat
){
    this->seed = seed;
    generator.seed(seed);
    this->xPeriod = xPeriod;
    this->yPeriod = yPeriod;
    this->repeat = repeat;
    this->turbulence = turbulence;

    std::vector<uint64_t> ptable = std::vector<uint64_t>(repeat);
    std::vector<uint64_t> qtable = std::vector<uint64_t>(repeat);

    for (int i = 0; i < repeat; i++){
        qtable[i] = i;
    }

    int i = 0;
    while (qtable.size() > 0){
        std::uniform_int_distribution<uint64_t> U(0,qtable.size()-1);
        uint64_t idx = U(generator);
        ptable[i] = qtable[idx];
        i++;
        qtable.erase(qtable.begin()+idx);
    }

    table = ptable;
}

void Perlin::gradient(uint64_t value, float & cx, float & cy){
    uint8_t h = value % 4;
    switch (h){
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

float Perlin::getValue(float x, float y){

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

float Perlin::getTurbulence(float x, float y, uint64_t size){
    float t = 0.0;
    float scale = size;
    while (scale > 1.0) {
        t += std::abs(scale*getValue(x/scale,y/scale));
        scale /= 2.0;
    }
    return t;
}


void Perlin::getRegion(uint64_t ix, uint64_t iy, float threshold, uint64_t size, bool * buffer){
    int w = size/2;
    int s = size;
    int k = 0;
    for (int i = int(ix)-w; i < int(ix)+w; i++){
        for (int j = int(iy)-w; j < int(iy)+w; j++){
            float u = i*xPeriod / s;
            float v = j*yPeriod / s;
            float t = u+v+turbulence*getTurbulence(i,j,s);
            buffer[k] = std::sin(t) > threshold;
            k += 1;
        }
    }
}

void Perlin::getAtCoordinate(uint64_t ix, uint64_t iy, float threshold, uint64_t size, bool & value){
    float u = ix*xPeriod / size;
    float v = iy*yPeriod / size;
    float t = u+v+turbulence*getTurbulence(ix,iy,size);
    value = std::sin(t) > threshold;
}