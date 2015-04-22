
#ifndef UTILITY_H
#define UTILITY_H

#include <memory>

namespace Viewer
{

template<class T> std::unique_ptr<T> make_unique(T* ptr) 
{
  return std::unique_ptr<T>(ptr);
}

} // end of namespace

#endif
