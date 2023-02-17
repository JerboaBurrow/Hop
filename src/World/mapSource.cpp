#include <World/mapSource.h>

void MapSource::save(std::string fileNameWithoutExtension, bool compressed)
{
    MapFile file;

    if (compressed)
    {
        file.save(fileNameWithoutExtension, data);
    }
    else
    {
        file.saveUncompressed(fileNameWithoutExtension, data);
    }
}

void MapSource::load(std::string fileNameWithoutExtension, bool compressed)
{
    MapFile file;

    if (compressed)
    {
        file.load(fileNameWithoutExtension, data);
    }
    else
    {
        file.loadUncompressed(fileNameWithoutExtension, data);
    }

}