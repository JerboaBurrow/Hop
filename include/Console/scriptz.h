#ifndef SCRIPTZ_H
#define SCRIPTZ_H

#include <Util/z.h>
#include <Util/util.h>

#include <Console/lua.h>
#include <Console/LuaString.h>

#include <unordered_map>
#include <string>
#include <vector>
#include <fstream>

#include <json.hpp>
using json = nlohmann::json;

namespace Hop
{
    static const char * SCRIPTZ_FILE_EXTENSION = ".scriptz";
    static const char * SCRIPTZ_HEADER = "Hop scriptz file, a zlib compressed JSON dump of lua scripts, next line is the uncompressed size";

    class Scriptz
    {

    public:

        Scriptz(std::string s = std::string(SCRIPTZ_HEADER))
        {
            std::string h = "";
            for (char c : s)
            {
                if (c != '\n')
                {
                    h += c;
                }
            }
            header = h;
        }

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

                if (!Hop::Util::endsWith(file, SCRIPTZ_FILE_EXTENSION))
                {
                    file = file + SCRIPTZ_FILE_EXTENSION;
                }

                Hop::Util::Z::save(file, bytes, header);
            }
        }

        void add(std::string name, std::string script) 
        {
            scripts[name] = script;
        }

        void remove(std::string name){ scripts.erase(name); }

        std::string get(std::string name) 
        { 
            if (scripts.find(name) != scripts.cend())
            {
                return scripts[name];
            }
            else
            {
                if (Hop::Util::endsWith(name, ".lua"))
                {
                    std::string n = name.substr(0, name.find(".lua"));
                    return scripts[n];
                }
                else
                {
                    return scripts[name+".lua"];
                }
            }
        }

        std::unordered_map<std::string, std::string>::const_iterator cbegin() const { return scripts.cbegin(); }
        std::unordered_map<std::string, std::string>::const_iterator cend() const { return scripts.cend(); }

        const size_t size() const { return scripts.size(); }

        int require(lua_State * lua)
        {
            int n = lua_gettop(lua);

            if (n != 1)
            {
                lua_pushliteral(lua, "expected a string script name as argument");
                return lua_error(lua);
            }

            LuaString script;

            script.read(lua, 1);

            std::string body = get(script.characters);

            if (body == "")
            {
                lua_pushliteral(lua, "script not found");
                return lua_error(lua);
            }

            luaL_dostring(lua, body.c_str());

            return 0;
        }

    private:

        std::string header;
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
