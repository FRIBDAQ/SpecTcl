//////////////////////////////////////////////////////////
//  CTreeParameter.cpp
//  Implementation of the Class CTreeParameter
//  Created on:      30-Mar-2005 11:03:50 AM
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
#include <config.h>
#include  <Event.h>

#include "CTreeParameter.h"
#include "CTreeException.h"
#include "MatchTreeParameter.h"
#include "BindVisitor.h"
#include "ResetVisitor.h"

#include  <Parameter.h>
#include  <SpecTcl.h>
#include <Iostream.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif



// Static member data:

const string CTreeParameter::TreeParameterVersion("2.1");
bool  CTreeParameter::StrictTypeChecking(false);
CEvent* CTreeParameter::m_pEvent(0);

multimap<string, CTreeParameter*> CTreeParameter::m_ObjectRegistry;

/**
 * Default constructor.  The parameter has no name and is therefore
 * not entered to be subsequently bound.   A later call to one of the
 * Initialize()
 * members will be required to get this parameter entered for binding.
 * 
 */
CTreeParameter::CTreeParameter() :
  m_dLowLimit(1.0),
  m_dHighLimit(100.0),          // have to choose something.
  m_nChannels(100),
  m_fDefinitionChanged(false),
  m_pParameter(0)
{

}


/**
 * Constructs a tree parameter with only a name.  Some simple values for the
 * axis defaults are selected.  Since only a name is required to register an
 * object, this is registered for later binding.
 * @param name    
 *    Name of the parameter to create.
 * 
 */
CTreeParameter::CTreeParameter(string name) :
  m_pParameter(0)
{
  Initialize(name);

}


/**
 * Constructs a tree parameter that has units as well as a name.  Suitable
 * defaults are chosen for the axis range.  The parameter is  registered for 
 * later binding as well.
 * @param name    Name of the parameter.
 * @param units    Units of measure of the parameter.
 * 
 */
CTreeParameter::CTreeParameter(string name, string units) :
  m_pParameter(0)
{
  Initialize(name, units);
}


/**
 * Constructs a tree parameter that has a name, units and default range.  A
 * suitable value is chosen for the default number of bins.  The parameter is
 * registered for subsequent binding.
 * 
 * @param name    
 *        Name of the new parameter.
 * @param lowLimit
 *        Default low limit for the parameter range on a spectrum axis
 *        created by the GUI.
 * @param highLimit
 *        Default high limit on range of parameter on axes created by
 *        the GUI.
 * @param units
 *        Units of measure of the parameter.
 * 
 */
CTreeParameter::CTreeParameter(string name, 
			       double lowLimit, double highLimit, 
			       string units) :
  m_pParameter(0)
{
  Initialize(name, 100, lowLimit, highLimit, units);
}


/**
 * Constructor that creates a parameter with  name, units, range and bin count.
 * @param name
 *        Name of the parameter to create.
 * @param channels
 *        Default number of channels for axes on this parameter
 *        created by the GUI.
 * @param lowLimit
 *        defaut low limit for axes on this parameter created by the GUI.
 * @param highLimit
 *        Upper limit value on axes created on this parameter by the GUI.
 * @param units
 *        Units of measure of the parameter.
 * 
 */
CTreeParameter::CTreeParameter(string name, UInt_t channels, 
			       double lowLimit, double highLimit, 
			       string units) :
  m_pParameter(0)
{

  Initialize(name, channels, lowLimit, highLimit, units);


}


/**
 * Constructor for SpecTcl < 2.0 parameters.  These parameters are specified with
 * a resolution in bits, and are assumed to be integers.
 * @param name
 *        Name of the parameter.
 * @param resolution
 *        Number of bits of resolution the parameter has.
 * 
 */
CTreeParameter::CTreeParameter(string name, UInt_t resolution) :
  m_pParameter(0)
{
  Initialize(name, resolution);
}


/**
 * SpecTcl < 2.0 parameter which have mapping information... the mapping is now
 * done via real parameters.
 * @param name
 *        Name of the parameter to create.
 * @param resolution
 *        Number of bits of resolution the parameter has.
 * @param lowLimit
 *        Low limit of axes on this parameter created by the GUI.
 * @param widthOrHigh
 *        Either the high limit of the parameter or the width of
 *        each channel, depending on the value of the widthOrHighGiven flag.
 * @param units
 *        Parameter units.
 * @param widthOrHighGiven
 *       If:
 *        - false the widthOrHigh parameter is the parameter high limit.
 *        - true  the widthOrHigh parameter is the width of a channel and lowLimit
 *                 together with resolution is used to calculate highLImit.
 * 
 */
CTreeParameter::CTreeParameter(string name, UInt_t resolution, 
			       double lowLimit, double widthOrHigh, 
			       string units, bool widthOrHighGiven) :
  m_pParameter(0)
{

  Initialize(name, resolution, lowLimit, widthOrHigh, units, widthOrHighGiven);
}


/**
 * Constructor that creates a tree parameter from a template, only providing a new
 * name for the parameter.  This is intended to be used for the bulk creation
 * required by the CTreeParameterArray constructors but can, of course be used by
 * the user as well.  Parameters are a const reference to the template and a name
 * for the new parameter.
 * @param name
 *        Name of the parameter being created.
 * @param Template
 *        Template from which to construct this.
 * 
 */
CTreeParameter::CTreeParameter(string name, const CTreeParameter& Template) :
  m_pParameter(0)
{

  Initialize(name, Template.m_nChannels, Template.m_dLowLimit, 
	     Template.m_dHighLimit, Template.m_sUnits);


}


/**
 * Copy construction.  The resulting parameter is a duplicate of the rhs parameter.
 * The parameter is registered in case it needs to be bound later.
 * @param rhs
 *        The CTreeParameter object that is being copied to construct us.
 * 
 */
CTreeParameter::CTreeParameter(const CTreeParameter& rhs) :
  m_pParameter(0)
{
  Initialize(rhs.m_sName, rhs.m_nChannels, rhs.m_dLowLimit, rhs.m_dHighLimit,
	     rhs.m_sUnits);
}


/**
 * Destroys this tree parameter.  If this is registered it is unregistered.
 */
CTreeParameter::~CTreeParameter()
{
  Unregister();
}


/**
 * Returns an iterator to the registration of ths tree parameter.  This is end()
 * if the object is not registered.

   \return multimap<string, CTreeParameter*>::iterator
   \retval CTreeParameter::end()  - if we are not registered.
   \retval If we are registered, the iterator that points to our multimap
           entry (e.g. returnvalue.second  == this).
 */
multimap<string, CTreeParameter*>::iterator 
CTreeParameter::findRegistration()
{
  MatchTreeParameter visitor(this);
  return find_if(begin(), end(), visitor);

}


/**
 * Returns true if this object is registered.
   \return bool
   \retval true  - this is  registered.
   \retval false - this is not registered.

 */
bool 
CTreeParameter::isRegistered()
{
  return (findRegistration() != end());
}


/**
 * Register this CTreeParameter in the map.  Note that if this tree parameter is
 * already reigstered,  this is a no-op.
 */
void 
CTreeParameter::Register()
{


  if (!isRegistered()) {
    pair<string, CTreeParameter*> entry(m_sName, this);
    m_ObjectRegistry.insert(entry); 
  }
  
}


/**
 * Unregister the tree parameter from the map.  If the object is not registered,
 * this is a  no-op.
 */
void 
CTreeParameter::Unregister()
{

  multimap<string, CTreeParameter*>::iterator p = findRegistration();
  if(p != end()) {
    m_ObjectRegistry.erase(p);
  }
}


/**
 * Binds this treeparameter to a CParameter, creating if necessary.
 * If the object is already bound, it is rebound in case the parameter it is bound
 * to is  not the 'correct' one (e.g. after copy constuction).
 */
void 
CTreeParameter::Bind()
{

  SpecTcl& api(*(SpecTcl::getInstance()));
  CParameter* pParam = api.FindParameter(m_sName);
  
  // If the parameter does not exist, create it; if it does,
  //  We re-get the units definition from	the existing parameter.
  
  if(!pParam) {
    pParam     = api.AddParameter(m_sName, api.AssignParameterId(), m_sUnits);
  }
  else {
    m_sUnits   = pParam->getUnits();
  }
  m_pParameter = pParam;

}


/**
 * Binds all parameters that are registered to CParameter items known to SpecTcl.
 */
void 
CTreeParameter::BindParameters()
{
  BindVisitor visitor;
  BindVisitor& bv(visitor);
  for_each(begin(), end(), bv);


}


/**
 * Binds an event to the class of Tree parameters.  Modifications to parameter
 * values will be reflected in this event.
 * @param rEvent
 *        Reference to the current event.  This is normally passed into
 *        the operator() of an event processor that will set us up.
 * 
 */
void 
CTreeParameter::setEvent(CEvent& rEvent)
{
  m_pEvent = &rEvent;
}


/**
 * \return bool
 * \retval True if the parameter is bound to an event element.  
 * \retval False for either of two cases:
 *         - m_pParameter NULL - there has not been a binding of the object to a 
 *           named parameter.
 *         - m_pEvent NULL - the class is not bound to a 'current event'.
 */
bool 
CTreeParameter::isBound()
{
	if(!m_pParameter) {
	   return false;
	}
	// If there's a parameter binding, there must also be an event as
	// otherwise we can't assign/get a value from this.
	
	return (m_pEvent != (CEvent*)NULL);
}

/**
 * This initialization function takes a parameter name and parameter resolution
 * (in bits) as parameters.  It is typically used when the default constructor was
 * used to create a parameter.  If m_pParameter is non null, an exception is
 * thrown, as this indicates the parameter has already forced the creation of a
 * new SpecTcl parameter.  Otherwise, the tree parameter name is set, the units
 * are cleared and the limits are modified as described by the parameterBits
 * parameter.  If the parameter is not already registered for later binding, it is
 * registered now.
 * @param name
 *        Name of new parameter.
 * @param resolution
 *        Bits of parameter resolution.
 * 
 */
void 
CTreeParameter::Initialize(string name, UInt_t resolution)
{

  Initialize(name);
  // This registers me.
  // Override the defaults with our defaults where different.
    
  m_dLowLimit          = 0.0;
  m_nChannels          = 1 << resolution;
  m_dHighLimit         = (float)(m_nChannels); // This is an exclusive limit
  m_fDefinitionChanged = false;
  
  

}


/**
 * Given a parameter name, parameter resolution (in bits), start/stop/channels
 * given either as start/stop/channels or as start/width/channels, and units,
 * initializes the parameter.
 * This is intended to be used with parameters that are default constructed. In
 * any event, if m_pParameter is not null, an exception is thrown.  Otherwise, the
 * parameter's internal members are set from the parameters and the parameter is,
 * if not already registered, registered for later binding.
 * @param name
 *        Name of the parameter that we will be generating.
 * @param resolution
 *        number of bits of resolution the parameter has.
 * @param lowLimit
 *        Low axis limit of the parameter on axes created by the GUI.
 * @param highOrWidth
 *        Either the high limit or the channel width depending on
 *        the highOrWidthGiven flag.
 * @param units
 *        Units of measure of the parameter.
 * @param widthOrHighGiven
 *        -false  - highOrWidth is the default high limit of axes created by the GU.
 *        -true - highOrWidth is the channel width in parameter coordinates.
 * 
 */
void 
CTreeParameter::Initialize(string name, UInt_t resolution, 
			   double lowLimit, double highOrWidth, 
			   string units, bool highOrWidthGiven)
{
  // Compute the number of channels, and the high limit and then
  // delegate to yet another initialize:
    
  UInt_t  nChannels = 1 << resolution;
  double  highLimit;

  if(!highOrWidthGiven) {
    highLimit = highOrWidth;
  }
  else {
    highLimit = lowLimit + (nChannels) * highOrWidth;
  }
  Initialize(name, nChannels, lowLimit, highLimit, units);
}


/**
 * takes only a parameter name.  If m_pParameter is not NULL, throws an exception.
 * otherwise, intializes the parameter name and, if the parameter is not already
 * registered, registers it.
 * @param name
 *        Name of the parameter to create.
 * 
 */
void 
CTreeParameter::Initialize(string name)
{

  if(m_pParameter) {
     throw CTreeException(CTreeException::Bound, 
			  "CTreeParameter::Initialize(string)");
  }
  m_sName              = name;
  m_sUnits             = "unknown";
  m_dLowLimit          = 1.0;
  m_dHighLimit         = 100.0;
  m_nChannels          = 100;
  m_fDefinitionChanged = false;
  Register();

}


/**
 * Takes a parameter name and units.  If m_pParameter is not null, throws an
 * exception.  Otherwise, initializes m_sName, and m_sUnits from the parameters
 * and, if the parameter is not already registered, registers it for later binding.
 * 
 * @param name
 *        Name of the parameter we are creating.
 * @param units
 *        Units of parameter being created.
 * 
 */
void 
CTreeParameter::Initialize(string name, string units)
{
  Initialize(name);             // This registers us.
  m_sUnits = units;		// Override the default units spec.
}


/**
 * Initializes a parameter givena a  name, units, and parameter range.   If
 * m_pParameter is non-null, throws an exception.  Otherwise, sets m_sName,
 * m_dLowLimit, m_dHighLimit, and m_sUnits from the parameters. m_nChannels is
 * chosen 'appropriately'.   If the parameter is ont already registered for later
 * binding, it is registered.
 * @param name    
 *        Name of the new parameter.
 * @param channels
 *        Number of channels in axes created by default by the GUI on
 *        this paramter.
 * @param lowLimit
 *        default low limit of axes on this parameter created by the GUI
 * @param highLimit
 *        default high limit of the parameter on axes created by th GUI for this
 *        parameter.
 * @param units
 *        Parameter units of measure.
 * 
 */
void 
CTreeParameter::Initialize(string name, UInt_t channels, 
			   double lowLimit, double highLimit, string units)
{

  Initialize(name, units);
  
  // Re-use the code we can and override the defaults:
  
  m_dLowLimit  = lowLimit;
  m_dHighLimit = highLimit;
  m_nChannels  = channels;

}

/**
 * Cases:
 * - isBound() == false  Throws an exception since the underlying value cannot be
 *                       deduced.
 * - Otherwise, returns (*m_pEvent)[m_pParameter->getNumber()];
 *
 *  All of these details are handled by the factorization in to getValue.
 */
CTreeParameter::operator double()
{
  return getValue();
}


/**
 * Assignment to this from double The following cases:
 * - isBound() false - throws an exception.
 * - Otherwise:  (*m_pEvent)[m_pParameter->getNumber()] = rhs, and return *this.
 * @param newValue
 *        New value for the parameter.
 * 
 */
CTreeParameter& 
CTreeParameter::operator=(double newValue)
{

  setValue(newValue);
  
  return *this;


}


/**
 * Assignment from another treeparameter:  The assumption is that what is wanted
 * is not a duplicate tree parameter, but to transfer the underlying value.
 * Therefore there are the following cases:
 * - isBound() false :  Throw an exception as the source/target is not defined.
 * - Otherwise: 
 *    (*m_pEvent)[m_pParameter->getNumber()] = 
 *                                (*(rhs.m_pEvent))[rhs.m_pParameter->getNumber()]
 *   and return *this.
 * - If TREEPARAMETERSTRICTUNITS is defined (preprocessor macro), and the units of
 *    the two tree parameters are not the same, then a UnitsMismatch exception gets thrown.
 * @param rhs
 *        The tree parmaeter from which we will assign.
 * 
 */
CTreeParameter& 
CTreeParameter::operator=(CTreeParameter& rhs)
{
  if(&rhs != this) {

    if(StrictTypeChecking) {
      if (rhs.getUnit() != getUnit()) {
	throw CTreeException(CTreeException::UnitsMismatch,
			     "CTreeParameter::operator=(const CTreeParameter&)");
      }
    }
    
    setValue(rhs.getValue());
  }
 return *this;
}


/**
 * Add double to this Cases:
 * - isBound() false - throw an exception.
 * - (*m_pEvent)[m_pParameter->getNumber()]   += rhs;
 * @param rhs
 *        value to add to the parameter.
 * 
 */
CTreeParameter& 
CTreeParameter::operator+=(double rhs)
{
  setValue(getValue() + rhs);
  return *this; 
}


/**
 * returns operator+= -rhs.
 * @param rhs
 *        Value to subtract from this.
 * 
 */
CTreeParameter& 
CTreeParameter::operator-=(double rhs)
{
  setValue(getValue() - rhs);
  return *this;
}


/**
 * Cases:
 * - !ifBound()  - Throw an exception.
 * - Otherwise, (*m_pEvent)[m_pParameter->getNumber()] += rhs and return *this.
 * @param rhs
 *        value to multiply this by.
 * 
 */
CTreeParameter& 
CTreeParameter::operator*=(double rhs)
{
  setValue(getValue()*rhs);
  return *this;
}


/**
 * Cases:
 * - !ifBound() - Throw an exception.
 * - (*m_pEvent)[m_pParameter->getNumber()] /= rhs, return *this
 * @param rhs
 *        Value to divide *this by.
 * 
 */
CTreeParameter& 
CTreeParameter::operator/=(double rhs)
{
  setValue(getValue()/rhs);
  return *this;
}


/**
 * Post-increment:
 * If the parameter is not bound an exception is thrown.  Otherwise,  a copy of
 * the value of the underlying parameter is taken and the underlying parameter is
 * then incremented.  Note that this modifies slightly the semantics of
 * operator++(int) in that you cannot cascade to in this way:
 * \verbatim
 *    (a++).getName();
 * \endverbatim
 * However this is the  \em only way to preserve the numerical semantics (e.g.
 * \verbatim
 *    b = a++;    // b has value prior to increment.
 * \endverbatim
 * 
 * 
 * @param dummy
 *        Forcing this to be the a++ as opposed to the ++a
 * 
 */
double
CTreeParameter::operator++(int dummy)
{
  double prior = getValue();
  setValue(prior+1.0); 
  return prior;     // post increment return semantics.
}


/**
 * Pre-increment:
 * If the parameter is not bound, throws an exception.  Otherwise, increments the
 * undelying parameter, and then returns *this.
 */
CTreeParameter& 
CTreeParameter::operator++()
{
  setValue(getValue() + 1.0);
  return *this;      // Post increment semantics.
}


/**
 * Post-decrement:
 * If not bound throws an exception.  Otherwise keeps a copy of the underlying
 * event parameter value, decrements it and returns the prior >value< of the
 * underlying parameter.  See the notes on post++ for semantic implications of
 * this choice of behavior.
 * @param dummy
 *        Required to make this a postdecrement-- as opposed to --predecrement.
 * 
 */
double 
CTreeParameter::operator--(int dummy)
{
  double prior = getValue();
  setValue(prior -1.0);
  return prior;
}


/**
 * Pre decrement:
 * If not bound, throws an exception. Otherwise, decrements the underlying
 * parameter and returns *this.
 */
CTreeParameter& 
CTreeParameter::operator--()
{
  setValue(getValue() - 1.0);
  return *this;
}


/**
 * Returns the parameter's name.  Note that the name returned is that in m_sName...
 * which can be empty if the default constructor was used to create *this. 
 */
string 
CTreeParameter::getName()
{
  return m_sName;

}


/**
 * If m_pParameter is null throws an exception. Otherwise, returns
 * m_pParameter->getNumber()
 */
static string gidstring("CTreeParameter::getId");
int 
CTreeParameter::getId()
{
  
  ThrowIfNoParameter(gidstring);
  return m_pParameter->getNumber();
}


/**
 * Get the value of the underlying parameter.
 */
static string gvaluestring("CTreeParameter::getValue");
double 
CTreeParameter::getValue()
{
  ThrowIfNoEvent(gvaluestring);
  return (*m_pEvent)[getId()];
}


/**
 * Set Value of underlying parameter from argument.
 * @param newValue
 *        New value to give the parameter.
 * 
 */
static string setvaluestring("Setting Value");
void 
CTreeParameter::setValue(double newValue)
{

  ThrowIfNoParameter(setvaluestring);
  (*m_pEvent)[getId()] = newValue; // Getid calls ThrowIfNoEvent.
}


/**
 * Returns the current default number of channels on the default axis of a
 * spectrum created by the GUI.
 */
UInt_t 
CTreeParameter::getBins()
{
  return m_nChannels;
}


/**
 * Modifies the number of channels in a default axis created by the GUI (also sets
 * m_fDefinitionChanged to true).
 * @param channels
 *        New value for the default number of channels on an axis
 *        created by the GUI.
 * 
 */
void 
CTreeParameter::setBins(UInt_t channels)
{
  m_nChannels = channels;
  m_fDefinitionChanged = true;
}


/**
 * Returns the default lower limit of parameter axes created by the GUI for this
 * parameter.
 */
double 
CTreeParameter::getStart()
{
  return m_dLowLimit;
}


/**
 * Modifes the low limit of an axis created by default for this parameter by the
 * GUI.  This also sets m_fDefinitionChanged to true.
 * @param low
 *        New default parameter low limit for axes created by the GUI on
 *        this parameter.
 * 
 */
void 
CTreeParameter::setStart(double low)
{
  m_dLowLimit = low;
  m_fDefinitionChanged = true;
}


/**
 * Retrieves the value of the high limit of an axis created on this parameter by
 * the GUI.   
 */
double 
CTreeParameter::getStop()
{
  return m_dHighLimit;
}


/**
 * Modifies the upper limit of an axis created by the gui on this parameter.  Also
 * sets the m_fDefinitionChanged to be true.
 * @param high
 *        Default High limit on the axes created by the GUI for this
 *        parameter.
 * 
 */
void 
CTreeParameter::setStop(double high)
{
  m_dHighLimit = high;
  m_fDefinitionChanged = true;
}


/**
 * For the current values of m_dLowLimit, m_dHighLimit, and m_nChannels, computes
 * and returns the width of a channel in parameter units.  Note that this 'channel
 * width' is that of the default axis for ths parameter of spectra created by the
 * gui.
 */
double 
CTreeParameter::getInc()
{

  return (m_dHighLimit - m_dLowLimit)/m_nChannels;   
}


/**
 * Holding the m_dLowLimit and m_nChannels fixed, computes a new m_dHighLimit such
 * that getInc will return this value.  Note that this will set
 * m_fDefinitionChanged.
 * @param channelWidth
 *        The new desired channel width.
 * 
 */
void 
CTreeParameter::setInc(double channelWidth)
{
  m_dHighLimit = m_dLowLimit + (m_nChannels)*channelWidth;
  m_fDefinitionChanged = true;


}


/**
 * Returns the unit string.  Note that internal units string is returned
 * regardless of the binding state.
 */
string 
CTreeParameter::getUnit()
{
  return m_sUnits;
}
/**
   Set the units of measure... not sure why you'd do this.  Note that the
   underlying parameter cannot have its units modified. Future versions of this
   may do parameter destroy/recreate, or alternatively we may support units modification
  in parameters to keep consistency.
*/
void
CTreeParameter::setUnit(string units)
{
  m_sUnits = units;
  m_fDefinitionChanged = true;
}

/**
 * If the parameter is not bound, an exception is thrown.  Otherwise, the validity
 * field of the underlying event parameter is returned.
 */
bool 
CTreeParameter::isValid()
{
  UInt_t id = getId();                // Throws if no underlying parameter.
  if(!m_pEvent) {
    throw CTreeException(CTreeException::
			 NotBound, "CTreeParameter::isValid");
  }
  
  return (*m_pEvent)[id].isValid();


}


/**
 * If the parameter is not bound, an exception is thrown.  Otherwise, the
 * underlying event's parameter is invalidated.  Invalid parameters will not be
 * involved in spectrum icrements.
 */
void 
CTreeParameter::setInvalid()
{
  
  UInt_t id = getId();                // Throws if no underlying parameter.
  if(!m_pEvent) {
    throw CTreeException(CTreeException::NotBound, "CTreeParameter::setInvalid");   
  }
  
  (*m_pEvent)[id].clear();
  
}

/**
 *   Reset the validity of the parameter... e.g. make it invalid.
 */
void 
CTreeParameter::Reset()
{
  setInvalid();
}


/**
 * Yet another invalidator for CTreeParameters.
 */
void 
CTreeParameter::clear()
{
  setInvalid();
}


/**
 * Returns the value of m_fDefinitionChanged.
 */
bool 
CTreeParameter::hasChanged()
{
  return m_fDefinitionChanged;
}


/**
 * Marks the definition of the parameter changed by setting m_fDefinitionChanged
 * true.
 */
void 
CTreeParameter::setChanged()
{
  m_fDefinitionChanged = true;

}


/**
 * Marks the definition of the parameter unchanged by setting m_fDefinitionChanged
 * to be false.
 * 
 */
void 
CTreeParameter::resetChanged()
{
  m_fDefinitionChanged = false;
}


/**
 * Iterates through the registered parameters, resetting all bound parameters.
 * Any unbound parameters are ignored.
 */
void 
CTreeParameter::ResetAll()
{

  // Note in general it is a waste of  time to call this as SpecTcl will  
  // invalidate all parameter prior  to processing each event.

  ResetVisitor visitor;

  for_each(begin(), end(), visitor);

  
  
}


/**
 * Returns an start iteration iterator to the registration map.
 */
multimap<string, CTreeParameter*>::iterator 
CTreeParameter::begin()
{
  return m_ObjectRegistry.begin();

}


/**
 * Returns an end of iteration iterator to the registration map.
 */
multimap<string, CTreeParameter*>::iterator 
CTreeParameter::end()
{
  return m_ObjectRegistry.end();
}


/**
 * Locate the first parameter that has the name passed in, or return end().
 * @param name
 *        Name to search for.
 * 
 */
multimap<string, CTreeParameter*>::iterator 
CTreeParameter::find(string name)
{
  return m_ObjectRegistry.find(name);

}



/*
   Throw a not bound exception if not parameter is defined for this treeeparameter.
*/
void
CTreeParameter::ThrowIfNoParameter(string& doing)
{
  if(!m_pParameter) {
    throw CTreeException(CTreeException::NotBound, doing);
  }
}

/*
   Throw a not bound exception if no event is bound to the class

*/
void
CTreeParameter::ThrowIfNoEvent(string& doing)
{
  if(!m_pEvent) {
    throw CTreeException(CTreeException::NotBound, doing);
  }
}
//
// Test support function empties the parameter map. deleting all parameters
// contained in it. If used outside the test suites. this is a bad thing to call.
//
void
CTreeParameter::testClearMap()
{
  // Several tree parameters may point to the same parameter 
  // so first we enumerate, sort, uniquify and then delete all the CParameters*

  multimap<string, CTreeParameter*>::iterator p = m_ObjectRegistry.begin();
  vector<CParameter*> parameters;
  while(p != m_ObjectRegistry.end()) {
    parameters.push_back(p->second->m_pParameter);
    p++;
  }
  sort(parameters.begin(), parameters.end());
  unique(parameters.begin(), parameters.end());

  // Get rid of the parameters...

  vector<CParameter*>::iterator pp = parameters.begin();
  while(pp != parameters.end()) {
    delete *pp;
    pp++;
  }

  // Get rid of the tree parameters:
  // As each is deleted, it destroys its registry entry... so once more we
  // first marshall them into a vector and thenkill them off:

  vector<CTreeParameter*> trees;
  
  m_pEvent = (CEvent*)NULL;	// not bound to an event.
  p = m_ObjectRegistry.begin();
  while (p != m_ObjectRegistry.end()) {
    trees.push_back(p->second);
    p++;
  }
  for(int i =0; i < trees.size(); i++) {
    delete trees[i];
  }

  m_ObjectRegistry.clear();
}
void
CTreeParameter::dumpmap()
{
  multimap<string, CTreeParameter*>::iterator p = begin();
  while(p != end()) {
    string name = p->first;
    CTreeParameter* pParam = p->second;
    cerr << name << " @ " << hex << pParam << dec << endl;
    if(pParam) {
      cerr << "  name:  " << pParam->m_sName <<  endl;
      cerr << "  units: " << pParam->m_sUnits << endl;
      cerr << " param : " << hex << pParam->m_pParameter << dec << endl;
    }
    cerr << "-----------\n";
    p++;
  }
}
