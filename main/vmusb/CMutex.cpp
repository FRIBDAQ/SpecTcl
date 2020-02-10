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
#include <config.h>
#include "CMutex.h"

#include <ErrnoException.h>
#include <errno.h>
#include <pthread.h>

//////////////////////////////////////////////////////////////////////////////////////
//////// Implement members of the CMutexAttr struct       ////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

/*!
  Construct a CMutexAttr&. The member is just initialized and
  CErrnoException thrown if there was an error:
*/
CMutexAttr::CMutexAttr()
{
  int status = pthread_mutexattr_init(&m_attributes);
  throwifbad(status, "CMutexAttr::constructor Initializing mutex attributes");

}
/*!
   Destruction just destroys the attribute block, reporting a CErrnoException
   if there's an error:
*/
CMutexAttr::~CMutexAttr()
{
  int status = pthread_mutexattr_destroy(&m_attributes);
  throwifbad(status, "CMutexAttr:destructor destroying mutex attributes");
}

/*!
  Set the attributes block for a sharable resource.  Shared mutexes can be
  created in shared memory and used for inter-process synchronization.
*/
void 
CMutexAttr::setShared()
{
  int status = pthread_mutexattr_setpshared(&m_attributes, PTHREAD_PROCESS_SHARED);
  throwifbad(status, "CMutexAttr:setShared");

}
/*!
  Set the mutex attributes to process private.  The mutex can only synchronize threads
  in the creating process.
*/
void
CMutexAttr::setUnshared()
{
  int status = pthread_mutexattr_setpshared(&m_attributes, PTHREAD_PROCESS_PRIVATE);
  throwifbad(status, "CMutexAttr::setUnshared");
}
/*!
  Get the state of the shared  flag
  \return bool
  \retval true - mutex can be used to synchronize processes.
  \retval false - Mutex can only be used within the process.
*/
bool
CMutexAttr::isShareable()
{
  int value;
  int status = pthread_mutexattr_getpshared(&m_attributes, &value);
  throwifbad(status, "CMutexAttr::isShareable");
  return value == PTHREAD_PROCESS_SHARED;
}
/*!
  Sets the mutex type:
  \param type - an integer code that can be one of:
  - PTHREAD_MUTEX_NORMAL - mutex does not detect deadlock 
  - PTHREAD_MUTEX_ERRORCHECK - recursively locking a mutex is an error.
  - PTHREAD_MUTEX_RECURSIVE - Recursive mutex locking is supported with a lock depth count.
  - PTHREAD_MUTEX_DEFAULT - Same meaning as PTHEAD_MUTEX_NORMAL in linux anyway,
                            I won't gaurentee this is the same value though.
*/
void
CMutexAttr::setType(int type)
{
  int status = pthread_mutexattr_settype(&m_attributes, type);
  throwifbad(status, "CMutexAttr::setType");
}
/*!
   \return int
   \retval one of the mutex type codes above.
*/
int
CMutexAttr::getType()
{
  int type;
  int status = pthread_mutexattr_gettype(&m_attributes, &type);
  throwifbad(status, "CMutexAttr::getType");
  return type;
}

/*
** if a status is bad, thrown an errno exception:
*/
void
CMutexAttr::throwifbad(int status, const char* msg)
{
  if (status != 0) {
    errno = status;
    throw CErrnoException(msg);
  }
}
////////////////////////////////////////////////////////////////////////////////////////
///////////////////////// Implement members of the CMutex class ////////////////////////
////////////////////////////////////////////////////////////////////////////////////////



/*!
   Construct the mutex with the default attributes:
*/
CMutex::CMutex()
{
  create(reinterpret_cast<pthread_mutexattr_t*>(NULL));
}
/*!
  Construct the mutex given a normal attributes block:
  \param attributes reference to the attributes block.
*/
CMutex::CMutex(pthread_mutexattr_t& attributes)
{
  create(&attributes);
}
/*!
  Construct the mutex given an encapsulated attributes block.
  \param attributes - the CMutexAttr that encapsulates the attributes.
*/
CMutex::CMutex(CMutexAttr& attributes)
{
  create(&(attributes.m_attributes));
}
/*!
   Destructor must detroy the mutex:
*/
CMutex::~CMutex()
{
  pthread_mutex_destroy(&m_mutex);
}

/*! 
   Lock the mutex, blocking if needed.
*/
void
CMutex::lock()
{
  int status = pthread_mutex_lock(&m_mutex);
  CMutexAttr::throwifbad(status, "CMutex::lock failed");
}
/*!
  Attempt lock without blocking.  Returns true on success.
*/
bool
CMutex::trylock()
{
  int status = pthread_mutex_trylock(&m_mutex);
  if (status == 0) return true;
  if (status == EBUSY) return false;

  CMutexAttr::throwifbad(status, "CMutex::trylock");
}
/*!
   Unlock the mutex:
*/
void 
CMutex::unlock()
{
  int status = pthread_mutex_unlock(&m_mutex);
  CMutexAttr::throwifbad(status, "CMutex::unlock");
}

/*
** create the mutex given an attribute block.
*/
void
CMutex::create
(pthread_mutexattr_t* pAttributes)
{
  int status = pthread_mutex_init(&m_mutex, pAttributes);
  CMutexAttr::throwifbad(status, "CMutex::Create");
}
