#ifndef MAPFILE_H
#define MAPFILE_H

#include <exception>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>

#include <zlib.h>

#include <Util/sparseData.h>
#include <utility>

using ivec2 = std::pair<int32_t,int32_t>;
using MapData = SparseData<ivec2,uint64_t>;

const char * const MAP_FILE_EXTENSION = ".hmap";
const char * const MAP_FILE_EXTENSION_COMPRESSED = ".hmap.z";

class MapFileIOError: public std::exception {
public:
    MapFileIOError(std::string msg)
    : msg(msg)
    {}
private:
    virtual const char * what() const throw(){
        return msg.c_str();
    }
    std::string msg;
};

class MapFile {
public:

    MapFile() = default;

    void load(std::string fileNameWithoutExtension, MapData & data);

    void save(std::string fileNameWithoutExtension, MapData & data);


private:

    double compressionRatio;

};

#endif /* MAPFILE_H */