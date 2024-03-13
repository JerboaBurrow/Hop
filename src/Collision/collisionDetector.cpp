#include <Collision/collisionDetector.h>

std::ostream & operator<<(std::ostream & o, Hop::System::Physics::CollisionDetector::CollisionType t)
{
    o << t.with << ", world" << t.world;
    return o;
}
