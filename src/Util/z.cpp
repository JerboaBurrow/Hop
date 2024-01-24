#include <Util/z.h>

namespace Hop::Util::Z
{
    std::vector<uint8_t> load(std::string file)
    {

        std::ifstream in(file,std::ios::binary);
        std::string header;
        std::string size;

        if (!std::getline(in,header))
        {
            throw CompressionIOError("EOF when reading header for "+file);
        }

        if (!std::getline(in,size))
        {
            throw CompressionIOError("EOF when reading uncompressed size for "+file);
        }

        char c;
        std::vector<uint8_t> compressedData;
        std::vector<uint8_t> rawData;

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
                throw CompressionIOError("Z_MEM_ERROR while loading "+file);
                break;
            case Z_BUF_ERROR:
                throw CompressionIOError("Z_BUF_ERROR while loading "+file);
                break;
        }

        return rawData;

    }

    void save
    (
        std::string file, 
        std::vector<uint8_t> data,
        std::string header
    )
    {
        std::vector<uint8_t> compressedData(data.size()*1.1+12);

        uint64_t dataSize = data.size();
        // long unsigned int for windows rather than uint64_t
        long unsigned int bufferSize = compressedData.size();

        int result = compress(

            &compressedData[0],
            &bufferSize,
            &data[0],
            dataSize
            
        );

        switch (result)
        {
            case Z_OK:
                break;
            case Z_MEM_ERROR:
                throw CompressionIOError("Z_MEM_ERROR while saving "+file);
                break;
            case Z_BUF_ERROR:
                throw CompressionIOError("Z_BUF_ERROR while saving "+file);
                break;
        }

        std::ofstream out(file,std::ios::binary);
        if (out.is_open())
        {
            out << header << "\n";
            out << dataSize << "\n";

            for (unsigned c = 0; c < bufferSize; c++/*ayy lmao*/)
            {
                out << compressedData[c];
            }
        }
        else
        {
            throw CompressionIOError("file "+file+" not openned");
        }
    }
}