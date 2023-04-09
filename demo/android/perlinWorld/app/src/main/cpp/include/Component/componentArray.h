#ifndef COMONENTARRAY_H
#define COMONENTARRAY_H

#include <Object/id.h>
#include <exception>
#include <unordered_map>

namespace Hop::Object::Component
{

    class NoComponentForId: public std::exception 
    {

    public:

        NoComponentForId(std::string msg)
        : msg(msg)
        {}

    private:

        virtual const char * what() const throw()
        {
            return msg.c_str();
        }
        
        std::string msg;
    };

    class AbstractComponentArray 
    {

    public:

        virtual ~AbstractComponentArray() = default;
        virtual void objectFreed(Id i) = 0;

    };

    template <class T>
    class ComponentArray : public AbstractComponentArray 
    {

    public:

        ComponentArray(uint32_t m)
        : maxObjects(m), nextIndex(0)
        {
            componentData = std::make_unique<T[]>(maxObjects);
        }

        ComponentArray(const ComponentArray<T> & a)
        : maxObjects(a.maxObjects), nextIndex(a.nextIndex)
        {
            this->componentData = std::make_unique<T[]>(maxObjects);

            for (unsigned i = 0; i < nextIndex; i++)
            {
                this->componentData[i] = a.componentData[i];
            }

            this->idToIndex = a.idToIndex;
            this->indexToId = a.indexToId;
        }
        
        void insert(Id & i, T component);
        void remove(Id & i);

        inline T & get(const Id & i)
        {
            // if (!idTaken(i)){
            //     throw NoComponentForId("In ComponentArray.get(i)");
            // }
            return componentData[idToIndex[i]];
        }

        inline void objectFreed(Id i)
        {
            if (!idTaken(i))
            {
                return;
            }
            remove(i);
        }
        


    private:

        bool idTaken(Id id){return idToIndex.find(id) != idToIndex.end();}

        std::unique_ptr<T[]> componentData;
        std::unordered_map<Id,size_t> idToIndex;
        std::unordered_map<size_t,Id> indexToId;

        size_t nextIndex;
        uint32_t maxObjects;

    };

    template <class T>
    void ComponentArray<T>::insert(Id & i, T component)
    {
        if (idTaken(i))
        {
            return;
        }

        componentData[nextIndex] = component;
        idToIndex[i] = nextIndex;
        indexToId[nextIndex] = i;

        nextIndex++;
    }

    template <class T>
    void ComponentArray<T>::remove(Id & i){
        if (!idTaken(i))
        {
            return;
        }

        size_t index = idToIndex[i];
        
        if (index != nextIndex-1)
        {
            componentData[index] = componentData[nextIndex-1];
            Id & moved = indexToId[nextIndex-1];
            idToIndex[moved] = index;
            indexToId[index] = moved;

            indexToId.erase(nextIndex-1);
        }
        else
        {
            idToIndex.erase(i);
            indexToId.erase(index);
        }
        idToIndex.erase(i);
        nextIndex--;

    }

}
#endif /* COMONENTARRAY_H */
