#include <Collision/collisionDetector.h>

std::ostream & operator<<(std::ostream & o, Hop::System::Physics::CollisionDetector::CollisionType t)
{
    switch (t)
    {
        case Hop::System::Physics::CollisionDetector::CollisionType::NONE:
            o << "NONE";
            break;
        case Hop::System::Physics::CollisionDetector::CollisionType::OBJECT:
            o << "OBJECT";
            break;
        case Hop::System::Physics::CollisionDetector::CollisionType::WORLD:
            o << "WORLD";
            break;
        case Hop::System::Physics::CollisionDetector::CollisionType::OBJECT_WORLD:
            o << "OBJECT_WORLD";
            break;
        default:
            o << "NONE";
            break;
    }
    return o;
}
