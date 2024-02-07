#include "main.h"

#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

const uint8_t width = 80;

std::string byteToHex(char c)
{
    std::stringstream ss;

    ss << std::hex << std::setw(2) << std::setfill('0') << c;

    std::string s = ss.str();

    return "0x"+s;
}

int main(int argc, char ** argv)
{
    if (argc == 2)
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

        std::vector<uint8_t> zd = Hop::Util::Z::deflate(bytes);

        std::ofstream out(file+".z",std::ios::binary);

        if (argc == 3)
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
                for (uint8_t c : zd)
                {
                    out << c;
                    w += 5;

                    if (w >= width)
                    {
                        out << "\n";
                        w = 0;
                    }
                    else
                    {
                        out << ",";
                    }
                }
                out << "}";
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