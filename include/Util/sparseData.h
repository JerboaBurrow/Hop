#ifndef SparseData_H
#define SparseData_H

#include <cstdint>
#include <string>

#include <map>
#include <iterator>

/*

    Wrapper around a map<INDEX,T> to return a default value 
    if none is stored, otherwise return the stored value. 
    Therebye only storing required data

*/
template <class INDEX, class VALUE>
class SparseData 
{

public:
    
    SparseData() {}

    VALUE operator[](INDEX index)
    {
        return elements[index];
    }

    bool notNull(INDEX index)
    {
        
        return elements.find(index) != elements.end();
    
    }

    void insert(INDEX index, VALUE value)
    {

        elements[index] = value;

    }

    void clear(INDEX index)
    {

        elements.erase(index);

    }

    void clear()
    {

        elements.clear();

    }

    auto cbegin(){return elements.cbegin();}
    auto cend(){return elements.cend();}

    const std::map<INDEX,VALUE> & getElements() const { return elements; }

private:

    std::map<INDEX,VALUE> elements;

};

template <class INDEX, class VALUE>
bool operator==(SparseData<INDEX,VALUE> const & lhs, SparseData<INDEX,VALUE> const & rhs)
{
    return lhs.getElements() == rhs.getElements();
}

#endif /* SparseData_H */
