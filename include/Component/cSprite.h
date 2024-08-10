#ifndef CSPRITE_H
#define CSPRITE_H

#include <string>
#include <cstdint>

namespace Hop::Object::Component
{
    /**
     * @brief A sprite component.
     * Represents a relative path to a texture asset, and a rectangular
     * region of the texture to draw (in pixel units).
     */
    struct cSprite
    {

        cSprite()
        : texturePath(""), tx(0), ty(0), lx(0), ly(0)
        {}

        cSprite
        (
            std::string path,
            u_int16_t tx,
            u_int16_t ty,
            u_int16_t lx,
            u_int16_t ly
        )
        : texturePath(path), tx(tx), ty(ty), lx(lx), ly(ly)
        {}

        std::string texturePath;
        uint16_t tx;
        uint16_t ty;
        uint16_t lx;
        uint16_t ly;
    };
}
#endif /* CSPRITE_H */
