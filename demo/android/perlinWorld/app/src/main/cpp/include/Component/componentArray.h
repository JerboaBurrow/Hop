#ifndef COMONENTARRAY_H
#define COMONENTARRAY_H

#include <Object/id.h>
#include <exception>
#include <unordered_map>
#include <Component/cPhysics.h>
#include <Component/cCollideable.h>


namespace Hop::Object
{
    class EntityComponentSystem;
}

namespace Hop::Object::Component
{

    enum class REDUCTION_TYPE {SUM_EQUALS};

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
            backBuffered = false;
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

        inline bool hasComponent(const Id & i) const
        {
            return idTaken(i);
        }

        inline T & get(const Id & i)
        {
            // if (!idTaken(i)){
            //     throw NoComponentForId("In ComponentArray.get(i)");
            // }
            return componentData[idToIndex[i]];
        }

        inline T & get(const Id & i, const size_t worker)
        {
            // if (!idTaken(i)){
            //     throw NoComponentForId("In ComponentArray.get(i)");
            // }
            return workerData[worker][idToIndex[i]];
        }

        inline void objectFreed(Id i)
        {
            if (!idTaken(i))
            {
                return;
            }
            remove(i);
        }

        size_t allocatedWorkerData(){ return workerData.size(); }

        inline T * getWorkerData(size_t worker) { return workerData[worker].get(); }
        inline const std::unordered_map<Id,size_t> & getIdToIndex() const { return idToIndex; }

        inline void allocateWorkerData(size_t workers)
        {
            if (workerData.size() != workers)
            {
                workerData.clear();
                for (unsigned i = 0; i < workers; i++)
                {
                    workerData.push_back
                    (
                        std::move
                        (
                            std::make_unique<T[]>(maxObjects)
                        )
                    );

                }
            }
        }

        inline void scatter(size_t worker)
        {
            unsigned start = std::floor(float(nextIndex)/float(workerData.size()))*worker;
            unsigned idx = 0;
            for (unsigned j = 0; j < nextIndex; j++)
            {   
                idx = (j+start) % nextIndex;
                workerData[worker][idx] = componentData[idx];
            }
        }

        inline void reduce(unsigned worker, REDUCTION_TYPE t = REDUCTION_TYPE::SUM_EQUALS);

    protected:

        friend class Hop::Object::EntityComponentSystem;

        bool idTaken(const Id & id) const {return idToIndex.find(id) != idToIndex.end();}
        bool backBuffered;

        std::unique_ptr<T[]> componentData;

        std::vector<std::unique_ptr<T[]>> workerData;

        std::unordered_map<Id,size_t> idToIndex;
        std::unordered_map<size_t,Id> indexToId;

        uint32_t maxObjects;
        size_t nextIndex;

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
