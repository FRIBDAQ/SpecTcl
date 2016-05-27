static const char* Copyright = "(C) Copyright Michigan State University 1994, All rights reserved";
/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**   superpos.cc 
**     This file contains implementations of the class methods for the classes
**     Superposition               - A single spectrum superposition,
**     SuperpositionList           - A list of superpositions.
**     SuperpositionListIterator   - Sequential access to a superposition list.
** Author:
**  Ron Fox
**  NSCL
**  Michigan State University
**  East Lansing, MI 48824-1321
*/

/*
** Include files:
*/
#include <config.h>
#include <assert.h>

#include "superpos.h"

/*
** Functional Description:
**    SuperpositionList::Add 
**      Add a spectrum to a superposition list.
**      To do this we must assign a rendition index and
**      put the spectrum on the end of the list.  We error out if the
**      list gets too big.
** Formal Parameters:
**   int spec:
**     Number of the spectrum to add.
*/
void SuperpositionList::Add(int s)
{

  /*  Make sure the input data is correct: */

  assert(s != Super_Undefined);
  assert(num < MAX_SUPERPOSITIONS);

  /* Assign the rendition:  */

  lastrep++;
  lastrep = lastrep % MAX_SUPERPOSITIONS;

  /* Set the list value: */

  list[num].Spectrum(s);
  list[num].Representation(lastrep);
  num++;

}

void SuperpositionList::Add(const std::string& name)
{

  /*  Make sure the input data is correct: */

  assert(num < MAX_SUPERPOSITIONS);

  /* Assign the rendition:  */

  lastrep++;
  lastrep = lastrep % MAX_SUPERPOSITIONS;

  /* Set the list value: */

  list[num].SpectrumName(name);
  list[num].Representation(lastrep);
  num++;

}

/*
** Functional Description:
**   SuperpostionListIterator::Next:
**     Returns a reference to the next spectrum in the list.
**     bombs out if there is no next one.
*/
Superposition & SuperpositionListIterator::Next()
{
  int loc = location;

  /* Check the legality of the operation. */

  assert(location < slist->num);

  location++;
  return slist->list[loc];
}

/*
** Functional Description:
**  SuperpositionListIterator::Last:
**    Returns true if the next call to Next would fail.
*/
int SuperpositionListIterator::Last()
{
  return !(location < slist->num);
}

/*
** Functional Description:
**   SuperpositionListIterator::DeleteCurrent:
**    Delete the most recently looked at entry.  fail if there isn't one.
*/
void SuperpositionListIterator::DeleteCurrent()
{
  /* Ensure that the location pointer is not at initial point */

  assert(location > 0);

  /* The last entry (if any) is placed in the delete position and 
  ** the location pointer is backed up by one.
  */

  location--;
  slist->num--;
  if(location < slist->num) 
    slist->list[location] = slist->list[slist->num];

}
/*
** Functional Description:
**  SuperpositionList::SuperpositionList:
**     Construct a superposition list.
** Formal Parmeters:
**    SuperPositionList &l:
**      Makes this a copy constructor.
*/
SuperpositionList::SuperpositionList(SuperpositionList &l)
{
  num = l.num;
  lastrep = l.lastrep;
  for(int i = 0; i < MAX_SUPERPOSITIONS; i++) {
    if(i < num) list[i] = l.list[i];
    else        list[i].Spectrum(Super_Undefined);
  }
}




