
#ifndef UTILITY_H
#define UTILITY_H

#include <memory>

template<class T> std::unique_ptr<T> make_unique(T* ptr) 
{
  return std::unique_ptr<T>(ptr);
}

#endif
