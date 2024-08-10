#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>

namespace Hop
{
    /**
     * @brief Maximum objects in the Hop::Object::EntityComponentSystem.
     *
     */
    const uint32_t MAX_OBJECTS = 100000;
    /**
     * @brief Maximum components in the Hop::Object::EntityComponentSystem.
     *
     */
    const uint32_t MAX_COMPONENTS = 64;
};

#endif /* CONSTANTS_H */
