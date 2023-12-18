#ifndef CSOUND_H
#define CSOUND_H

#include <string>

namespace Hop::Object::Component
{
    struct cSound
    {

        cSound()
        : filename("")
        {}

        cSound(const char * f)
        : filename(f)
        {}

        cSound(std::string f)
        : filename(f)
        {}

        std::string filename;

    };
}

#endif /* CSOUND_H */
