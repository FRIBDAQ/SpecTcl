/*
** Facility:
**    Xamine  - X-11/Motif support software.
** Abstract:
**    compatspec.h:
**       This file contains definitions for clients of the compatible spectrum
**       list composite widget.  We also export functions to test various
**       compatibilities.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   @(#)compatspec.h	8.1 6/23/95 
*/
#ifndef _COMPATSPEC_H
#define _COMPATSPEC_H
#include <stdlib.h>

#include "XMManagers.h"
#include "XMList.h"
#include "XMPushbutton.h"
#include "XMLabel.h"

class CompatibleSpectrumList : public  XMForm
{
 public:
  CompatibleSpectrumList(char *name, XMWidget *parent, 
			 char *title_string,
			 int spectrum_number);
  virtual ~CompatibleSpectrumList();

  virtual void Clear();
  virtual void Update();
  virtual void NewSpectrum(int newspec);
  void TitleString(char *title);

  void Manage();
  void UnManage();

  void GetSelections(char ***items, int *count);
  void SelectAll();
 protected:
  XMLabel *title;

  XMScrolledList  *speclist;
  XMPushButton    *select_all;
  int     spectrum;

};

/*
** The class below is specialized in that the only spectra shown are those
** which are displayed in the current pane.  Note that all panes are displayed
** even those which match the spectrum which we're compatible with since it's 
** possible that the spectrum is displayed in more than one pane.
** 
*/

class DisplayedCompatibleSpectrumList : public CompatibleSpectrumList
{
 public:

  /* We need these to relay the call to our parent constructors.
  */
  DisplayedCompatibleSpectrumList(char *name,
				XMWidget *parent,
				char *title_string,
				int spectrum_number);
  virtual ~DisplayedCompatibleSpectrumList();

  /* Only a few methodds need to be different. */

  virtual void Update();
  virtual void NewSpectrum(int newspec);
};

/*
** The function below returns true if two spectra are compatible
** (Same dimensionality and same # of channels.
*/

Boolean Xamine_IsCompatible(int spec1, int spec2);

/*
** The function below is useful when destroying string lists:
*/

void Xamine_FreeStrings(char **items, int count);

#endif
