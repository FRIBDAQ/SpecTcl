/*
	Implementation file for CLinearFitCreator for a description of the
	class see CLinearFitCreator.h
*/

////////////////////////// FILE_NAME.cpp ///////////////////////////

// Include files required:

#include <config.h>
#include "CLinearFitCreator.h"    				
#include "CLinearFit.h"


#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


/*!
    Create an object of type CLinearFitCreator
*/
CLinearFitCreator::CLinearFitCreator () 
{ 
} 

/*!
    Called to destroy an instance of CLinearFitCreator
*/
 CLinearFitCreator::~CLinearFitCreator ( )
{
}
/*!
   Called to create an instance of CLinearFitCreator that is a
   functional duplicate of another instance.
   \param rSource (const CLinearFitCreator& ):
      The object that we will dupliate.
*/
CLinearFitCreator::CLinearFitCreator (const CLinearFitCreator& aCLinearFitCreator ) 
  : CFitCreator (aCLinearFitCreator) 
{
} 
/*!
  Assign to *this from rhs so that *this becomes a functional
  duplicate of rhs.
  \param rhs (const CLinearFitCreator& rhs ):
     The object that will be functionally copied to *this.
 */
CLinearFitCreator& 
CLinearFitCreator::operator= (const CLinearFitCreator& rhs)
{ 
  if(this != &rhs) {
    CFitCreator::operator=(rhs);
  }
  return *this;

}
/*!
  Compare *this for functional equality with another object of
  type CLinearFitCreator.
  \param rhs (const CLinearFitCreator& rhs ):
     The object to be compared with *this.

 */
int 
CLinearFitCreator::operator== (const CLinearFitCreator& rhs) const
{
  return CFitCreator::operator==(rhs);
}
/*!
   Compare *this for functional inequality with another object
   of type CLinearFitCreator.  Functional inequality is defined as
   the boolean inverse of functional equality.
   \param rhs (const CLinearFitCreator& rhs ):
      The object to compare with *this.
*/
int
CLinearFitCreator::operator!= (const CLinearFitCreator& rhs) const
{
  return !(*this == rhs);
}

// Functions for class CLinearFitCreator

/*! 

Description:

Creates and returns a newly constructeed CLinearFit
object in the AccumulatingPoints state.



Parameters:

\return CFit*
\retval 0 - Object could not be created for whatever reason.
\retval !=0 Pointer to the newly created fit object.


*/
CFit* 
CLinearFitCreator::operator()(std::string name, int id)  
{ 
  return new CLinearFit(name, id);
}
/*!
   Return a description of the type of fit we create:
*/
string
CLinearFitCreator::DescribeFit()
{
  return string("Linear (straight line) fit");
}
