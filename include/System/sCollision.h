#ifndef SCOLLISION_H
#define SCOLLISION_H

#include <memory>
#include <System/system.h>
#include <Collision/cellList.h>
#include <Collision/springDashpotResolver.h>

/*
    System to detect collisions and apply forces
*/

class sCollision : public System {
public:

    sCollision(){}

    void update(ObjectManager * m, World * w);

    void setDetector(std::unique_ptr<CollisionDetector> d){
        detector = std::move(d);
    }

    void setResolver(std::unique_ptr<CollisionResolver> r){
        resolver = std::move(r);
    }

private:

    void processThreaded(ObjectManager * m, size_t threadId);

    void updateThreaded(ObjectManager * m);

    std::unique_ptr<CollisionDetector> detector;
    std::unique_ptr<CollisionResolver> resolver;
};

#endif /* SCOLLISION_H */
