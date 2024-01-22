#include <World/mapFile.h>

#include <algorithm>
#include <sstream>

namespace Hop::World 
{

    void MapFile::loadUncompressed(std::string fileNameWithoutExtension, MapData & data)
    {

        std::string fileName = fileNameWithoutExtension+MAP_FILE_EXTENSION;
        std::ifstream in(fileName,std::ios::binary);

        std::vector<uint8_t> rawData;

        if (in.is_open())
        {

            data.clear();

            char c;

            std::string header;
            std::string size;

            if (!std::getline(in,header))
            {
                throw MapFileIOError("EOF when reading header for "+fileNameWithoutExtension);
            }

            while (in.get(c))
            {
                rawData.push_back(c);
            }

            std::string stringData;

            unsigned i = 0;
            while (i < rawData.size())
            {

                char c = rawData[i];
                stringData = "";
                while (c != '\n' && i < rawData.size())
                {
                    c = rawData[i];
                    if (c == '\n'){break;}
                    stringData += c;
                    i += 1;
                }

                size_t p = stringData.find(",");
                int ix = std::stoi(stringData.substr(0,p));
                
                stringData = stringData.substr(p+1);

                p = stringData.find(",");
                int iy = std::stoi(stringData.substr(0,p));

                stringData = stringData.substr(p+1);

                uint64_t value = std::stoull(stringData);

                data.insert(ivec2(ix,iy),value);

                i += 1;

            }
        }
        else
        {
            throw MapFileIOError("file "+fileName+" not openned");
        }

        in.close();
    }

    void MapFile::saveUncompressed(std::string fileNameWithoutExtension, MapData & data)
    {
        std::vector<uint8_t> rawData;
        std::string stringData;

        for (auto citer = data.cbegin(); citer != data.cend(); citer++)
        {

            ivec2 coord = citer->first;

            uint64_t datum = citer->second;

            stringData = std::to_string(coord.first) + "," + std::to_string(coord.second) + "," + std::to_string(datum) + "\n";

            for (unsigned i = 0; i < stringData.size(); i++)
            {
                rawData.push_back(stringData[i]);
            }

        }

        std::string fileName = fileNameWithoutExtension + MAP_FILE_EXTENSION;
        std::ofstream out(fileName,std::ios::binary);
        if (out.is_open())
        {
            
            out << MAP_FILE_HEADER << "\n";

            for (unsigned c = 0; c < rawData.size(); c++){

                out << rawData[c];

            }

        }
        else
        {
            throw MapFileIOError("file "+fileName+" not openned");
        }

        out.close();
    }


    void MapFile::load(std::string fileNameWithoutExtension, MapData & data)
    {

        std::string fileName = fileNameWithoutExtension+MAP_FILE_EXTENSION_COMPRESSED;
        std::string stringData = "";

        std::vector<uint8_t> rawData = Hop::Util::Z::load(fileName);

        unsigned i = 0;
        while (i < rawData.size())
        {

            char c = rawData[i];
            stringData = "";
            while (c != '\n' && i < rawData.size())
            {
                c = rawData[i];
                if (c == '\n'){break;}
                stringData += c;
                i += 1;
            }


            size_t p = stringData.find(",");
            int ix = std::stoi(stringData.substr(0,p));
            
            stringData = stringData.substr(p+1);

            p = stringData.find(",");
            int iy = std::stoi(stringData.substr(0,p));

            stringData = stringData.substr(p+1);

            uint64_t value = std::stoull(stringData);

            data.insert(ivec2(ix,iy),value);

            i += 1;

        }

    }

    void MapFile::save(std::string fileNameWithoutExtension, MapData & data)
    {

        std::vector<uint8_t> rawData;
        std::string stringData;

        for (auto citer = data.cbegin(); citer != data.cend(); citer++)
        {

            ivec2 coord = citer->first;

            uint64_t datum = citer->second;

            stringData = std::to_string(coord.first) + "," + std::to_string(coord.second) + "," + std::to_string(datum) + "\n";

            for (unsigned i = 0; i < stringData.size(); i++)
            {
                rawData.push_back(stringData[i]);
            }

        }

        std::string fileName = fileNameWithoutExtension + MAP_FILE_EXTENSION_COMPRESSED;
        
        Hop::Util::Z::save
        (
            fileName,
            rawData,
            MAP_FILE_HEADER
        );

    }

}