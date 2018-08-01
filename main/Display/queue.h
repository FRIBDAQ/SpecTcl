/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


/*
** Facility:
**   Xamine - NSCL Display program support routines.
** Abstract:
**   queue.h 
**     Defines a set of related classes which implement a non-intrusive
**     singly linked queue.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
** SCCS info:
**   @(#)queue.h	1.1 5/26/94 
*/
#ifndef QUEUE_H_INSTALLED
#define QUEUE_H_INSTALLED

/*
** The Q_element class below is a parametrized queue element type.
** It contains a single forward link and a body containing the data
** associated with the queue node.
*/
template <class T> 
class Q_element {
 public:
  /* Constructors which generate Q_element s.   */

  Q_element(const T &body) {
    next = (Q_element<T> *)0;
    entry= body;
  }
  Q_element(const T &body, Q_element<T> *n) {
    next = n;
    entry= body;
  }
  /* Constructors which copy and convert Q_element s. */

  Q_element(const Q_element<T> &q) {
    next = q.next;
    entry= q.entry;
  }

  T &operator=(Q_element<T> &q) {
    return q.entry;
  }
  /* Manipulators:   */

  void Link(Q_element<T> &prior) {	/* Link after an element. */
    next = prior.next;
    prior.next = this;
  }
  void UnLink(Q_element<T> &prior) { /* Unlink given prior element. */
    prior.next = next;
    next       = (Q_element<T> *)NULL;
  }
  Q_element<T> *Next() { return next; }
  T &Entry()   { return entry; }
 private:
  Q_element<T> *next;		/* Pointer to next element in queue. */
  T         entry;		/* An entry in the queue.            */
};
/* 
** The Queue class implements a queue.
** It contains a queue header which points to the first element in the queue
** and a queue tail which points to the last element of the queue.
** Operators allow insertion into the queue, peeking at the first entry
** dequeuing the first entry, and adding entries to the queue<.
** A friend class QIterator allows one to traverse the entries in the
** list, as well as to do some other less structured operations such as
** removing arbitrary queue entries.
*/

template <class T> 
class Queue {
 public:

  /* Constructors: */

  Queue() {
    first = (Q_element<T> *)0;
    last  = (Q_element<T> *)0;
  }
  ~Queue() { Clear(); }		/* Destructor releases storage.     */
  /* Manipulators: */

  int IsEmpty() {		/* TRUE if queue is empty.          */
    return (first == 0); 
  }
  void Append(T &entry);		/* Append new entry to queue tail. */
  void Clear();			/* Clear queue of all elements.    */
  void Remove();		/* Remove an entry from the queue  */
  T    &Peek();
  
public:
  Q_element<T> *first;
  Q_element<T> *last;
};

/*
** The QIterator class allows the client to iterate through a queue and
** remove individual objects from the queue.
*/
template <class T>
class QIterator {
 public:

  /* Constructors: */
  QIterator(Queue<T> &queue) {
    q = &queue;
    here = 0;
  }

  /* Manipulators:  */

  int Last() {			/* TRUE if looked at all entriess.  */
    return (here == q->last);
  }
  T   &Next();			/* Get next entry.                  */
  void RemoveThis();		/* Remove previously gotten entry.  */

 private:
  Queue<T> *q;			/* The queue that we'll be manipulating */
  Q_element<T> *here;		/* Current location in queue.           */
};

#include "queue.cc"
#endif
