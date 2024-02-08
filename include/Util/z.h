#ifndef Z_H
#define Z_H

#include <string>
#include <vector>
#include <exception>
#include <fstream>
#include <memory>

#include <zlib.h>

namespace Hop::Util::Z
{

    class CompressionIOError: public std::exception 
    {

    public:

        CompressionIOError(std::string msg)
        : msg(msg)
        {}

    private:

        virtual const char * what() const throw()
        {
            return msg.c_str();
        }
        std::string msg;
    };

    std::vector<uint8_t> inflate(std::vector<uint8_t> & cdata, long unsigned int decompressedSize);

    std::vector<uint8_t> deflate(std::vector<uint8_t> & data, uint8_t level = Z_DEFAULT_COMPRESSION);

    std::vector<uint8_t> load(std::string file);

    void save
    (
        std::string file, 
        std::vector<uint8_t> data,
        std::string header = "compressed file, next line is uncompressed size",
        uint8_t level = Z_DEFAULT_COMPRESSION
    );
}

#endif /* Z_H */
