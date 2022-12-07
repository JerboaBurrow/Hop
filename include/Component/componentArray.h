#ifndef COMONENTARRAY_H
#define COMONENTARRAY_H

#include <Object/id.h>

class AbstractComponentArray {
public:
    virtual ~AbstractComponentArray() = default;
    virtual void objectFreed(Id ) = 0;
};

template <class T>
class ComponentArray : AbstractComponentArray {

public:

    ComponentArray(uint32_t m)
    : maxEntities(m), nextIndex(0)
    {
        componentData = std::make_unique<T[]>(maxEntities);
    }
    
    void insert(Id i, T component);
    void remove(Id i);
    T & get(Id i);
    void objectFreed(Id i);

private:

    bool idTaken(Id id){return idToIndex.find(id) != idToIndex.end();}

    std::unique_ptr<T[]> componentData;
    std::unordered_map<Id,size_t> idToIndex;
    std::unordered_map<size_t,Id> indexToId;

    size_t nextIndex;
    uint32_t maxEntities;

};

#endif /* COMONENTARRAY_H */
