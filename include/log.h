#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include <ctime>
std::string get_time();

class Log {
public:
  Log(const char * s){
    logStream.clear();
    logStream.push_back(std::string(s));
  }

  Log(){}
  ~Log(){
    if (logStream.size()>0){
      put("\033[1;33m[WARN] \033[0mDestructor called on non-empty log, contents:\n\n");
      std::cout << get();
    }
  }

  void put(std::string s){
    s = get_time() + " " + s;
    logStream.push_back(s);
  }

  inline void put(const char * c){
    std::string s = get_time() + " " + std::string(c);
    logStream.push_back(s);
  }

  inline std::string get() {
    std::string l;
    for (int i = 0; i < logStream.size(); i++){
      l += logStream[i] + "\n";
    }
    logStream.clear();
    return l;
  }

private:
  std::vector<std::string> logStream;
};


std::ostream & operator<<(std::ostream & o, Log & l);

class LogType {
public:
  LogType(){}
  LogType(std::string s):msg(s){}
  LogType(const char * c):msg(c){}
  virtual std::string get()const {};
  void operator>> (Log & l){l.put(this->get());}
protected:
  std::string msg;
};

/*
  INFO log type leaves a [INFO] declarator and a msg
*/

class INFO : public LogType{
public:
  using LogType::LogType; // inherit all constructors
  std::string get()const{return u+msg;}
private:
  const char * u = "\033[1;34m[INFO] \033[0m";
};

/*
  WARN log type leaves a [WARN] declarator and a msg
*/

class WARN : public LogType{
public:
  using LogType::LogType; // inherit all constructors
  std::string get()const{return u+msg;}
private:
  const char * u = "\033[1;33m[WARN] \033[0m";
};

/*
  ERROR log type leaves a [ERROR] declarator follwed by a (ERRORCODE) and
  finally a msg
*/

enum ERRORCODE { UNSPECIFIED };

std::string operator+ (std::string s, const ERRORCODE e);

class ERROR : public LogType{
public:
  ERROR(std::string s) : LogType(s) {}
  ERROR(const char * c) : LogType(c) {}
  ERROR(ERRORCODE xx, std::string s) : LogType(s), x(xx) {}
  ERROR(ERRORCODE xx, const char * c) : LogType(c), x(xx) {}
  std::string get()const{return u+std::string("\033[1;33m(")+x+std::string(")\033[0m ")+msg;}
private:
  const char * u = "\033[1;31m[ERROR] \033[0m";
  ERRORCODE x = ERRORCODE::UNSPECIFIED;
};

/*
  PROGRESS log type leaves a msg followed by a progress meter
*/

class Progress {
public:
  Progress(
    uint64_t l,
    uint64_t p,
    std::vector<std::string> s = std::vector<std::string>{"\\","-","/","-"}
  )
  : length(l), progress(p), sequence(s)
  {}
  bool next(std::ostream & o){
    o << '\r';
    o.flush();
    if (progress < length){
      progress++;
      o << '\t' << sequence[progress % sequence.size()];
    }
    else {
      return true;
    }
  }
private:
  uint64_t length;
  uint64_t progress;
  std::vector<std::string> sequence;
};

#endif /* LOG_H */
