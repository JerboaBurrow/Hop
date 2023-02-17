#include <World/mapFile.h>

#include <algorithm>
#include <sstream>

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

    std::vector<uint8_t> compressedData;
    std::vector<uint8_t> rawData;

    std::string fileName = fileNameWithoutExtension+MAP_FILE_EXTENSION_COMPRESSED;
    std::ifstream in(fileName,std::ios::binary);

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

        if (!std::getline(in,size))
        {
            throw MapFileIOError("EOF when reading uncompressed size for "+fileNameWithoutExtension);
        }

        // long unsigned int for windows rather than uint64_t
        long unsigned int uncompressedSize = std::stoull(size);

        rawData.reserve(uncompressedSize);
        for (unsigned i = 0; i < uncompressedSize; i++)
        {
            rawData.push_back(0);
        }

        while (in.get(c))
        {
            compressedData.push_back(c);
        }

        int result = uncompress(

            &rawData[0],
            &uncompressedSize,
            &compressedData[0],
            compressedData.size()

        );

        switch (result)
        {
            case Z_OK:
                break;
            case Z_MEM_ERROR:
                throw MapFileIOError("Z_MEM_ERROR while loading "+fileNameWithoutExtension);
                break;
            case Z_BUF_ERROR:
                throw MapFileIOError("Z_BUF_ERROR while loading "+fileNameWithoutExtension);
                break;
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

void MapFile::save(std::string fileNameWithoutExtension, MapData & data)
{

    std::vector<uint8_t> rawData;
    std::vector<uint8_t> compressedData;
    std::string stringData;

    for (auto citer = data.cbegin(); citer != data.cend(); citer++)
    {

        ivec2 coord = citer->first;

        uint64_t datum = citer->second;

        stringData = std::to_string(coord.first) + "," + std::to_string(coord.second) + "," + std::to_string(datum) + "\n";

        for (unsigned i = 0; i < stringData.size(); i++)
        {
            rawData.push_back(stringData[i]);
            compressedData.push_back(0);
        }

    }

    unsigned buffer = compressedData.size()*0.1+12;
    for (unsigned i = 0; i < buffer; i++)
    {
        compressedData.push_back(0);
    }

    uint64_t dataSize = rawData.size();
    // long unsigned int for windows rather than uint64_t
    long unsigned int bufferSize = compressedData.size();

    int result = compress(

        &compressedData[0],
        &bufferSize,
        &rawData[0],
        dataSize
        
    );

    std::string fileName = fileNameWithoutExtension + MAP_FILE_EXTENSION_COMPRESSED;
    std::ofstream out(fileName,std::ios::binary);
    if (out.is_open())
    {
        
        out << COMPRESSED_MAP_FILE_HEADER << "\n";
        out << dataSize << "\n";

        for (unsigned c = 0; c < bufferSize; c++/*ayy lamao*/)
        {

            out << compressedData[c];

        }

    }
    else
    {
        throw MapFileIOError("file "+fileName+" not openned");
    }

    out.close();

    switch (result)
    {
        case Z_OK:
            break;
        case Z_MEM_ERROR:
            throw MapFileIOError("Z_MEM_ERROR while saving "+fileNameWithoutExtension);
            break;
        case Z_BUF_ERROR:
            throw MapFileIOError("Z_BUF_ERROR while saving "+fileNameWithoutExtension);
            break;
    }

    compressionRatio = bufferSize / float(dataSize);


}