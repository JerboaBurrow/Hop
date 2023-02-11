#include <World/fixedSource.h>

uint64_t FixedSource::getAtCoordinate(int i, int j){
    return data[ivec2(i,j)];
}