#include <log.h>

std::string get_time()
{
  time_t time = std::time(nullptr);
  char time_buf[80];
  struct tm ts;
  ts = *localtime(&time);
  strftime(time_buf, sizeof(time_buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);

  return std::string(time_buf);
}

std::string operator+ (std::string s, const ERRORCODE e){
  switch (e) {
    case ERRORCODE::UNSPECIFIED:
      return s+std::string("UNSPECIFIED");
      break;
  }
}

std::ostream & operator<<(std::ostream & o, Log & l){
  o << l.get();
  return o;
}