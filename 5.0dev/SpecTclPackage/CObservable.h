/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef _COBSERVABLE_H
#define _COBSERVABLE_H

#ifndef __STD_LIST
#include <list>
#ifndef __STD_LIST
#define __STD_LIST
#endif
#endif


#ifndef _C_STDLIB_H
#include <stdlib.h>
#ifndef _C_STDLIB_H
#define _C_STDLIB_H
#endif
#endif

 /**
  * @file CObservable.h
  * @brief Defines a templated observable class and observer.
  */

 /**
  * @class CObserver
  *
  * CObserver defines a templated observer class.
  * This is the observer part of the observable object pattern
  * described in the GOF patterns book.  The template parameter determines
  * What is passed into the observer.  To implement an object with observer
  * behavior:
  *   * Derive a class from CObserver with its template parameter being your
  *     class.
  *   * When the observed behavior occurs invoke observe method.
  *   * Observers derive from CObserver<T> where T is the type of the class
  *     above.
  * Example:
  *\code{.cpp}
  * #include <CObservable.h>
  *    class ConcreteObservable;            // may need forward definition.
  *    class ConcreteObservable : public CObservable<ConcreteObservable>
  *    {
  *      ...
  *    };
  *  
  *\endcode
  *
  *  Where some method(s) of ConcreteObservable will invoke observe (base class method).
  *  People observing an object anObservable will need to do stuff like:
  *
  * \code{.cpp}
  *#include <CObservable.h>
  *#include <ConcreteObservable.h>
  * 
  *   class ConcretObserver : public CObserver<ConcreteObservable> {
  *      virtual void operator()(ConcreteObservable pObservable);
  *   };
  *
  *   ...
  *     anObservable.addObserver(new ConcreteObserver);
  * \endcode
  *
  * An arbitary number of observers can be managed by an observable.
  * See CObservable below for the public interface to observables.
  *
  *  
  */
template<class T>
class CObservable;

 template <class T>
 class CObserver {
 public:
   virtual ~CObserver() {}
   virtual void operator()(CObservable<T>*  pObservable) = 0;
 };


 /**
  * @class CObservable
  *
  *   Templated base class for objects that want to be observed.
  *  See the comments for the CObserver class for most of they 
  *  documentation about observers.
  */
 template <class T>
 class CObservable {
   // Private data types:
 private:
   typedef std::list<CObserver<T>*> ObserverList;

   // Public data types:
 public:
   typedef typename CObservable<T>::ObserverList::iterator ObserverIterator;

   // private per object data:

 private:
   ObserverList m_observers;

   // Public canonicals:
 public:
   virtual ~CObservable() {}

   // Observer interface:

 public:
   void addObserver(CObserver<T>* pObserver) {
     m_observers.push_back(pObserver);
   }
   void removeObserver(CObserver<T>* pObserver) {
     ObserverIterator p = beginObservers();
     while (p != endObservers()) {
       if (pObserver == *p) {
	 m_observers.erase(p);
	 break;
       }
       p++;
     }
   }
   size_t observerCount() const {return m_observers.size(); }
   typename CObservable<T>::ObserverIterator beginObservers() {return m_observers.begin(); }
   typename CObservable<T>::ObserverIterator endObservers()  {return m_observers.end(); }
 protected:
   void observe() {
     ObserverIterator p = beginObservers();
     while (p != endObservers()) {

       (**p)(this);
       p++;
     } 
   }
};
#endif
