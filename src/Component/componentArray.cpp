#include <Component/componentArray.h>

template <class T>
void ComponentArray<T>::insert(Id i, T component){
    if (idTaken(i)){
        return;
    }

    componentData[nextIndex] = component;
    idToIndex[i] = nextIndex;
    indexToId[nextIndex] = i;

    nextIndex++;
}

template <class T>
void ComponentArray<T>::remove(Id i){
    if (!idTaken(i)){
        return;
    }

    size_t index = idToIndex[i];
    
    if (index != nextIndex-1){
        componentData[index] = componentData[nextIndex-1];
        Id moved = indexToId[nextIndex-1];
        idToIndex[moved] = index;
        indexToId[index] = moved;

        indexToId.erase(nextIndex-1);
    }
    else{
        idToIndex.erase(i);
        indexToId.erase(index);
    }
    idToIndex.erase(i);
    nextIndex--;

}

template <class T>
T & ComponentArray<T>::get(Id i){
    if (!idTaken(i)){
       
    }
    return componentData[idToIndex[i]];
}

template <class T>
void ComponentArray<T>::objectFreed(Id i){
    if (!idTaken(i)){
        return;
    }
    remove(i);
}