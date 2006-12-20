///////////////////////////////////////////////////////////
//  CTreeParameter.h
//  Implementation of the Class CTreeParameter
//  Created on:      30-Mar-2005 11:03:49 AM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
             NSCL
             Michigan State University
             East Lansing, MI 48824-1321
*/
// This implementation of TreeParameter is based on the ideas and original code of::
//    Daniel Bazin
//    National Superconducting Cyclotron Lab
//    Michigan State University
//    East Lansing, MI 48824-1321
//

#if !defined(__CTREEPARAMETER_H)
#define __CTREEPARAMETER_H

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif


#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_MAP
#include <map>
#ifndef __STL_MAP
#define __STL_MAP
#endif
#endif


// Forward definitions:

class CParameter;
class CEvent;

/**
 * The treeparameter is built to pretend to be a double member of a structure with
 * the following additional attributes:
 * - A name which is supposed to reflect it's structural element.
 * - A low/hi/channel count which are used as axis specifications for default
 * spectrum generation.
 * - A linkage to a parameter (done after construction).
 * - Linkage to an element of a CEvent (done for each event).
 * 
 * To use CTreeParameter requires (for g++) the -fno-elide-constructors flag,
 * otherwise registration is screwed up.
 * Each tree parameter object is registered at construction time in a multimap
 * that is keyed by parameter name and contains as a value a pointer to the object.
 *  A static member supports traversing this multimap, binding objects to
 * parameters.
 * 
 * Static member data includes a pointer to the current rEvent.  This will be
 * filled in by an event processor that is related to the treeparameter.
 * @author Ron Fox
 * @version 1.3
 * @created 30-Mar-2005 11:03:49 AM
 */
class CTreeParameter
{
private:
  /**
   * Name of the  parameter that will be bound to this object.
   */
  STD(string) m_sName;
  /**
   * Units of the parameter.
   */
  STD(string) m_sUnits;
  /**
   * Default low parameter limit on spectrum axes.
   */
  double m_dLowLimit;
  /**
   * Default high parameter limit on axes of spectra created by gui.
   */
  double m_dHighLimit;
  /**
   * Default channels on axes of spectra created by GUI.
   */
  UInt_t m_nChannels;
  /**
   * True if the parameter definition has changed since construction.
   */
  bool m_fDefinitionChanged;
  /**
   * Associated parameter.  This parameter is bound post construction and is a
   * parameter with the same name as the treeparameter object.
   */	
  CParameter *m_pParameter;
  
  /**
   * Map of tree parameter objects that currently exist. Object register on
   * construction and unregister on destruction.
   */
  static STD(multimap)<STD(string), CTreeParameter*> m_ObjectRegistry;
  /**
   * The event that we are unpacking into.
   * The parameter element indicates which element of the Event we will fill in.
   */
  static CEvent *m_pEvent;
  
public:
  /**
   * Version of the software.
   */
  static const STD(string) TreeParameterVersion;
  /**
   *  Set this true for strict units type checking where possible.
   */
  static bool StrictTypeChecking;
  
  // Constructors
  
  CTreeParameter();
  CTreeParameter(STD(string) name);
  CTreeParameter(STD(string) name, STD(string) units);
  CTreeParameter(STD(string) name, double lowLimit, double highLimit, 
		 STD(string) units);
  CTreeParameter(STD(string) name, UInt_t channels, 
		 double lowLimit, double highLimit, STD(string) units);
  CTreeParameter(STD(string) name, UInt_t resolution);
  CTreeParameter(STD(string) name, UInt_t resolution, 
		 double lowLimit, double widthOrHigh, 
		 STD(string) units, bool widthOrHighGiven);
  CTreeParameter(STD(string) name, const CTreeParameter& Template);
  CTreeParameter(const CTreeParameter& rhs);
  ~CTreeParameter();
  
  // Initializers that do the real work of construction.
  
  void Initialize(STD(string) name, UInt_t resolution);
  void Initialize(STD(string) name, UInt_t resolution, 
		  double lowLimit, double highOrWidth, STD(string) units, 
		  bool highOrWidthGiven);
  void Initialize(STD(string) name);
  void Initialize(STD(string) name, STD(string) units);
  void Initialize(STD(string) name, UInt_t channels, 
		  double lowLimit, double highLimit, STD(string) units);
  
  // Other member functions.
  
  static void BindParameters();
  static void setEvent(CEvent& rEvent);
  bool isBound();
  
	// Arithmetic operations...
  
  operator double();
  CTreeParameter& operator= (double newValue);
  CTreeParameter& operator= (CTreeParameter& rhs);
  CTreeParameter& operator+=(double rhs);
  CTreeParameter& operator-=(double rhs);
  CTreeParameter& operator*=(double rhs);
  CTreeParameter& operator/=(double rhs);
  double          operator++(int dummy);
  CTreeParameter& operator++();
  double          operator--(int dummy);
  CTreeParameter& operator--();
  
  // Selectors and mutators.
  
  STD(string) getName();
  int    getId();
  double getValue();
  void   setValue(double newValue);
  UInt_t getBins();
  void   setBins(UInt_t channels);
  double getStart();
  void   setStart(double low);
  double getStop();
  void   setStop(double high);
  double getInc();
  void   setInc(double channelWidth);
  STD(string) getUnit();
  void   setUnit(STD(string) units);
  bool   isValid();
  void   setInvalid();
  void   Reset();
  void   clear();
  bool   hasChanged();
  void   setChanged();
  void   resetChanged();
  static void ResetAll();
  
  // Iteration and location.
  
  static STD(multimap)<STD(string), CTreeParameter*>::iterator begin();
  static STD(multimap)<STD(string), CTreeParameter*>::iterator end();
  static STD(multimap)<STD(string), CTreeParameter*>::iterator find(STD(string) name);
  void Bind();
  
  
  // Internal utilities.
  
protected:
  STD(multimap)<STD(string), CTreeParameter*>::iterator findRegistration();
  bool isRegistered();
  void Register();
  void Unregister();
  
  // Factor out some exception generating logic:
  
  void ThrowIfNoParameter(STD(string)& doing);
  void ThrowIfNoEvent(STD(string)& doing);
  
  // The functions below are inteded to support testability:
  
private:
  friend class TreeTestSupport;

  // The intent is that TreeParamTests will only make use of members below this line:
  // These functions allow the restoration of the world of tree parameters back to 'pristine'
  // form (form at program startup).

  static void testClearMap();
public:
  static void dumpmap();
};




#endif
