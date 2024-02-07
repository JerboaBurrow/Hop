#include <Util/z.h>

namespace Hop::Util::Z
{
    std::vector<uint8_t> inflate(std::vector<uint8_t> & cdata, long unsigned int decompressedSize)
    {
        std::vector<uint8_t> rawData;

        rawData.reserve(decompressedSize);
        for (unsigned i = 0; i < decompressedSize; i++)
        {
            rawData.push_back(0);
        }

        int result = uncompress(

            &rawData[0],
            &decompressedSize,
            &cdata[0],
            cdata.size()
        );

        switch (result)
        {
            case Z_OK:
                break;
            case Z_MEM_ERROR:
                throw CompressionIOError("Z_MEM_ERROR decompressing");
                break;
            case Z_BUF_ERROR:
                throw CompressionIOError("Z_BUF_ERROR while decompressing");
                break;
        }

        return rawData;
    }

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

        while (in.get(c))
        {
            compressedData.push_back(c);
        }

        return inflate(compressedData, uncompressedSize);

    }

    std::vector<uint8_t> deflate(std::vector<uint8_t> & data)
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
                throw CompressionIOError("Z_MEM_ERROR while compressing");
                break;
            case Z_BUF_ERROR:
                throw CompressionIOError("Z_BUF_ERROR while compressing");
                break;
        }

        return compressedData;
    }

    void save
    (
        std::string file, 
        std::vector<uint8_t> data,
        std::string header
    )
    {
        uint64_t dataSize = data.size();

        std::vector<uint8_t> compressedData = deflate(data);

        std::ofstream out(file,std::ios::binary);
        if (out.is_open())
        {
            out << header << "\n";
            out << dataSize << "\n";

            for (unsigned c = 0; c < compressedData.size(); c++/*ayy lmao*/)
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