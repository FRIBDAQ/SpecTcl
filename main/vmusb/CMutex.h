#ifndef __CMUTEX_H
#define __CMUTEX_H
/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef __CRT_PTHREAD_H
#include <pthread.h>
#ifndef __CMUTEX_H
#define __CMUTEX_H
#endif
#endif

/*!
  Direct encapsulation of a pthread mutex attribute block.  We only support the
attributes in the base pthreads specification (not the realtime nor the
advanced realtime specs), as you should be able to count on those always being
available

*/
struct CMutexAttr
{
  pthread_mutexattr_t  m_attributes;
  CMutexAttr();
  ~CMutexAttr();

  void setShared();
  void setUnshared();
  bool isShareable();
  
  void setType(int type);
  int  getType();

  static void  throwifbad (int status, const char* message);
};


/*!
  Direct encapsulation of a pthread  mutex.

*/
class CMutex
{
public:
  pthread_mutex_t      m_mutex;            //  The underlying mutex.
  
public:
  CMutex();
  CMutex(pthread_mutexattr_t& attributes);
  CMutex(CMutexAttr&          attributes);
  virtual ~CMutex();

private:
  CMutex(const CMutex&);
  CMutex& operator=(const CMutex&);
  int operator==(const CMutex&) const;
  int operator!=(const CMutex&) const;



  // Synchronization operations.
public:

  void lock();
  bool trylock();
  void unlock();

private:
  void create(pthread_mutexattr_t* pAttributes);
};


/**
 *  Safe lock/unlock of mutexes
 *  Declaring a CriticalSection object in a block will lock the mutex and
 *  gaurantee the mutex is unlocked when the block is exited.
 *  The scope of the block is the period the mutex is held.
 *  
 */
class CriticalSection {
private:
    CMutex& m_mutex;
public:
    CriticalSection(CMutex& mutex) : m_mutex(mutex)
    {
        m_mutex.lock();
    }
    ~CriticalSection() {
        m_mutex.unlock();
    }
};

#endif
