/*
** Facility:
**  Xamine -- NSCL display program.
** Abstract:
**   queue.cc:
**      This file contains implementation code for the classes and
**      objects defined in queue.h  These implement non-intrusive
**      queue classes and associated iterators.
** Author:
**  Ron Fox
**  NSCL
**  Michigan State University
**  East Lansing, MI 48824-1321
*/

/*
** Include files:
*/
#include "queue.h"
#include <assert.h>


/*
** Functional Description:
**   Queue::Append:
**     Append a queue entry to the end of a queue.
** Formal Parameters:
**  T entry:
**     The entry to append.
*/
#ifdef VMS
void Queue::Append(Xamine_RefreshContext &entry)
#else
template <class T>
void Queue<T>::Append(T &entry)
#endif
{
#ifdef VMS
  Q_element<Xamine_RefreshContext> *qe = 
          new Q_element<Xamine_RefreshContext>(entry);
#else
  Q_element<T> *qe = new Q_element<T>(entry);
#endif

  if (first == 0) {		/* queue empty. */
    last = qe;
    first= qe;
  }
  else {			/* Queue not empty: */
#ifdef VMS
    Q_element<Xamine_RefreshContext> *ol = last;
#else
    Q_element<T> *ol = last;
#endif
    qe->Link(*ol);
    last = qe;
  }

}

/*
** Functional Description:
**    Queue::Clear:
**      Clears the queue of all elements.
*/
#ifdef VMS
void Queue::Clear()
#else
template <class T>
void Queue<T>::Clear()
#endif
{
  while(first != 0) {
#ifdef VMS
    Q_element<Xamine_RefreshContext> *q = first;
#else
    Q_element<T> *q = first;
#endif
    first           = q->Next();
    delete q;
  }
  first = 0;
  last = 0;
}

/*
** Functional Description:
**   Queue::Remove:
**     Removes the entry at the front of the queue from the list.
** Returns:
**   Nothing.
*/
#ifdef VMS
void Queue::Remove()
#else
template <class T>
void Queue<T>::Remove()
#endif
{
  assert(first != 0);
#ifdef VMS
  Q_element<Xamine_RefreshContext> *q;
#else
  Q_element<T> *q;
#endif
  q = first;
  first = q->Next();
  delete q;
  if(first == 0) last = 0;
}

/*
** Functional description:
**   Queue::Peek:
**     Returns the contents of the first entry in the queue.
*/
#ifdef VMS
Xamine_RefreshContext &Queue::Peek()
#else
template <class T>
T &Queue<T>::Peek()
#endif
{
  assert(first != 0);
#ifdef VMS
  Q_element<Xamine_RefreshContext> *q = first;
#else
  Q_element<T> *q = first;
#endif
  return q->Entry();
}

/*
** Functional Description:
**   QIterator::Next:
**      Returns a reference to the contents of the next entry in the queue.
*/
#ifdef VMS
Xamine_RefreshContext &QIterator::Next()
#else
template <class T>
T &QIterator<T>::Next()
#endif
{
  assert(!Last());

  if(here == 0) {
    here = q->first;
    return here->Entry();
  }
  else {
    here = here->Next();
    return here->Entry();
  }

}

/*
** Functional Description:
**   QIterator::RemoveThis:
**     This function removes the element that was last looked at via Next.
*/
#ifdef VMS
void QIterator::RemoveThis()
#else
template <class T>
void QIterator<T>::RemoveThis()
#endif
{
  
  assert(here != 0);

  if(here == q->first) {	/* Remove first element: */
    q->Remove();
    here = 0;
  }
  else {			/* Locate the element just prior to here. */
#ifdef VMS
    Q_element<Xamine_RefreshContext> *p = q->first;
#else
    Q_element<T> *p = q->first;
#endif

    while(p->Next() != here)
      p = p->Next();
    here->UnLink(*p);		/* Unlink *here from the queue. */
    if(here == q->last) 
      q->last = p;
    delete here;		/* Release storage. */
    here = p;			/* Backup here so that next will work. */
  }

}
