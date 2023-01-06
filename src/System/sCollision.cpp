#include <System/sCollision.h>

void sCollision::update(ObjectManager * m, World * w){
    detector->handleObjectCollisions(
        m,
        resolver.get(),
        objects
    );
}