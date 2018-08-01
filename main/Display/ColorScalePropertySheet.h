// Author:
//   Ron Fox
//   NSCL
//   Michigan State University
//   East Lansing, MI 48824-1321
//   mailto:fox@nscl.msu.edu
//
// Copyright 
//! \class: ColorScalePropertySheet           
//! \file:  .h
/*!
  \class ColorScalePropertySheet
  \file  .h

*/



#ifndef COLORSCALEPROPERTYSHEET_H  //Required for current class
#define COLORSCALEPROPERTYSHEET_H

//
// Include files:
//


// Forward Definitions.

class XMWidget;
class XMForm;
class XMToggleButton;
class XMScale;


//  The class interface: 

class ColorScalePropertySheet      
{
private:
  
  // Private Member data:
	XMForm*          m_pContainer;  //!< Form that holds the components.
    XMToggleButton*  m_pCheckBox;   //!< Toggle button for display colorscale
    XMScale*         m_pScale;      //!< Scale with timeout.

public:
    //  Constructors and other canonical operations.

    ColorScalePropertySheet (XMWidget* pParent);		//!< Constructor.
    virtual  ~ ColorScalePropertySheet ( ); //!< Destructor.
    
    
    // This compound widget won't allow copy like operations and therefore
    // comparisons also don't mean much.
private:
    ColorScalePropertySheet (const ColorScalePropertySheet& rSource ); //!< Copy construction.
    ColorScalePropertySheet& operator= (const ColorScalePropertySheet& rhs); //!< Assignment.
    int operator== (const ColorScalePropertySheet& rhs) const; //!< == comparison.
    int operator!= (const ColorScalePropertySheet& rhs) const; //!< != comparison.
public:

	// Selectors:
	
    XMForm*  getContainer ()           //!< Retrieve top level widget.  
    {
    	return m_pContainer;
    }

	// Class operations:

public:

    void EnableColorScale ()    ;    //!< Programmatically set the toggle. 
    void DisableColorScale ()   ;    //!< Programmatically clear the toggle.
    void SetPopupTimeout (unsignedint nTimeout)     ;    //!< Set position of scale.
    void isColorScaleEnabled () ;    //!< Get the state of the toggle.
    int  getPopupTimeout ()     ;    //!< Get the popup timeout from the scale.
    void Manage()               ;    //!< Activate geometry management.
    void UnManage()             ;    //!< Deactivate geometry management.

	// Private utility functions.

private:
	// The functions below are used at construction:
	
	void CreateWidgets(XMWidget* pParent);  //!< Create the widget subhierarchy.
	void LayoutWidgets();                   //!< Attach widgets to form.
};

#endif
