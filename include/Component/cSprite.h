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
        : texturePath(""), textureRegion()
        {}

        cSprite
        (
            std::string path,
            uint16_t tx,
            uint16_t ty,
            uint16_t lx,
            uint16_t ly
        )
        : texturePath(path), textureRegion(tx, ty, lx, ly)
        {}

        std::string texturePath;
        jGL::TextureRegion textureRegion;
    };
}
#endif /* CSPRITE_H */
