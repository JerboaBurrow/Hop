#ifndef SparseData_H
#define SparseData_H

#include <map>

/*

    Wrapper around a map<INDEX,T> to return a default value 
    if none is stored, otherwise return the stored value. 
    Therebye only storing required data

*/
template <class INDEX, VALUE>
class SparseData {

public:
    
    SparseData(VALUE nullElement, INDEX numberOfElements = -1)
    : NULL_ELEMENT(nullElement)
    {}

    virtual VALUE operator[](INDEX index){

        if (index >= numberOfElements){
            throw std::out_of_range("Index " +std::string(index) + " out of range for SparseData of length "+std::string(numberOfElements));
        }

        if (elements.find(index) != elements.end()){
            return elements[index];
        }

        return NULL_ELEMENT;
        
    }

    bool notNull(INDEX index){
        
        return elements.find(index) != elements.end();
    
    }

    void insert(INDEX index, INDEX value){

        elements[index] = value;

    }

    void clear(INDEX index){

        elements.erase(index);

    }

    void clear(){

        elements.clear();

    }

private:
    VALUE NULL_ELEMENT;
    std::map<INDEX,VALUE> elements;
};

#endif /* SparseData_H */
