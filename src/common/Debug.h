#ifndef DEBUG_H
#define DEBUG_H

#include <SDL.h>
#include <iostream>

#include <map>
#include <string>
#include <typeinfo>

// Use namespace instead of static functions in a module, the preferred c++ way
// to prevent pollution of the Global namespace. Ref:
// http://stackoverflow.com/a/314630
namespace debug
{

  class MemoryAnalyser
  {
  private:
    std::map<std::string, int> constructed_types;
    std::map<std::string, int> deconstructed_types;

  public:
    template <typename T> void type_constructed(const T *type);

    template <typename T> void type_deconstructed(const T *type);

    void print();
  };
  extern MemoryAnalyser analyser;

  template <typename T> inline void MemoryAnalyser::type_constructed(const T *type)
  {
    // A virtual function must be defined for exact data type, else base class
    // will be used.
    constructed_types[typeid(*type).name()] += 1;
  }

  template <typename T> inline void MemoryAnalyser::type_deconstructed(const T *type)
  {
    // A virtual function must be defined for exact data type, else base class
    // will be used.
    deconstructed_types[typeid(*type).name()] += 1;
  }

  inline void log(const std::string &str)
  {
    std::cout << str << std::endl;
    ;
  }

  inline void runtime_error(std::string str)
  {
    log(str);
    throw std::runtime_error(str);
  }

  inline void logical_error(std::string str)
  {
    log(str);
    throw std::logic_error(str);
  }
} // namespace debug

#endif
