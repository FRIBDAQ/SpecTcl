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
** The Q_element class below is a parameterized queue element type.
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
#if defined(VMS)
  friend class Queue;
  friend class QIterator;
#elif defined(unix)
#else
  friend class Queue<T>;
  friend class QIterator<T>;
#endif
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

#ifndef VMS
template <class T> 
#endif
class Queue {
 public:

  /* Constructors: */

  Queue() {
#ifdef VMS
    first = (Q_element<Xamine_RefreshContext> *)0;
    last  = (Q_element<Xamine_RefreshContext> *)0;
#else
    first = (Q_element<T> *)0;
    last  = (Q_element<T> *)0;
#endif
  }
  ~Queue() { Clear(); }		/* Destructor releases storage.     */
  /* Manipulators: */

  int IsEmpty() {		/* TRUE if queue is empty.          */
    return (first == 0); 
  }
#ifdef VMS
  void Append(Xamine_RefreshContext &entry);
#else
  void Append(T &entry);		/* Append new entry to queue tail. */
#endif
  void Clear();			/* Clear queue of all elements.    */
  void Remove();		/* Remove an entry from the queue  */
#ifdef VMS
  Xamine_RefreshContext &Peek();
#else
  T    &Peek();
#endif
  
#if defined(VMS)
  friend class QIterator;
#elif defined(unix)
#else
  friend class QIterator<T>;
#endif
#ifdef unix
public:
#else
private:
#endif
#ifdef VMS
  Q_element<Xamine_RefreshContext> *first;
  Q_element<Xamine_RefreshContext> *last;
#else
  Q_element<T> *first;
  Q_element<T> *last;
#endif
};

/*
** The QIterator class allows the client to iterate through a queue and
** remove individual objects from the queue.
*/
#ifndef VMS
template <class T>
#endif
class QIterator {
 public:

  /* Constructors: */
#ifdef VMS
  QIterator(Queue &queue) {
#else
  QIterator(Queue<T> &queue) {
#endif
    q = &queue;
    here = 0;
  }

  /* Manipulators:  */

  int Last() {			/* TRUE if looked at all entriess.  */
    return (here == q->last);
  }
#ifdef VMS
  Xamine_RefreshContext &Next();
#else
  T   &Next();			/* Get next entry.                  */
#endif
  void RemoveThis();		/* Remove previously gotten entry.  */

 private:
#ifdef VMS
  Queue *q;
  Q_element<Xamine_RefreshContext> *here;
#else
  Queue<T> *q;			/* The queue that we'll be manipulating */
  Q_element<T> *here;		/* Current location in queue.           */
#endif
};

#include "queue.cc"
#endif
