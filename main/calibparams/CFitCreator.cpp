/*
	Implementation file for CFitCreator for a description of the
	class see CFitCreator.h
*/

////////////////////////// FILE_NAME.cpp ///////////////////////////

// Include files required:

#include <config.h>
#include "./CFitCreator.h"    				

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/*!
    Create an object of type CCalibFitCreator
*/
CCalibFitCreator::CCalibFitCreator ()
 
{ 
} 

/*!
    Called to destroy an instance of CCalibFitCreator
*/
 CCalibFitCreator::~CCalibFitCreator ( )
{
}
/*!
   Called to create an instance of CCalibFitCreator that is a
   functional duplicate of another instance.
   \param rSource (const CCalibFitCreator& ):
      The object that we will dupliate.
*/
CCalibFitCreator::CCalibFitCreator (const CCalibFitCreator& aCCalibFitCreator ) 
{
} 
/*!
  Assign to *this from rhs so that *this becomes a functional
  duplicate of rhs.
  \param rhs (const CCalibFitCreator& rhs ):
     The object that will be functionally copied to *this.
 */
CCalibFitCreator& CCalibFitCreator::operator= (const CCalibFitCreator& rhs)
{ 

  return *this;
}
/*!
  Compare *this for functional equality with another object of
  type CCalibFitCreator.
  \param rhs (const CCalibFitCreator& rhs ):
     The object to be compared with *this.

 */
int 
CCalibFitCreator::operator== (const CCalibFitCreator& rhs) const
{
  return true;
}
/*!
   Compare *this for functional inequality with another object
   of type CCalibFitCreator.  Functional inequality is defined as
   the boolean inverse of functional equality.
   \param rhs (const CCalibFitCreator& rhs ):
      The object to compare with *this.
*/
int
CCalibFitCreator::operator!= (const CCalibFitCreator& rhs) const
{
  return !(*this == rhs);
}


