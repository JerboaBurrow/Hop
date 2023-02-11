#ifndef FIXEDSOURCE_H
#define FIXEDSOURCE_H

#include <World/mapSource.h>

class FixedSource : public MapSource {
public:

    FixedSource(){}

    uint64_t getAtCoordinate(int i, int j);

private:
};

#endif /* FIXEDSOURCE_H */
