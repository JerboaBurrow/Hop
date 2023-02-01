#ifndef FIELDSOURCE_H
#define FIELDSOURCE_H

#include <Util/sparseData.h>
#include <utility>

using ivec2 = std::pair<int,int>;

class FieldSource {
public:

    FieldSource(){}

    virtual ~FieldSource(){}

    virtual uint64_t getAtCoordinate(int i, int j) = 0;
    virtual void save(std::string filename) = 0;
    virtual void load(std::string filename) = 0;

private:

    SparseData<ivec2,uint64_t> field(0);

};

#endif /* FIELDSOURCE_H */
