/*
** Facility:
**   Display program graphical object database
** Abstract:
**   grobjdb.cc  - This file contains code which implements the methods
**                 for a graphical objects database.
** Author:
**   Ron Fox
**   NSCL 
**   Michigan State University
**   East Lansing, MI 48824-1321
** Version:
**    @(#)grobjdb.cc	8.1 6/23/95 
*/
#include <stdio.h>
#include <errno.h>
#include "dispgrob.h"
#ifndef TRUE
#define TRUE -1
#define FALSE 0
#endif

/*
** Method Description:
**    enter    - This function enters a new graphical object in a database.
**               Since we want to allow the user to pass in temporarily
**               scoped variables, we must instantiate a new copy of the
**               object prior to entry.  Since each graphical object has 
**               virtual functions, the correct argument must be passed in to
**               new which requires that we have several overloads for enter
**               all but one of them just new the variable and pass it on
**               to the generic enter.
**   enteras   - Same as enter, but uses the id in the grobj rather than
**               assigning a new one.
** Formal Parameters:
**   const grobj_??? &object:
**     Reference to the object instance we wish to enter in the database.
**     The object is assigned an Id by the generic entry function.
** Returns:
**   grobj_generic * - Pointer to the entered object if successful.
**   NULL            - If there was a failure... can only be new failure
**                     or full database.
*/

/* Generic function does all the work: */

grobj_generic *grobj_database::enteras(grobj_generic *obj)
{
  grobj_generic *object;

  object =obj->clone();
  if(object == NULL) {
    errno = ENOMEM;
    return NULL;
  }
  if(obj_count < GROBJ_MAXOBJECTS) {
    object->setid(obj->getid());
    objects[obj_count] = object;
    obj_count++;
    return object;
  }else {
    delete object;
    errno = ENOMEM;
    return (grobj_generic *)NULL;
  }
}
grobj_generic *grobj_database::enter(grobj_generic *obj)
{
  grobj_generic *object;

  object =obj->clone();
  if(object == NULL) {
    errno = ENOMEM;
    return NULL;
  }
  if(obj_count < GROBJ_MAXOBJECTS) {
    object->setid(lastid);
    lastid++;
    objects[obj_count] = object;
    obj_count++;
    return object;
  }else {
    delete object;
    errno = ENOMEM;
    return (grobj_generic *)NULL;
  }

}

/*
** Method Description:
**   grobj_database::remove:
**     This function removes an object from the object database.
**     This is done by locating the object.  If the object does not match,
**     then the operation fails with ENOENT.   The object storage is deleted
**     and the last object in the list is moved to the position of the
**     deleted object.  After this the object count is decremented.
** Formal Parameters:
**    grobj_generic *entry:
**      An entry in the database.  Must have been returned by one of the
**      location or iteration functions.
** Returns:
**    -1   - Success.
**     0   - Fails.
*/
int grobj_database::remove(grobj_generic *entry)
{

  for(int i = 0; i < obj_count; i++) {
    if(objects[i] == entry) {
      obj_count--;
      if(obj_count > 0)
	objects[i] = objects[obj_count];
      delete entry;
      return -1;
    }
  }
  errno = ENOENT;
  return 0;
}

/*
** Method Description:
**   replace   - This function replaces a database entry with a different
**               set of values.  Again, to support polymorphism, we have
**               a pile of front end functions feeding the generic
**               function which does all the real work.
** Formal Parameters:
**   int spec:
**      spectrum the object belongs to.
**   int id:
**      id of object being replaced.
**   grobj_???? &object:
**      reference or pointer (depending on the function) to the 
**      object to use to replace
** Returns:
**   NULL _ Failure
**   Pointer to replaced grobj.
**   The replaced grobj retains the id and name of the old
**   object, only the spectrum number, type and point list can be modified.
** Errors:
**    ENOMEM   - NEW failed.
**    ENOENT   - No match with the requested object.
**/
grobj_generic *grobj_database::replace(int id, grobj_generic *obj)
{
  grobj_generic *robj;		/* Pointer to replaced object */
  grobj_generic *object;
  grobj_name    name;
  int           i;

  for(i = 0; i < obj_count; i++) {
    if(objects[i]->getid() == id) { /*  Replace this one */
      object = obj->clone();
      if(object == NULL ) {
	errno = ENOMEM;
	return NULL;
      }
      robj = objects[i];
      object->setid(robj->getid());
      if(robj->named()) object->setname(robj->getname(name));
      objects[i] = object;
      delete robj;
      return object;
    }
  }
  errno = ENOENT;
  return (grobj_generic *)NULL;
}

      
/*  The versions of replace below also cause the search to be limited to
**  a single spectrum and cause the replaced entry to retain a spectrum 
**  number.
*/
grobj_generic *grobj_database::replace(int spec,int id, grobj_generic *obj)
{
  grobj_generic *robj;		/* Pointer to replaced object */
  grobj_name    name;
  int           i;
  grobj_generic *object;

  for(i = 0; i < obj_count; i++) {
    if((objects[i]->getid() == id) && 
       (objects[i]->getspectrum() == spec)) { /*  Replace this one */
      object = obj->clone();
      if(object == NULL) {
	errno = ENOMEM;
	return NULL;
      }
      robj = objects[i];
      object->setid(id);
      object->setspectrum(spec);
      if(robj->named()) object->setname(robj->getname(name));
      objects[i] = object;
      delete robj;
      return object;
    }
  }
  errno = ENOENT;
  return (grobj_generic *)NULL;
}

/*
** Method description:
**     find            - Methods which locate graphical objects according
**                       to various criteria:
** Parameters:
**    int id              - A graphical object id.
**    int spec            _ A spectrum on which the object is defined.
** Returns:
**   Pointer to the graphical object or NULL if there is no match.
*/
grobj_generic *grobj_database::find(int id)
{
  for(int i = 0; i < obj_count; i++) {
    if(objects[i]->getid() == id) return objects[i];
  }
  return (grobj_generic *)NULL;
}

grobj_generic *grobj_database::find(int id, int spec) 
{
  for(int i = 0; i < obj_count; i++) {
    if( (objects[i]->getid() == id) &&
        (objects[i]->getspectrum() == spec)) 
      return objects[i];
  }
  return (grobj_generic *)NULL;
}

/*
** Method descriptions:
**   find_first  - Finds the first occurence of a particular subset of
**                 graphical objects in the database.  If spec is
**                 present, then the find_next's are confined to
**                 a particular spectrum.
** Formal parameters:
**   int spec  - SPectrum selector.
*/

grobj_generic *grobj_database::find_first()
{
  confined = FALSE;
  cursor   = 0;
  if(obj_count > 0) return objects[0];
  else              return (grobj_generic *) NULL;
}
grobj_generic *grobj_database::find_first(int spec)
{
  confined   = TRUE;
  searchspec = spec;
  for(cursor = 0; cursor < obj_count; cursor++)
    if(objects[cursor]->getspectrum() == spec) 
      return objects[cursor];
  return (grobj_generic *)NULL;
}

/*
** Method Description:
**     find_next      - This method continues a search/traversal begun by
**                      find_first.  If confined is true, then the
**                      next graphical object defined on searchspec is
**                      returned.  Otherwise, the next spectrum is
**                      returned.
*/
grobj_generic *grobj_database::find_next()
{
  for(cursor++; cursor < obj_count; cursor++) {
    if(!confined) return objects[cursor]; /* Return next if not confined. */
    else {
      if(objects[cursor]->getspectrum() == searchspec)
	return objects[cursor];
    }
  }
  return (grobj_generic *)NULL;
}

/*
** Method descriptions:
**    ~grobj_database  - The destructor eliminates all free store allocated
**                       to database entries.
**/
grobj_database::~grobj_database()
{
  int i;
  for(i = 0; i < GROBJ_MAXOBJECTS; i++)
    if(objects[i] != NULL) delete objects[i];
}

/*
** Functional Description:
**   grobj_database::clear:
**     This function clears the object's graphical object data base and
**     re-sets the id's to zero. 
*/
void grobj_database::clear()
{
  for(int i = 0; i < obj_count; i++) {
    delete objects[i];
    objects[i] = NULL;
  }

  /* Reset all the counters and things */

  obj_count = 0;
  lastid    = 0;
  cursor    = 0;
  
  
}
