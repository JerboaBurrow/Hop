#ifndef SYSTEM_H
#define SYSTEM_H

#include <Object/id.h>

#include <set>

namespace Hop::System
{
    using Hop::Object::Id;

    class System 
    {

    public:

        std::set<Id> objects;
        std::vector<std::set<Id>> threadJobs;
        
    };

}
#endif /* SYSTEM_H */
