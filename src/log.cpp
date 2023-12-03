#include <log.h>
namespace Hop::Logging
{
  std::string get_time()
  {
    time_t time = std::time(nullptr);
    char time_buf[80];
    struct tm ts;
    ts = *localtime(&time);
    strftime(time_buf, sizeof(time_buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);

    return std::string(time_buf);
  }

  std::string operator+ (std::string s, const ERRORCODE e)
  {
    switch (e) 
    {
      case ERRORCODE::UNSPECIFIED:
        return s+std::string("UNSPECIFIED");
        break;
      case ERRORCODE::LUA_ERROR:
        return s+std::string("LUA");
        break;
      default:
        return s+std::string("UNSPECIFIED");
    }
  }

  std::ostream & operator<<(std::ostream & o, Log & l)
  {
    o << l.get();
    return o;
  }

}