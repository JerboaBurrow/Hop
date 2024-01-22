#include "main.h"

#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>

int main(int argc, char ** argv)
{
    if (argc == 2)
    {
        // unpack
        std::string file = argv[1];
        scriptz.load(file);

        std::cout << "Found " << scriptz.size() << " packed files\n Extracting to packout/\n";

        std::filesystem::create_directory("packout");

        for (auto it = scriptz.cbegin(); it != scriptz.cend(); it++)
        {
            std::ofstream out("packout/"+it->first);
            out << it->second;
        }
    }
    else if (argc >= 3)
    {
        // pack
        int f = 2;
        while (f < argc)
        {
            std::ifstream in(argv[f]);

            if (in.is_open())
            {
                std::cout << "Packing file " << argv[f] << "\n";
                std::string file, line;

                while (std::getline(in, line))
                {
                    file += line+"\n";
                }

                scriptz.add(argv[f], file);
            }
            else
            {
                std::cout << "could not open that file...\n";
            }


            f++;
        }

        std::string end = scriptz.FILE_EXTENSION;
        std::string file = argv[1];

        if (file.size() > end.size())
        {
            if (!std::equal(end.rbegin(), end.rend(), file.rbegin()))
            {
                file = file + scriptz.FILE_EXTENSION;
            }
        }
        else
        {
            file = file + scriptz.FILE_EXTENSION;
        }
        
        std::cout << "saving scripts to " << file;

        scriptz.save(file);
    }

    return 0;
}