#include <log.h>

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