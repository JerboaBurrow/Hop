#include "main.h"

#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

const uint8_t width = 80;

std::string byteToHex(uint8_t c)
{
    std::stringstream ss;

    ss << std::hex << std::setw(2) << std::setfill('0') << int(c);

    std::string s = ss.str();

    return "0x"+s;
}

int main(int argc, char ** argv)
{
    if (argc >= 2)
    {
        // read in a file
        std::string file = argv[1];
        
        std::ifstream in(file,std::ios::binary);

        std::vector<uint8_t> bytes;

        char c;

        while (in.get(c))
        {
            bytes.push_back(c);
        }

        uint8_t level = Z_DEFAULT_COMPRESSION;
        if (argc >= 4)
        {
            level = std::stoul(argv[3]);
        }

        std::vector<uint8_t> zd = Hop::Util::Z::deflate(bytes, level);

        std::ofstream out(file+".z",std::ios::binary);

        std::cout << "Input/Compressed length: " << bytes.size() << " / " << zd.size() << "\n";

        if (argc >= 3)
        {
            std::string option = argv[2];
            if (option == "-dump")
            {
                for (uint8_t c : zd)
                {
                    out << c;
                }
            }
            else if (option == "-hexdump")
            {
                unsigned w = 0;
                for (uint8_t c : zd)
                {
                    out << byteToHex(c);
                    w += 5;

                    if (w >= width)
                    {
                        out << "\n";
                        w = 0;
                    }
                    else
                    {
                        out << " ";
                    }
                }
            }
            else if (option == "-bytearray")
            {
                out << "static const unsigned char bytes[] __attribute__((unused)) = {\n";
                unsigned w = 0;
                unsigned n = 0;
                for (uint8_t c : zd)
                {
                    out << byteToHex(c);
                    w += 5;

                    if (w >= width)
                    {
                        out << ",\n";
                        w = 0;
                    }
                    else if (n < zd.size()-1)
                    {
                        out << ",";
                    }
                    n+=1;
                }
                out << "};";
            }
            else
            {
                std::cout << "Choose either a -dump, a -hexdump, or a -bytearray\n";
            }
        }
        else
        {
            for (uint8_t c : zd)
            {
                out << c;
            }
        }

        out.close();
    }

    return 0;
}