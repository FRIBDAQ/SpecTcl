/*
	Implementation file for CFitCreator for a description of the
	class see CFitCreator.h
*/

////////////////////////// FILE_NAME.cpp ///////////////////////////

// Include files required:

#include "CFitCreator.h"    				


/*!
    Create an object of type CFitCreator
*/
CFitCreator::CFitCreator ()
 
{ 
} 

/*!
    Called to destroy an instance of CFitCreator
*/
 CFitCreator::~CFitCreator ( )
{
}
/*!
   Called to create an instance of CFitCreator that is a
   functional duplicate of another instance.
   \param rSource (const CFitCreator& ):
      The object that we will dupliate.
*/
CFitCreator::CFitCreator (const CFitCreator& aCFitCreator ) 
{
} 
/*!
  Assign to *this from rhs so that *this becomes a functional
  duplicate of rhs.
  \param rhs (const CFitCreator& rhs ):
     The object that will be functionally copied to *this.
 */
CFitCreator& CFitCreator::operator= (const CFitCreator& rhs)
{ 

  return *this;
}
/*!
  Compare *this for functional equality with another object of
  type CFitCreator.
  \param rhs (const CFitCreator& rhs ):
     The object to be compared with *this.

 */
int 
CFitCreator::operator== (const CFitCreator& rhs) const
{
  return true;
}
/*!
   Compare *this for functional inequality with another object
   of type CFitCreator.  Functional inequality is defined as
   the boolean inverse of functional equality.
   \param rhs (const CFitCreator& rhs ):
      The object to compare with *this.
*/
int
CFitCreator::operator!= (const CFitCreator& rhs) const
{
  return !(*this == rhs);
}


