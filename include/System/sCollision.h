#ifndef SCOLLISION_H
#define SCOLLISION_H

#include <memory>
#include <System/system.h>
#include <Collision/cellList.h>
#include <Collision/springDashpotResolver.h>

namespace Hop::System::Physics
{
    /*
        System to detect collisions and apply forces
    */

    class sCollision : public System 
    {

    public:

        sCollision(){}

        void update(ObjectManager * m, World * w);

        void setDetector(std::unique_ptr<CollisionDetector> d)
        {
            detector = std::move(d);
        }

        void setResolver(std::unique_ptr<CollisionResolver> r)
        {
            resolver = std::move(r);
        }

        void centreOn(std::pair<float,float> p){centreOn(p.first,p.second);}

        void centreOn(double x, double y);

    private:

        void processThreaded(ObjectManager * m, size_t threadId);

        void updateThreaded(ObjectManager * m);

        std::unique_ptr<CollisionDetector> detector;
        std::unique_ptr<CollisionResolver> resolver;
        
    };

}
#endif /* SCOLLISION_H */
