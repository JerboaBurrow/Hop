#ifndef SYSTEM_H
#define SYSTEM_H

#include <Object/id.h>

#include <set>

namespace Hop::System
{
    using Hop::Object::Id;

    /**
     * @brief A System operating on objects.
     *
     * @remark Each system operates on all objects satisfying a
     *         given list of components. See Signature.
     */
    class System
    {
    public:

        /**
         * @brief Enqueue an object to be added to this System.
         *
         * @param id
         */
        void queueRemove(Id id)
        {
            if (objects.find(id) != objects.end()) { toRemove.insert(id); }
        }

        /**
         * @brief Enqueue an object to be removed from this System.
         *
         * @param id
         */
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
