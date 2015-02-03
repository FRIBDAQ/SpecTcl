/*
	Implementation file for ColorScaleDefaultDialog for a description of the
	class see ColorScaleDefaultDialog.h
*/

////////////////////////// FILE_NAME.cpp ///////////////////////////

// Include files required:

#include "ColorScaleDefaultDialog.h"    				
#include "ColorScalePropertySheet.h"
#include "dfltmgr.h"               // We'll manipulate the defaults d.b.
#include "dispwind.h"              // And a default window property.

// Static attribute storage and initialization for ColorScaleDefaultDialog

// Help text:

static const char*[] help_text = {
	"This dialog allows you to set the color scale properties applied\n",
	"to new 2-d spectra.  Color scales can be enabled to appear to the\n",
	"left of the y axis on the spectrum itself.  Doing a Control-MB1\n",
	"in the spectrum also pops up a colorscale dialog for the spectrum\n",
	"that stays up for at most a fixed period of time.  This dialog\n",
	"allows you to determine the defaults for: \n",
	"  - Whether the color scales will be displayed on spectra\n",
	"  - How long a color scale dialog stays up after a Control-MB1\n",
	"\n",
	"o Checking the toggle button at the left of this dialog enables\n",
	"  the display of color scales.\n",
	"o Sliding the scale to the right of this dialog allows you to adjust\n",
	"  the default maximum time color scale widgets popped up with \n",
	"  Control-MB1 remain visible.\n"
	"\n",
	"Clicking the buttons at the bottom of this dialog has the following\n"
	"effect:\n",
	"   Ok           - Updates the defaults and dismisses the dialog\n",
	"   Apply To All - Updates the defaults, and applies them to all\n",
	"                  2-d spectra currently displayed\n",
	"   Cancel       - Makes not changes\n",
	(char*)NULL
};

/*!
    Create an object of type ColorScaleDefaultDialog
    \param 
*/
ColorScaleDefaultDialog::ColorScaleDefaultDialog(char*     pName,
                                                 XMWidget& rParent,
                                                 char*     pTitle) :
    XMCustomDialogBox(pName, rParent, pTitle),
    m_pPropertySheet(0)
{ 
	// Create the property sheet, and put it in the work area:
	
	m_pPropertySheet = new ColorScalePropertySheet(WorkArea());
	LayoutWorkArea();
	
	// Set up the help text:
	
	InitializeHelp(help_text);
	
	// Relabel the Apply button to "Apply to All":

	XMPushButton* pButton = apply();	
	pButton->Label("Apply To All");
	
	// Load the buttons with the default resources:
	
	GetDefaults();
	
} 

/*!
    Called to destroy an instance of ColorScaleDefaultDialog
*/
 ColorScaleDefaultDialog::~ColorScaleDefaultDialog ( )
{
    // Unmanage and destroy the property sheet:
    
    UnManage();
    delete m_pPropertySheet;
    
}

// Functions for class ColorScaleDefaultDialog

/*! 

Description: Layout Work area, the property sheet is placed in the
work area so that all four edges of its top level container are
pasted to the edges of the work are form. 

*/
void 
ColorScaleDefaultDialog::LayoutWorkArea()  
{
	XMForm* pWorkArea = WorkArea();        // My work area form.
	pWorkArea->SetLeftAttachment(m_pPropertySheet,  XmATTACH_FORM); 
	pWorkArea->SetTopAttachment(m_pPropertySheet,   XmATTACH_FORM);
	pWorkArea->SetRightAttachment(m_pPropertySheet, XmATTACH_FORM);
	pWorkArea->SetBottomAttachment(m_pPropertySheet,XmATTACH_FORM);
}  

/*! 

Description:  Make the dialog widgets visible.  This is done by
first managing my base class (outer widgets),  and then my property
sheet (inner widgets).


*/
void 
ColorScaleDefaultDialog::Manage()  
{ 
	XMCustomDialog::Manage();
	m_pPropertySheet->Manage();
}  

/*! 

Description: Called as a result of an apply or ok button press.
  Our job is to get the current parameters from the work area and
  set the defaults.  This is actually already done by SetDefaults
  so our code is pretty uninteresting
  The parameters passed in are uninteresting to us and therefor left
  undocumented.
  
*/
Boolean
ColorScaleDefaultDialog::Perform(XMWidget* pWidget, 
								 XtPointer pClient,
								 XtPointer pEvent)  
{ 
	SetDefaults();
	return true;            // Pop the dialog down (I think).
}  

/*! 
 Un manages (makes invisible) the dialog.  This is done by first
 unmanaging the property sheet (inner widgets), and then the 
 dialog framework (outer widgets):
 
*/
void 
ColorScaleDefaultDialog::UnManage()  
{
	m_pPropertySheet->UnManage();
	XMCustomDialogBox::UnManage(); 
}  


/*! 

Description: Called when the apply to all button is pressed.
	this function will set the defaults (SetDefaults()), and then
	apply them to all of the spectra in the current pane set.
	(Xamine_ApplyDefaultsEverywhere).  We then unmanage the dialog
	(calling OnCancel is good enough for that.
	
	
	Note that none of this destroys the widget!!
	
The parameters are uninteresting to us and therefore not documented.
*/
void 
ColorScaleDefaultDialog::ApplyPressed(XMWidget* pWidget, 
									  XtPointer pClient,
									  XtPointer pEvent)  
{ 
	SetDefaults();                   // Update the defaults.
	Xamine_ApplyDefaultsEveryWhere();
	CancelPressed(pWidget, pClient, pEvent);
}  



/*! 

Reads the widget values and sets the Xamine defaults from them.


*/
void 
ColorScaleDefaultDialog::SetDefaults()  
{
	
	// Retrive the values from the property sheet.
	
	bool fColorOn = m_pPropertySheet->isColorScaleEnabled();
	int  nTimeout = m_pPropertySheet->getTimeout();
	
	Xamine_SetDefaultColorScale(fColorOn);
	Xamine_SetDefaultColorScaleTimeout(nTimeout);
}  


/*! 

Retrieve the colorscale defaults and load them into the dialog property sheet.

*/
void 
ColorScaleDefaultDialog::GetDefaults()  
{
		// Get the current set of 2d defaults.
		
	win_2d Defaults;
	Xamine_Construct2dDefaultProperties(&Defaults);
	
	    // Light or don't light the property sheet's colorscale
	    // button depending on the default state.
	
	if (Defaults.ShowColorScale()) {
		m_pPropertySheet->EnableColorScale();
	} else {
		m_pPropertySheet->DisableColorScale();
	}
	// Set the property sheet's scale position depending on the 
	// default information.
	
	m_pPropertySheet->SetPopupTimeout(Defaults.ColorPopupTimeout());

}  

