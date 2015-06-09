/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**   select2.h:
**     This file contains a header for users of the Xamine_Select2 class.
**     The Xamine_Select2 class is a motif custom dialog which prompts for the
**     textual style input for a pair of points.  The work area is a 
**     form which contains typeins, labels and delete buttons sufficient to
**     accept 2 points (the form of data in the point is up to the user).
**     The format of the points is up to the client.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   @(#)select2.h	8.1 6/23/95 
*/

#ifndef _SELECT2_H_INSTALLED
#define _SELECT2_H_INSTALLED

#include "XMManagers.h"
#include "XMLabel.h"
#include "XMText.h"
#include "XMPushbutton.h"

struct Select2_CallbackStruct {
  XMWidget        *thewid;
  void            (*callback)(XMWidget *, XtPointer, XtPointer);
  XtPointer       user_data;
};

// Unbound functions
void  Xamine_Select2CommonCallback(XMWidget *w,
				   XtPointer user_d, 
				   XtPointer call_d);

void Xamine_Select2SetValue(XMTextField *txt, unsigned int val);
void Xamine_Select2SetFloatValue(XMTextField *txt, float val);
void Xamine_Select2SetValue2(XMTextField *txt, 
			     unsigned  x, unsigned  y);
void Xamine_Select2SetFloatValue2(XMTextField *txt, float x, float y);

int Xamine_Select2GetValue(XMTextField *t);
int Xamine_Select2Get2Values(XMTextField *t, int *x, int *y);

// Needed to accept mapped axis input coordinates.

float Xamine_Select2GetFloatValue(XMTextField *t);
void Xamine_Select2Get2FloatValues(XMTextField* t, 
				   float& x, float&y);

// 2 point graphical input class

class Xamine_Select2 {
 protected:
  XMWidget     *pthis;       /* Root of dialog subhierarchy */
  XMLabel      *pt1_label;
  XMTextField  *pt1_value;
  XMPushButton *pt1_delete;
 
  XMLabel      *pt2_label;
  XMTextField  *pt2_value;
  XMPushButton *pt2_delete;

  Select2_CallbackStruct Point1;
  Select2_CallbackStruct Point2;
  Select2_CallbackStruct Delete1;
  Select2_CallbackStruct Delete2;

  
  void Create(XMForm &form);
 public:
  /* Constructors and destructors: */

  Xamine_Select2(XMWidget *root, XMForm &parent) {
    pthis = root;
    Create(parent);
  }
  
  virtual ~Xamine_Select2();

  /* Establish callbacks: */

  void AddPt1Callback(void (*cb)(XMWidget *, XtPointer, XtPointer),
		      XtPointer user_data = NULL) {
    Point1.callback = cb;
    Point1.user_data= user_data;
  }
  void AddPt2Callback(void (*cb)(XMWidget *, XtPointer, XtPointer), 
		      XtPointer user_data = NULL) {
    Point2.callback = cb;
    Point2.user_data= user_data;
  }
  void AddDeletePt1Callback(void (*cb)(XMWidget *, XtPointer, XtPointer), 
			    XtPointer user_data = NULL) {
    Delete1.callback = cb;
    Delete1.user_data= user_data;
  }
  void AddDeletePt2Callback(void (*cb)(XMWidget *, XtPointer, XtPointer), 
			    XtPointer user_data = NULL) {
    Delete2.callback = cb;
    Delete2.user_data= user_data;
  }

  void DeletePt1Callback() {
    AddPt1Callback(NULL);
  }
  void DeletePt2Callback() {
    AddPt2Callback(NULL);
  }
  void DeleteDeletePt1Callback() {
    AddDeletePt1Callback(NULL);
  }
  void DeleteDeletePt2Callback() {
    AddDeletePt2Callback(NULL);
  }
 

  void SetPoint1(unsigned int value) {
    Xamine_Select2SetValue(pt1_value, value);
  }
  void SetPoint1(unsigned int x, unsigned int y) {
    Xamine_Select2SetValue2(pt1_value, x,y);
  }
   
  void SetPoint2(unsigned int value) {
    Xamine_Select2SetValue(pt2_value, value);
  }
  void SetPoint2(unsigned  x, unsigned  y) {
    Xamine_Select2SetValue2(pt2_value, x,y);
  }

  void SetMappedPoint1(float value) {
    Xamine_Select2SetFloatValue(pt1_value, value);
  }
  void SetMappedPoint1(float x, float y) {
    Xamine_Select2SetFloatValue2(pt1_value, x, y);
  }

  void SetMappedPoint2(float value) {
    Xamine_Select2SetFloatValue(pt2_value, value);
  }
  void SetMappedPoint2(float x, float y) {
    Xamine_Select2SetFloatValue2(pt2_value, x, y);
  }


  int GetPoint1() {		/* 1-d getpoint. */
    return Xamine_Select2GetValue(pt1_value);
  }
  int GetPoint1(int *x, int *y) { /* 2-d getpoint. */
    return Xamine_Select2Get2Values(pt1_value, x,y);
  }
  float GetFloatPoint1() {
    return Xamine_Select2GetFloatValue(pt1_value);
  }
  void GetFloatPoint1(float& x, float& y) {
      Xamine_Select2Get2FloatValues(pt1_value, x, y);
  }

  int GetPoint2() {		/* 1-d get point. */
    return Xamine_Select2GetValue(pt2_value);
  }

  int GetPoint2(int *x, int *y) { /* 2-d getpoint */
    return Xamine_Select2Get2Values(pt2_value, x,y);
  }
  float GetFloatPoint2() {
    return Xamine_Select2GetFloatValue(pt2_value);
  }
  void GetFloatPoint2(float& x, float& y) {
    Xamine_Select2Get2FloatValues(pt2_value, x,y);
  }


  void SetPoint1Label(char *text) {
    pt1_label->SetLabel(text);
  }
  void SetPoint2Label(char *text) {
    pt2_label->SetLabel(text);
  }		    
};


#endif
