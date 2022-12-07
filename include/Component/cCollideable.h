#ifndef CCOLLIDEABLE_H
#define CCOLLIDEABLE_H

#include <CollisionMesh>

struct cCollideable {
    bool isGhost;
    CollisionMesh mesh;
}

#endif /* CCOLLIDEABLE_H */
