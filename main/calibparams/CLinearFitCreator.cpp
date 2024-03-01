/*
	Implementation file for CLinearFitCreator for a description of the
	class see CLinearFitCreator.h
*/

////////////////////////// FILE_NAME.cpp ///////////////////////////

// Include files required:

#include <config.h>
#include "./CLinearFitCreator.h"    				
#include "./CLinearFit.h"


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


/*!
    Create an object of type CCalibLinearFitCreator
*/
CCalibLinearFitCreator::CCalibLinearFitCreator () 
{ 
} 

/*!
    Called to destroy an instance of CCalibLinearFitCreator
*/
 CCalibLinearFitCreator::~CCalibLinearFitCreator ( )
{
}
/*!
   Called to create an instance of CCalibLinearFitCreator that is a
   functional duplicate of another instance.
   \param rSource (const CCalibLinearFitCreator& ):
      The object that we will dupliate.
*/
CCalibLinearFitCreator::CCalibLinearFitCreator (const CCalibLinearFitCreator& aCCalibLinearFitCreator ) 
  : CCalibFitCreator (aCCalibLinearFitCreator) 
{
} 
/*!
  Assign to *this from rhs so that *this becomes a functional
  duplicate of rhs.
  \param rhs (const CCalibLinearFitCreator& rhs ):
     The object that will be functionally copied to *this.
 */
CCalibLinearFitCreator& 
CCalibLinearFitCreator::operator= (const CCalibLinearFitCreator& rhs)
{ 
  if(this != &rhs) {
    CCalibFitCreator::operator=(rhs);
  }
  return *this;

}
/*!
  Compare *this for functional equality with another object of
  type CCalibLinearFitCreator.
  \param rhs (const CCalibLinearFitCreator& rhs ):
     The object to be compared with *this.

 */
int 
CCalibLinearFitCreator::operator== (const CCalibLinearFitCreator& rhs) const
{
  return CCalibFitCreator::operator==(rhs);
}
/*!
   Compare *this for functional inequality with another object
   of type CCalibLinearFitCreator.  Functional inequality is defined as
   the boolean inverse of functional equality.
   \param rhs (const CCalibLinearFitCreator& rhs ):
      The object to compare with *this.
*/
int
CCalibLinearFitCreator::operator!= (const CCalibLinearFitCreator& rhs) const
{
  return !(*this == rhs);
}

// Functions for class CCalibLinearFitCreator

/*! 

Description:

Creates and returns a newly constructeed CCalibLinearFit
object in the AccumulatingPoints state.



Parameters:

\return CCalibFit*
\retval 0 - Object could not be created for whatever reason.
\retval !=0 Pointer to the newly created fit object.


*/
CCalibFit* 
CCalibLinearFitCreator::operator()()  
{ 
  return new CCalibLinearFit;
}
/*!
   Return a description of the type of fit we create:
*/
string
CCalibLinearFitCreator::DescribeFit()
{
  return string("Linear (straight line) fit");
}
