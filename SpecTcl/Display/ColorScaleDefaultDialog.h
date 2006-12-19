// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
//
/*!
  \class ColorScaleDefaultDialog
  \file  .h

*/



#ifndef __COLORSCALEDEFAULTDIALOG_H  //Required for current class
#define __COLORSCALEDEFAULTDIALOG_H

//
// Include files:
//

                               //Required for base classes
#ifndef __XMDIALOGS_H     //XMCustomDialogBox
#include "XMCustomDialogBox.h"
#ifndef __XMDIALOGS_H
#define __XMDIALOGS_H
#endif
#endif
 
                               //Required for 1:1 association classes

// Forward class definitions (convert to includes if required):

class ColorScalePropertySheet;

class ColorScaleDefaultDialog  : public XMCustomDialogBox        
{
private:
  
     ColorScalePropertySheet* m_pPropertySheet; //!< Work area contents.

    //  Constructors and other canonical operations.
    //  You may need to adjust the parameters
    //  and the visibility esp. if you cannot
    // implement assignment/copy construction.
    // safely.

    ColorScaleDefaultDialog (char*     pName,
    						 XMWidget& rParent,
    						 char*     pTitle);		//!< Constructor.
    virtual  ~ ColorScaleDefaultDialog ( ); //!< Destructor.
    
    // Copy like operations are not allowed.  Therefore
    // copy constructors are private and, since therefore comparisons
    // can't make sense,they are forbiddent too:
    // The functions below are not implemented in order to ensure
    // they don't get called.
    
 private:
    ColorScaleDefaultDialog (const ColorScaleDefaultDialog& rSource ); //!< Copy construction.
    ColorScaleDefaultDialog& operator= (const ColorScaleDefaultDialog& rhs); //!< Assignment.
    int operator== (const ColorScaleDefaultDialog& rhs) const; //!< == comparison.
    int operator!= (const ColorScaleDefaultDialog& rhs) const; //!< != comparison.

// Class operations:

public:

    void Manage ()   ; 
    void UnManage ()   ; 
    void GetDefaults ()   ; 
    void SetDefaults ()   ; 

    virtual Boolean Perform (XMWidget* pWidget,
    					     XtPointer pClient,
    					     XtPointer pEvent)   ; 
    virtual void ApplyPressed (XMWidget* pWidget,
                               XtPointer pClient,
                               XtPointer pEvent)   ; 


private:
	void LayoutWorkArea ()   ; 

};

#endif
