#ifndef Z_H
#define Z_H

#include <string>
#include <vector>
#include <exception>
#include <fstream>
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

    std::vector<uint8_t> load(std::string file);

    void save
    (
        std::string file, 
        std::vector<uint8_t> data,
        std::string header = "compressed file, next line is uncompressed size"
    );
}

#endif /* Z_H */
