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


#ifndef CTREEPARAMETER_H
#define CTREEPARAMETER_H

#include <histotypes.h>
#include <string>
#include <map>


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
  std::string m_sName;
  /**
   * Units of the parameter.
   */
  std::string m_sUnits;
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
  static std::multimap<std::string, CTreeParameter*> m_ObjectRegistry;
  /**
   * The event that we are unpacking into.
   * The parameter element indicates which element of the Event we will fill in.
   */
  static CEvent *m_pEvent;
  
  bool m_initialized;
  
public:
  /**
   * Version of the software.
   */
  static const std::string TreeParameterVersion;
  /**
   *  Set this true for strict units type checking where possible.
   */
  static bool StrictTypeChecking;
  
  // Constructors
  
  CTreeParameter();
  CTreeParameter(std::string name);
  CTreeParameter(std::string name, std::string units);
  CTreeParameter(std::string name, double lowLimit, double highLimit, 
		 std::string units);
  CTreeParameter(std::string name, UInt_t channels, 
		 double lowLimit, double highLimit, std::string units);
  CTreeParameter(std::string name, UInt_t resolution);
  CTreeParameter(std::string name, UInt_t resolution, 
		 double lowLimit, double widthOrHigh, 
		 std::string units, bool widthOrHighGiven);
  CTreeParameter(std::string name, const CTreeParameter& Template);
  CTreeParameter(const CTreeParameter& rhs);
  ~CTreeParameter();
  
  // Initializers that do the real work of construction.
  
  void Initialize(std::string name, UInt_t resolution);
  void Initialize(std::string name, UInt_t resolution, 
		  double lowLimit, double highOrWidth, std::string units, 
		  bool highOrWidthGiven);
  void Initialize(std::string name);
  void Initialize(std::string name, std::string units);
  void Initialize(std::string name, UInt_t channels, 
		  double lowLimit, double highLimit, std::string units);
  
 
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
  
  std::string getName();
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
  std::string getUnit();
  void   setUnit(std::string units);
  bool   isValid();
  void   setInvalid();
  void   Reset();
  void   clear();
  bool   hasChanged();
  void   setChanged();
  void   resetChanged();
  static void ResetAll();
  

 // Other member functions.
  
  static void BindParameters();
  static void setEvent(CEvent& rEvent);
  bool isBound() const;
  
  // Iteration and location.
  
  static std::multimap<std::string, CTreeParameter*>::iterator begin();
  static std::multimap<std::string, CTreeParameter*>::iterator end();
  static std::multimap<std::string, CTreeParameter*>::iterator find(std::string name);
  void Bind();
  
  
  // Internal utilities.
  
protected:
  std::multimap<std::string, CTreeParameter*>::iterator findRegistration();
  bool isRegistered();
  void Register();
  void Unregister();
  
  // Factor out some exception generating logic:
  
  void ThrowIfNoParameter(std::string& doing);
  void ThrowIfNoEvent(std::string& doing);
  
  // The functions below are inteded to support testability:
  
private:
  friend class TreeTestSupport;

  // The intent is that TreeParamTests will only make use of members below this line:
  // These functions allow the restoration of the world of tree parameters back to 'pristine'
  // form (form at program startup).

  static void testClearMap();
  void throwIfDoubleInit(const char* newName);
public:
  static void dumpmap();
};




#endif
