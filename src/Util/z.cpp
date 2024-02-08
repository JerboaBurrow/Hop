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

        std::unique_ptr<uLong> n = std::make_unique<uLong>(cdata.size());

        int result = uncompress2(

            &rawData[0],
            &decompressedSize,
            &cdata[0],
            n.get()
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

    std::vector<uint8_t> deflate(std::vector<uint8_t> & data, uint8_t level)
    {

        if (level > Z_BEST_COMPRESSION)
        {
            level = Z_BEST_COMPRESSION;
        }

        std::vector<uint8_t> compressedData(data.size()*1.1+12);

        uint64_t dataSize = data.size();
        // long unsigned int for windows rather than uint64_t
        long unsigned int bufferSize = compressedData.size();

        int result = compress2(

            &compressedData[0],
            &bufferSize,
            &data[0],
            dataSize,
            level
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

        return std::vector<uint8_t>(compressedData.begin(),compressedData.begin()+bufferSize);
    }

    void save
    (
        std::string file, 
        std::vector<uint8_t> data,
        std::string header,
        uint8_t level
    )
    {
        uint64_t dataSize = data.size();

        std::vector<uint8_t> compressedData = deflate(data, level);

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