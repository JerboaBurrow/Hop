#include <System/sCollision.h>

void sCollision::update(ObjectManager * m, World * w){
    detector->handleObjectCollisions(
        m,
        resolver.get(),
        objects
    );
    //auto it = objects.begin();

    // for (auto it = objects.begin(); it != objects.end(); it++){
    //     resolver->handleObjectWorldCollisions(
    //         *it,
    //         m,
    //         w
    //     );
    // }
}