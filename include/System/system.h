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

        void queueRemove(Id id)
        {
            toRemove.insert(id);
        }

        void queueAdd(Id id)
        {
            toAdd.insert(id);
        }

    protected:

        std::set<Id> objects;
        std::set<Id> toRemove;
        std::set<Id> toAdd;

        virtual void updateObjects()
        {
            for (auto & id : toRemove)
            {
                objects.erase(id);
            }

            for (auto & id : toAdd)
            {
                objects.insert(id);
            }

            toRemove.clear();
            toAdd.clear();
        }

    };

}
#endif /* SYSTEM_H */
