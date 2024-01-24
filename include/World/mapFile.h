#ifndef MAPFILE_H
#define MAPFILE_H

#include <exception>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>

#include <Util/z.h>

#include <Util/sparseData.h>
#include <utility>

namespace Hop::World 
{

    const uint64_t MAP_DATA_NULL = uint64_t();

    using ivec2 = std::pair<int32_t,int32_t>;
    using MapData = SparseData<ivec2,uint64_t>;

    const char * const MAP_FILE_EXTENSION = ".hmap";
    const char * const MAP_FILE_EXTENSION_COMPRESSED = ".hmap.z";

    const char * const MAP_FILE_HEADER = "Hop map file 0.0.1";
    const char * const COMPRESSED_MAP_FILE_HEADER = "Hop compressed map file 0.0.1 using Zlib 1.2.13 next line is the uncompressed file size";

    class MapFileIOError: public std::exception 
    {

    public:

        MapFileIOError(std::string msg)
        : msg(msg)
        {}

    private:

        virtual const char * what() const throw()
        {
            return msg.c_str();
        }
        std::string msg;
    };

    class MapFile 
    {
        
    public:

        MapFile() = default;

        void load(std::string fileNameWithoutExtension, MapData & data);

        void save(std::string fileNameWithoutExtension, MapData & data);

        void loadUncompressed(std::string fileNameWithoutExtension, MapData & data);

        void saveUncompressed(std::string fileNameWithoutExtension, MapData & data);

    private:

        double compressionRatio;

    };

}

#endif /* MAPFILE_H */
