#ifndef SCRIPTZ_H
#define SCRIPTZ_H

#include <Util/z.h>

#include <unordered_map>
#include <string>
#include <vector>
#include <fstream>

#include <vendored/json.hpp>
using json = nlohmann::json;

namespace Hop
{
    class Scriptz
    {

    public:

        const char * FILE_EXTENSION = ".scriptz";
        const char * HEADER = "Hop scriptz file, a zlib compressed JSON dump of lua scripts, next line is the uncompressed size";

        Scriptz()
        {}

        void load(std::string file)
        {
            std::vector<uint8_t> data = Hop::Util::Z::load(file);

            json parsed = json::parse(data.cbegin(), data.cend());
            
            for (auto c : parsed.items())
            {
                if (c.value().is_string())
                {
                    add(c.key(), c.value());
                }
            }
        }

        void save(std::string file)
        {
            if (size() > 0)
            {
                json data;

                for (auto s : scripts)
                {
                    data[s.first] = s.second;
                }

                std::string dump = data.dump();
                std::vector<uint8_t> bytes(dump.begin(), dump.end());

                std::string end = FILE_EXTENSION;

                if (file.size() > end.size())
                {
                    if (!std::equal(end.rbegin(), end.rend(), file.rbegin()))
                    {
                        file = file + FILE_EXTENSION;
                    }
                }
                else
                {
                    file = file + FILE_EXTENSION;
                }

                Hop::Util::Z::save(file, bytes, HEADER);
            }
        }

        void add(std::string script, std::string name) 
        {
            scripts[name] = script; 
        }

        void remove(std::string name){ scripts.erase(name); }

        std::string get(std::string name) { return scripts[name]; }

        std::unordered_map<std::string, std::string>::const_iterator cbegin() const { return scripts.cbegin(); }
        std::unordered_map<std::string, std::string>::const_iterator cend() const { return scripts.cend(); }

        const size_t size() const { return scripts.size(); }

    private:

        std::unordered_map<std::string, std::string> scripts;

        class ScriptzIOError: public std::exception 
        {

        public:

            ScriptzIOError(std::string msg)
            : msg(msg)
            {}

        private:

            virtual const char * what() const throw()
            {
                return msg.c_str();
            }
            std::string msg;
        };

    };
}
#endif /* SCRIPTPACK_H */
