#include "main.h"

int main(int argc, char ** argv)
{
    jLog::Log log;

    Hop::Console console(log);
    Hop::LuaExtraSpace luaStore;
    Hop::Scriptz scripts;

    luaStore.console = &console;
    luaStore.scripts = &scripts;
    console.luaStore(&luaStore);

    scripts.load("pack.scriptz");

    console.runScript("a");
    std::string status = console.luaStatus();
    if (status != "LUA_OK") { WARN(status) >> log; }

    return 0;
}
