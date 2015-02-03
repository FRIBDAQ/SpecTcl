

/*
	Implementation file for ColorScalePropertySheet for a description of the
	class see ColorScalePropertySheet.h
*/

////////////////////////// FILE_NAME.cpp ///////////////////////////

// Include files required:

#include "ColorScalePropertySheet.h"    	
#include <XMManagers.h>
#include <XMPushbutton.h>
#include <XMScale.h>
			

// Static attribute storage and initialization for ColorScalePropertySheet

statc const int MAXTIMEOUT(60);          // A minute is a long time!!

/*!
    Create an object of type ColorScalePropertySheet
    \param pParent (XMWidget*):
       The parent widget of this property sheet.  Usually this is
     a container widget of some sort such as a form e.g.
*/
ColorScalePropertySheet::ColorScalePropertySheet (XMWidget* pParent) :
   m_pContainer(0),
   m_pCheckBox(0),
   m_pScale(0)
{
	CreateWidgets(pParent);
	LayoutWidgets();
} 

/*!
    Called to destroy an instance of ColorScalePropertySheet
*/
 ColorScalePropertySheet::~ColorScalePropertySheet ( )
{
	UnManage();                       // Only delete invisible widgets.
	delete m_pCheckBox;               // Do the inner widgets first:
	delete m_pScale;
	delete m_pContainer;              // Finally the outer container.
}


// Functions for class ColorScalePropertySheet

/*! 

Description: Return the container widget. In our case, this is
m_pContainer, the form in which al the other widgets are laid out.
This function is provided so that the outer form of the property sheet
can itself be laid out within some other external container widget that
it does not need to know about.


*/
void 
ColorScalePropertySheet::getContainer()  
{
	return m_pContainer; 
}  

/*! 

Description: Set the checkbox state to on.  This checkbox, represnents
the current system knowledge of the state of the enable color scale
property.

*/
void 
ColorScalePropertySheet::EnableColorScale()  
{
	m_pCheckBox->SetState(true);
}  

/*! 

Description: sets the checkbox state to off.  This checkbox, represnts the
current system knowlede of the state of the enable color scale property,
so switching it off is the same as turning off that property.
*/
void 
ColorScalePropertySheet::DisableColorScale()  
{ 
	m_pCheckBox->SetState(false);
}  

/*! 

Set the popup timeout.  This function sets the scale to a selected
value.  This effectively sets the timeout value as known by the property
sheet. Values > MAXTIMEOUT are pegged to MAXTIMEOUT

\param nTimeout (unsigned int)
   The new timeout value (seconds).
*/
void 
ColorScalePropertySheet::SetPopupTimeout(unsigned int nTimeout)  
{ 
	if (nTimeout > MAXTIMEOUT) {
		nTimeout = MAXTIMEOUT;
	}
	m_pScale->Value(nTimeout);
}  

/*! 

Description: Determines if the checkbox is currently checked. The
checkbox corresponds to the value of the colorscale enabled property.


\return bool
	\retval true - The colorscale enable checkbox is on.
	\retval false- The colorscale enable checkbox is off.


*/
void 
ColorScalePropertySheet::isColorScaleEnabled()  
{
	 return m_pCheckBox->GetState();
}  

/*! 

Description: Return the value of the color scale dialog pop up scale.
This scale represents the timeout property for the colorscale pop up
dialog.

*/
void 
ColorScalePropertySheet::getPopupTimeout()  
{ 
	return m_pScale->Value();
}
/*!
   Manage the property sheet.  This is generally called by the outer
   layers of software to make the property sheet visible in a container
   widget.
 */
 void 
 ColorScalePropertySheet::Manage()
 {
 	//Widgets should be managed bottom to top:
 	
 	m_pCheckBox->Manage();
 	m_pScale->Manage();
 	m_pContainer->Manage();
 }
 /*!
    Unmanage the property sheet.  This is generally called by the outer
    layers of the software to make the property sheet invisible (e.g. when 
    withdrawing a cached widget that contains us).
 */
 void
 ColorScalePropertySheet::UnManage()
{
	// My recollection is that it's good to manage from out to in:
	
	m_pContainer->UnManage();
	m_pCheckBox->UnManage();
	m_pContainer->UnManage();
}
/*!
   Creates the set of widgets that make up the property sheet.  This
   is called by the constructor and is a utility function to help
   reduce code complexity.
   \param pParent (XMWidget*):
      The parent widget for the property sheet.
*/
void 
ColorScalePropertySheet::CreateWidgets(XMWidget* pParent)
{
	// Form is a child of pParent and the other two
	// two widgets are children of the form:
	
	m_pContainer   = new XMForm("ColorScalePropertySheet",
					     		*pParent);
	m_pCheckButton = new XMToggleButton("CSPSToggle",
										*m_pContainer);
	m_pScale       = new XMScale("CSPSScale",
								 *m_pContainer);
    m_pScale->SetRange(MAXTIMEOUT);
    
}
/*!
   Lays out the widgets (must have already been created).  This is
   called by the constructor to layout the geometry of the elements of
   the property list widgets in the containing form.
*/
void
ColorScalePropertySheet::LayoutWidgets()
{
	// The toggle button will be attached to the form at the left
	// top and bottom edges, but allowed to roam free on the right
	// edge.:
	
	m_pContainer->SetLeftAttachment(*m_pCheckBox,   XmATTACH_FORM);
	m_pContainer->SetTopAttachment(*m_pCheckBox,    XmATTACH_FORM);
	m_pContainer->SetBottomAttachment(*m_pCheckBox, XmATTACH_FORM);
	m_pContainer->SetRightAttachment(*m_pCheckBox,  XmATTACH_NONE);
	                                 
	// The scale will be attached on the left to the checkbox widget,
	// on the right, top and bottom to the form.  I think/hope, this
	// set of constraints will allow the scale to expand if the outer
	// form is stretched horizontally, and keep the widgets about centered
	// vertically in the extent of the form:
	
	m_pContainer->SetLeftAttachment(*m_pScale,   XmATTACH_WIDGET);
    m_pContainer->SetLeftWidget(*m_pScale,       *m_pChecBox);
    m_pContainer->SetTopAttchment(*m_pScale,     XmATTACH_FORM);
    m_pContainer->SetRightAttachment(*m_pScale,  XmATTACH_FORM);
    m_pContainer->SetBottomAttachment(*m_pScale, XmATTACH_FORM);
    
}