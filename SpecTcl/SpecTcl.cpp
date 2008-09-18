///////////////////////////////////////////////////////////
//  SpecTcl.cpp
//  Implementation of the Class SpecTcl
//  Created on:      08-Mar-2005 09:57:36 AM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

// Implement the SpecTcl API class.  See SpecTcl.h for more 
// information about the class itself, and indivual member
// comments.   These can all be pulled together with Doxygen.
//
// Headers:

#include <config.h>
#include "SpecTcl.h"

#include <Parameter.h>
#include <Spectrum.h>


#include <Gate.h>
#include <AndGate.h>
#include <CompoundGate.h>
#include <DeletedGate.h>
#include <FalseGate.h>
#include <OrGate.h>
#include <PointlistGate.h>
#include <TrueGate.h>
#include <Band.h>
#include <C2Bands.h>
#include <Contour.h>
#include <Cut.h>
#include <CGammaBand.h>
#include <CGammaContour.h>
#include <CGammaCut.h>
#include <MaskGates.h>
#include <MaskEqualGate.h>
#include <MaskAndGate.h>
#include <MaskNotGate.h>
#include <Not.h>


#include <GateContainer.h>
#include <EventProcessor.h>
#include <SpectrumFormatter.h>
#include <EventSink.h>

#include <Histogrammer.h>
#include <GateFactory.h>
#include <GateFactoryException.h>
#include <SpectrumFactory.h>
#include <SpectrumFactoryException.h>
#include <SpectrumFormatterFactory.h>

#include <FilterDictionary.h>
#include <CFilterOutputStageFactory.h>

#include <DictionaryException.h>

#include <TCLInterpreter.h>

#include <TCLAnalyzer.h>


#include <Globals.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

// Define the class specific member data; This represents the 
// the single instance that this class is allowed to have:


SpecTcl* SpecTcl::m_pInstance(0);

/*!
  The only reason we have a constructor is so that it can be kept private to
  prevent 'unauthorized' creation of the API object.   At present this is not
  really an issue, but may become one later, as we add state to the API that is
  now hiding in Globals.h 

  The actual creation of the singleton is done in the first call to 
  SpecTcl::getInstance().
*/
SpecTcl::SpecTcl()
{

}
/*!
   Same comments for the constructor here.
*/
SpecTcl::~SpecTcl()
{

}

/*!
  Retrieve the instance of the singleton SpecTcl Object.  
*/
SpecTcl* 
SpecTcl::getInstance()
{

  // If necessary create the singleton.

  if(!m_pInstance) {
    m_pInstance = new SpecTcl;
  }
  // Regardless return it to the caller.

  return m_pInstance;


}
///////////////////////////// API functions /////////////////////////////

/*!
  Register the creator for a new type of buffer decoder.
*/
void
SpecTcl::addBufferDecoder(string type,
			  CAttachCommand::CDecoderCreator* creator)
{
  CAttachCommand::addDecoderType(type, creator);
}

/*!
  Allocate a new parameter id. and return it to the caller.
  The id returned is the smallest unused id.  This may take time
  that is O(n^2) in the number of parameters.
*/
UInt_t 
SpecTcl::AssignParameterId()
{

  vector<UInt_t> existingIds;	// Will hold the existing parameter ids:
  for(ParameterDictionaryIterator i = BeginParameters();
      i != EndParameters(); i++) {
    CParameter* pParameter = &i->second;
    existingIds.push_back(pParameter->getNumber());
  }
  // Sort the ids:

  sort(existingIds.begin(), existingIds.end());
  
  // See if there are holes we can fit the id into:

  UInt_t id(0);

  for(UInt_t i = 0; i < existingIds.size(); i++) {
    if(id != existingIds[i]) {
      return id;
    }
    id++;
  }
  return id;			// If a tight block starting @ 0 return next int
}


/*!
  Create a new parameter that is a floating point parameter, and add it to the
  parameter dictionary.  Note that AssignParameterId can be used to get an unused
  slot in the parameter array.

  @param name
    The name of the parameter to add.
  @param Id
    The slot in the CEvent 'array' to which this parameter is bound.
  @param Units
    Units associated with this parameter.  If an empty string is
    supplied, the parameter
    has no units.  Units are displayed on spectra which have the parameter as an
    axis.

  \return CParameter*
  \retval A pointer to the new parameter.  This parameter is dynamically created
          and therefore must be deleted by you if you manually remove it from
	  the dictionary.  The parameter -delete command assumes dynamically
	  created parameters and will do this for you if the parameter is deleted
	  via SpecTcl command or script.

  \throw CDictionaryException if the parameter name or id is a duplicate.
*/
CParameter* 
SpecTcl::AddParameter(string name, UInt_t Id, string Units)
{
  CHistogrammer* pHistogrammer  = GetHistogrammer();
  CParameter*    pParameter    = pHistogrammer->AddParameter(name, Id, 
	 						    Units.c_str());
  return pParameter;

}


/*!
  Create a new parameter that is a scaled integer parameter compatibile with
  SpecTcl 0.4  The parameter is entered into the parameter dictionary.

  @param name
    Name of the new parameter.
  @param id
    Id of the parameter.  The id represents the slot in the CEvent
    'array' that this parameter name is bound to.
  @param scale
    The number of bits in the parameter.

  \return CParameter*
  \retval A pointer to the new parameter.  This parameter is dynamically created
          and therefore must be deleted by you if you manually remove it from
	  the dictionary.  The parameter -delete command assumes dynamically
	  created parameters and will do this for you if the parameter is deleted
	  via SpecTcl command or script.  

  \throw CDictionaryException if the parameter name or id is a duplicate.
*/
CParameter* 
SpecTcl::AddParameter(string name, UInt_t id, UInt_t scale)
{
  CHistogrammer* pHistogrammer   = GetHistogrammer();
  CParameter*    pParameter      = pHistogrammer->AddParameter(name, id,
							       scale);
  return pParameter;
}


/*!
  Add a mapped paraemeter that is a scaled integer.
  This type was introduced in SpecTcl 2.0, rejected by the uses
  and kept for compatibility anyway.

  @param name
    Name of the parameter
  @param id
    Id of the parameter.
  @param scale
    Number of bits in the integer parameter
  @param low
    The low value of the window in real coordinates represented by
    this integer.
  @param high
    The high value of the window represented by this integer value.
  @param units
    The units of measure of the real coordinates represented by
    this integer.

  \return CParameter*
  \retval A pointer to the new parameter.  This parameter is dynamically created
          and therefore must be deleted by you if you manually remove it from
	  the dictionary.  The parameter -delete command assumes dynamically
	  created parameters and will do this for you if the parameter is deleted
	  via SpecTcl command or script.  
  

  \throw CDictionaryException if the parameter name or id is a duplicate.
 */
CParameter* 
SpecTcl::AddParameter(string name, UInt_t id, 
				  UInt_t scale, Float_t low, Float_t high, 
				  string units)
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  CParameter*    pParameter    = pHistogrammer->AddParameter(name, id, 
							     scale, low, high,
							     units);
  return pParameter;
}


/*!
   Removes a parameter from the dictionary of parameters.
   The parameter object is not destroyed, but returned to the caller.
  @param name
    name of the parameter.

  \return CParameter*
  \retval - Pointer to the parameter removed.  It is your responsibility to 
            delete it
  \retval NULL - No such parameter.

*/
CParameter* 
SpecTcl::RemoveParameter(string name)
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  return         pHistogrammer->RemoveParameter(name);
}


/*!
  Find a parameter, returns a pointer to it to the user.
  @param name
    Name of the parameter to locate.

  \return CParameter*
  \retval - Pointer to the parameter removed.
  \retval NULL - No such parameter.
  
*/
CParameter* 
SpecTcl::FindParameter(string name)
{  
  CHistogrammer* pHistogrammer = GetHistogrammer();
  return         pHistogrammer->FindParameter(name);
}


/*!
  Find a parameter given its id.
  
  @param Id
    Id of the parameter to locate.

  \return CParameter*
  \retval - Pointer to the parameter removed.
  \retval NULL - No such parameter.
  
  
*/
CParameter* 
SpecTcl::FindParameter(UInt_t Id)
{
  CHistogrammer*   pHistogrammer = GetHistogrammer();
  return           pHistogrammer->FindParameter(Id);
}


/*!
  Return a begin iterator to the parameter dictionary.
*/
ParameterDictionaryIterator
SpecTcl::BeginParameters()
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  return         pHistogrammer->ParameterBegin();
}


/*!
  Returns an end of iteration iterator for the parameter dictionary.
 */
ParameterDictionaryIterator 
SpecTcl::EndParameters()
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  return         pHistogrammer->ParameterEnd();
}


/*!
  The current count of the number of defined parameters.
*/
UInt_t 
SpecTcl::ParameterCount()
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  return         pHistogrammer->ParameterCount();
}


/*!
  Creates a new spectrum.  This is a
  front   end to the spectrum factory.  A  pointer to the new spectrum is returned.
  @param Name
    Name of the spectrum to create.
  @param type
    Type of the spectrum to create.
  @param dataType
    Type of data to store in a channel (e.g. keByte keLong etc.).  
  @param parameters
    The set of parameters that this spectrum requires to
    increment.  These are interpreted in a spectrum type dependent manner.
  @param channels
    Number of channels for each axis in the spectrum.
  @param pLows
    Pointer to the vector of channel low limits. If omitted, the
    low limits for all channels is 0.0
  @param pHighs
    Pointer to the vector of axisl high limits.  If omitted, the
    high limit on axis i for all i is Channels[i]-1.
  
  \return CSpectrum*
  \retval Pointer to the spectrum that was created.  Note that if you want the
          histogrammer to be aware of the spectrum, you must enter it into the
	  spectrum dictionary via SpecTcl::AddSpectrum.

  \throw CSpectrumFactoryException if there is a problem.
*/
CSpectrum* 
SpecTcl::CreateSpectrum(string Name, 
			SpectrumType_t type, DataType_t dataType, 
			vector<string> parameters, 
			vector<UInt_t> channels, 
			vector<Float_t>* pLows, 
			vector<Float_t>* pHighs)
{
  CSpectrumFactory factory;
  return           factory.CreateSpectrum(Name, type, dataType,
					  parameters, channels,
					  pLows, pHighs);
}


/*!
   Creates a new spectrum that requires separate x/y axis information.
   we are a bit more flexible than the spectrum factory method.
   For all but Gamma 2D Deluxe spectra (keG2DD), we are going to 
   re-marshal the parameters for a call to the previous CreateSpectrum.
   For Gamma 2D Delux we will delegate to CreateG2DDeluxe below.
   @param Name          Name of the spectrum to create.
   @param type          Type of spectrum to create.
   @param dataType      Channel datatype for the spectrum.
   @param xParameters   Names of the parameters on the x axis.
   @param yParametesr   Names of the paramteers on the y axis.
   @param channels      Vector of number of channels on the axes.
   @param pLows         Pointer to low level axis cut offs.
   @param pHighs        Pointer to high level axis cut offs.

   \return CSpectrum*
   \retval The spectrum created.  If a spectrum cannot be created,
      the spectrum factor will throw an exception the caller should catch
      at some level or else SpecTcl will exit.
*/
CSpectrum*
SpecTcl::CreateSpectrum(STD(string) Name,
			SpectrumType_t type,
			DataType_t     dataType,
			STD(vector)<STD(string)> xParameters,
			STD(vector)<STD(string)> yParameters,
			STD(vector)<UInt_t>      channels,
			STD(vector)<Float_t>*    pLows,
			STD(vector)<Float_t>*    pHighs)
{
  if (type == keG2DD) {
    return CreateG2DDeluxe(Name, dataType, 
			    xParameters, yParameters,
			    channels, pLows, pHighs);
  }
  else {
    // conglomorate the parameters into one set and delegate.
    
    xParameters.insert(xParameters.end(),
		       yParameters.begin(), yParameters.end());
    return CreateSpectrum(Name, type, dataType, xParameters,
			  channels, pLows, pHighs);
  }
}

/*!
  Create a Gamma 2d Deluxe spectrum

   @param Name          Name of the spectrum to create.
   @param type          Type of spectrum to create.
   @param dataType      Channel datatype for the spectrum.
   @param xParameters   Names of the parameters on the x axis.
   @param yParametesr   Names of the paramteers on the y axis.
   @param channels      Vector of number of channels on the axes.
   @param pLows         Pointer to low level axis cut offs.
   @param pHighs        Pointer to high level axis cut offs.

   \return CSpectrum*
   \retval The spectrum created.  If a spectrum cannot be created,
      the spectrum factory will throw an exception the caller should catch
      at some level or else SpecTcl will exit.
*/

CSpectrum*
SpecTcl::CreateG2DDeluxe(STD(string) Name,
			DataType_t     dataType,
			STD(vector)<STD(string)> xParameters,
			STD(vector)<STD(string)> yParameters,
			STD(vector)<UInt_t>      channels,
			STD(vector)<Float_t>*    pLows,
			STD(vector)<Float_t>*    pHighs)
{
  if (channels.size() != 2) {
    throw CSpectrumFactoryException(dataType, keG2DD,
				    Name,
				    CSpectrumFactoryException::keBadResolutionCount,
				    "SpecTcl::CreateG2DDeluxe marshalling arguments");
  }
  CSpectrumFactory factory;
  return factory.CreateSpectrum(Name, keG2DD, dataType,
				xParameters, yParameters, 
				channels[0], channels[1],
				pLows, pHighs);

}
/*!
  Creates a 1-d spectrum with an x axis that runs from
  0-n in parameter space.
  @param name
    Name of the new spectrum.
  @param dataType
    Type of data to store in the spectrum (e.g. keLong).
  @param paramete
    Reference to the object that describes the parameter to
    a histogram.
  @param channels
    Number of x axis channels.
  
  \return CSpectrum*
  \retval Pointer to the spectrum that was created.  Note that if you want the
          histogrammer to be aware of the spectrum, you must enter it into the
	  spectrum dictionary via SpecTcl::AddSpectrum.

  \throw CSpectrumFactoryException if there is a problem.
*/
CSpectrum* 
SpecTcl::Create1D(string name, 
		  DataType_t dataType, 
		  CParameter& parameter, 
		  UInt_t channels)
{
  CSpectrumFactory factory;
  return           factory.Create1D(name, dataType, parameter, channels);
}


/*!
  Creates a 1d spectrum that covers an arbitrary region of
  parameter space with an arbitrary number of channels.
  @param name
    Name of the new spectrum.
  @param dataType
    Type of data stored in each channel (e.g. keWord).
  @param parameter
    Reference to the parameter that will be histogrammed in
    this spectrum.
  @param channels
    Number of channels on the x axis.
  @param lowLimit
    Low limit of real coordinates on the x axis.
  @param hiLimit
    High real coordinate limit on the x axis.
  
  \return CSpectrum*
  \retval Pointer to the spectrum that was created.  Note that if you want the
          histogrammer to be aware of the spectrum, you must enter it into the
	  spectrum dictionary via SpecTcl::AddSpectrum.

  \throw CSpectrumFactoryException if there is a problem.
  
*/
CSpectrum* 
SpecTcl::Create1D(string name, 
		  DataType_t dataType, 
		  CParameter& parameter, 
		  UInt_t channels, Float_t lowLimit, Float_t hiLimit)
{
  CSpectrumFactory factory;
  return           factory.Create1D(name, dataType, parameter,
				    channels, lowLimit, hiLimit);
  

}


/*!
  Creates a spectrum  and returns a pointer to it.
  The X and Y axes run from [0..nx) and [0..ny) in parameter space.
  @param name
    Name of the spectrum to create.
  @param dataType
    Type of data to put in each channel
  @param xParameter
    Reference to the paramter on the X axis of the spectrum.
  @param yParmaeter
    Reference to the paramter on the Y axis of the spectrum.
  @param xChannels
    Number of channels on the X axis.
  @param yChannels
    Number of channels on the y axis of the spectrum.
  
  \return CSpectrum*
  \retval Pointer to the spectrum that was created.  Note that if you want the
          histogrammer to be aware of the spectrum, you must enter it into the
	  spectrum dictionary via SpecTcl::AddSpectrum.

  \throw CSpectrumFactoryException if there is a problem.
  
*/
CSpectrum* 
SpecTcl::Create2D(string name, DataType_t dataType, 
		  CParameter& xParameter, CParameter& yParameter, 
		  UInt_t xChannels, UInt_t yChannels)
{
  CSpectrumFactory factory;
  return           factory.Create2D(name, dataType,
				    xParameter, yParameter,
				    xChannels, yChannels);

}


/*!
  Creates a 2-d spectrum
  and returns a pointer to it.  The spectrum is created with
  arbitrary x/y binning and covers an arbitrary range in parameter space
  on both axes.
  @param name
    Name of the spectrum to create.
  @param dataType
    Type of data to store in each channel.
  @param xParameter
    Describes the parameter on the X axis.
  @param yParameter
    Describes the parameter on the y axis.
  @param xChannels
    Channels on the x axis.
  @param xLow
    Low limit of real coordinates on the y axis.
  @param xHigh
    Real coordinate upper bound on the x axis.
  @param yChannels
  Channels on the y axis.
  @param yLow
    Low limit of real coordinates on the Y axis.
  @param yHigh
    Upper limit on real coordaintes on the y axis.
   
  \return CSpectrum*
  \retval Pointer to the spectrum that was created.  Note that if you want the
          histogrammer to be aware of the spectrum, you must enter it into the
	  spectrum dictionary via SpecTcl::AddSpectrum.

  \throw CSpectrumFactoryException if there is a problem.
  
*/
CSpectrum* 
SpecTcl::Create2D(string name, DataType_t dataType,
		  CParameter& xParameter, CParameter& yParameter, 
		  UInt_t xChannels, Float_t xLow, Float_t xHigh, 
		  UInt_t yChannels, Float_t yLow, Float_t yHigh)
{
  CSpectrumFactory factory;
  return           factory.Create2D(name, dataType, 
				    xParameter, yParameter,
				    xChannels, xLow, xHigh,
				    yChannels, yLow, yHigh);
}


/*!
  Creates a 1-d gamma spectrum with simple binning
  (n channels that cover the range [0..n) in parameter space).
  A pointer to the spectrum is returned to the caller.
  @param name
    Name of the spectrum being created
  @param dataType
    Type of data to store in the spectrum.
  @param parameters
    Vector of pointers to parameter objects describing the
    parameters that increment this
    spectrum. 
  @param channels
    Number of channels on the X axis.
   
  \return CSpectrum*
  \retval Pointer to the spectrum that was created.  Note that if you want the
          histogrammer to be aware of the spectrum, you must enter it into the
	  spectrum dictionary via SpecTcl::AddSpectrum.

  \throw CSpectrumFactoryException if there is a problem.
 
 */
CSpectrum* 
SpecTcl::CreateG1D(string name, DataType_t dataType, 
		   vector<CParameter> parameters, UInt_t channels)
{
  CSpectrumFactory factory;
  return           factory.CreateG1D(name, dataType, parameters, channels);
}


/*!
  Creates a 1-d gamma spectrum with arbitrary binning
  covering an arbitrary segment in the parameter range.
  A pointer to the spectrum is returned.
  @param name
    Name of the spectrum to create.
  @param dataType
    Type of data to put in the spectrum.
  @param parameters
    Vector of pointers to the parameters that will be used to
    increment this spectrum.
  @param channels
    Number of channels on the X axis.
  @param lowLimit
    Lower limit of the real parameter space covered by the x
    axis.
  @param hiLimit
    Upper limit of the parameter space that is covered by the x
    axis of the spectrum.
   
  \return CSpectrum*
  \retval Pointer to the spectrum that was created.  Note that if you want the
          histogrammer to be aware of the spectrum, you must enter it into the
	  spectrum dictionary via SpecTcl::AddSpectrum.

  \throw CSpectrumFactoryException if there is a problem.
 
  
*/
CSpectrum* SpecTcl::CreateG1D(string name, DataType_t dataType, 
			      vector<CParameter> parameters, 
			      UInt_t channels, Float_t lowLimit, Float_t hiLimit)
{
  CSpectrumFactory factory;

  return           factory.CreateG1D(name, dataType, parameters,
				     channels, lowLimit, hiLimit);
}


/*!
  Creates a spectrum with simple binning
  (nx * ny channels covering parameter range
  [0..nx) and [0..ny)).  A pointer to it is returned to the caller.
  @param name
    Name of the new spectrum to create.
  @param dataType
    Data type to put in the channels.
  @param parameters
    Vector of pointer to parameter describing objects that
    will be histogrammed in this spectrum.
  @param xChannels
    # channels in x axis.
  @param yChannels
    # Channels on y axis.
   
  \return CSpectrum*
  \retval Pointer to the spectrum that was created.  Note that if you want the
          histogrammer to be aware of the spectrum, you must enter it into the
	  spectrum dictionary via SpecTcl::AddSpectrum.

  \throw CSpectrumFactoryException if there is a problem.
   
 */
CSpectrum* 
SpecTcl::CreateG2D(string name, DataType_t dataType, 
		   vector<CParameter> parameters, 
		   UInt_t xChannels, UInt_t yChannels)
{
  CSpectrumFactory factory;
  return           factory.CreateG2D(name, dataType,
				     parameters, 
				     xChannels, yChannels);
}


/*!
  Creates a 2-d gamma spectrum with arbitrary binning.
  A pointer to it returned to the caller.
  @param name
    Name of the new spectrum to create.
  @param dataType
    Type of the spectrum to create.
  @param parameters
    Vector of pointers to the parameters to histogram.
  @param xChannels
    Number of channels on the x axis.
  @param xLow
    Low limit of the x coordinates
  @param xHigh
    High limit of the x coordinates.
  @param yChannels
    Number of channels on the y axis.
  @param yLow
    Low limit on the y axis.
  @param yHigh
    High limit on the y axis.
   
  \return CSpectrum*
  \retval Pointer to the spectrum that was created.  Note that if you want the
          histogrammer to be aware of the spectrum, you must enter it into the
	  spectrum dictionary via SpecTcl::AddSpectrum.

  \throw CSpectrumFactoryException if there is a problem.
   
  
*/
CSpectrum* 
SpecTcl::CreateG2D(string name, DataType_t dataType, 
		   vector<CParameter> parameters, 
		   UInt_t xChannels, Float_t xLow, Float_t xHigh, 
		   UInt_t yChannels, Float_t yLow, Float_t yHigh)
{
  CSpectrumFactory   factory;
  return             factory.CreateG2D(name, dataType, parameters,
				       xChannels, xLow, xHigh,
				       yChannels, yLow, yHigh);
}


/*!
  Creates a spectrum of a bitmask.  The spectrum has
  n channels that cover bits 0..n-1 of the parameter
  that is cast to and assumed to be an integer.
  A pointer to the spectrum is returned.
  @param name
    Name of the spectrum being created.
  @param dataType
    Type of the data to put in the channels of the spectrum.
  @param parameter
    Reference to the parameter to histogram.
  @param channels
    Number of x axis channels in the spectrum  the low order
    'channels' bits of the parameter
    will be histogrammed.
   
  \return CSpectrum*
  \retval Pointer to the spectrum that was created.  Note that if you want the
          histogrammer to be aware of the spectrum, you must enter it into the
	  spectrum dictionary via SpecTcl::AddSpectrum.

  \throw CSpectrumFactoryException if there is a problem.
   
  
*/
CSpectrum* 
SpecTcl::CreateBit(string name, DataType_t dataType, 
		   CParameter& parameter, UInt_t channels)
{
  CSpectrumFactory factory;
  return           factory.CreateBit(name, dataType,
				     parameter, channels);
}


/*!
  A bit spectrum is created with n channels that
  cover an arbitrary set of bits in the integerized parameter.
  A pointer is returned to the spectrum.
  @param name
    Name of the spectrum to be created.
  @param dataType
    Type of data being created.
  @param parameter
    Reference to the parameter to histogram.
  @param channels
    Number of channels (bits) to histogram.
  @param lowBit
    Low order bit to histogram.
  
   
  \return CSpectrum*
  \retval Pointer to the spectrum that was created.  Note that if you want the
          histogrammer to be aware of the spectrum, you must enter it into the
	  spectrum dictionary via SpecTcl::AddSpectrum.

  \throw CSpectrumFactoryException if there is a problem.
   
*/
CSpectrum* 
SpecTcl::CreateBit(string name, DataType_t dataType, 
		   CParameter& parameter, 
		   UInt_t channels, UInt_t lowBit)
{
  CSpectrumFactory factory;
  return           factory.CreateBit(name, dataType, parameter,
				     lowBit, (lowBit + channels -1));
}


/*!
  Creates a summary spectrum with simple binning.  The Y axis has n channels
  that cover the range [0..n) in parameter space.
  A pointer is returned to the spectrum.
  @param name
    Name of the spectrum being created.  
  @param dataType
    Type of data to be stored in the spectrum channels.
  @param parameters
    Pointers to the parameters that will be histogrammed.
  @param channels
    Number of channels on the y axis.  The axis will run from 0-
    (channels-1) on the y axis.
   
  \return CSpectrum*
  \retval Pointer to the spectrum that was created.  Note that if you want the
          histogrammer to be aware of the spectrum, you must enter it into the
	  spectrum dictionary via SpecTcl::AddSpectrum.

  \throw CSpectrumFactoryException if there is a problem.
  
*/
CSpectrum* 
SpecTcl::CreateSummary(string name, DataType_t dataType, 
		       vector<CParameter> parameters, 
		       UInt_t channels)
{
  CSpectrumFactory factory;
  return           factory.CreateSummary(name, dataType,
					 parameters, channels);
}


/*!
  Creates a summary spectrum with an arbitrary number of channels.  The
  spectrum's y axis covers an arbitrary range in parameter space.
  A ponter to the new spectrum is returned.
  @param name
    name of the spectrum being created.
  @param dataType
    Type of data in each channel of the histogram.
  @param parameters
    Pointers to the parameters histogrammed in the spectrum.
  @param nChannels
    Number of y axis channels.
  @param low
    Low limit on parameter values on the Y axis.
  @param high
    High limit on the values on the y axis.
   
  \return CSpectrum*
  \retval Pointer to the spectrum that was created.  Note that if you want the
          histogrammer to be aware of the spectrum, you must enter it into the
	  spectrum dictionary via SpecTcl::AddSpectrum.

  \throw CSpectrumFactoryException if there is a problem.
  
*/
CSpectrum*
SpecTcl::CreateSummary(string name, DataType_t dataType, 
				  vector<CParameter> parameters, 
				  UInt_t nChannels, Float_t low, Float_t high)
{
  CSpectrumFactory factory;
  return           factory.CreateSummary(name, dataType, parameters,
					 nChannels, low, high);
}

/*!
   Create a gamma 2d deluxe spectrum.   This is a spectrum with independent
   X/Y axis parameters.
   \param name     - Name of the new spectrum.
   \param dataType - Type of data (defines the channel 'width').
   \param xParameters - Vector of paramters on the X axis.
   \param yParameters - Vector of paramters on the y Axis.
   \param xChannels   - Number of bins on the xAxis.
   \param xLow        - Low limit of the x axis in parameter coordinates.
   \param xHigh       - High limit of the y axis in parameter coordinates.
   \param yChannels   - Number of bins on the yAxis.
   \param yLow        - Low limit of the y axis in parameter coordinates.
   \param yHigh       - High limit of the y axis in parameter coordinates.

   \return CSpectrum*
   \retval Pointer to the newly created spectrum object.
   \retval NULL  There was a problem with the creation that did not require
                 an exception (I don't think this actually can happen).
    \throw CSpectrumFactoryException on a few different errors.

*/
CSpectrum*
SpecTcl::CreateGamma2DD(string name, DataType_t dataType, 
			vector<CParameter> xParameters,
			vector<CParameter> yParameters,
			UInt_t xChannels, Float_t xLow, Float_t xHigh,
			UInt_t yChannels, Float_t yLow, Float_t yHigh)
{
  CSpectrumFactory factory;
  return           factory.CreateG2dDeluxe(name,
					   dataType,
					   xParameters, yParameters,
					   xChannels, xLow, xHigh,
					   yChannels, yLow, yHigh);
}

/*!
   Creates a 2-d multiply incremented spectrum.  Where the Gamma 2d Deluxe
   spectrum increments for all combinations of x/y pairs, this spectrum only 
   increments for corresponding pairs of parameters.

   \param name     Name of the new spectrumm.
   \param dataType Data type for each channel.
   \param xParameters Vector of x parameters.
   \param yParameters Vector of y parameters.
   \param xChans      number of channels on the x axis.
   \param xLow        low limit of the x axis.
   \param xHigh       high limit of the x axis.
   \param yChans      Number of channelson the y axis.
   \param yLow        Low limit of the y axis.
   \param yHigh       High limit of the y axis.

   \return CSpectrum*
   \retval Pointer to the newly created spectrum.

   \throw CSpectrumFactoryException 
   \throw string - if the number of x and y parameters is not the same.
*/
CSpectrum*
SpecTcl::Create2DSum(string name, DataType_t dataType,
		     vector<CParameter> xParameters, vector<CParameter> yParameters,
		     UInt_t xChannels, Float_t xLow, Float_t xHigh,
		     UInt_t yChannels, Float_t yLow, Float_t yHigh)
{
  CSpectrumFactory  factory;

  // Check the parameter counts an marshall the parameters into a single array:

  if (xParameters.size() != yParameters.size()) {
    throw string("SpecTcl::Create2DSum - number of x/y parameters not equal");
  }
  vector<CParameter> parameters;
  for (int i =0; i < xParameters.size(); i++) {
    parameters.push_back(xParameters[i]);
    parameters.push_back(yParameters[i]);
  }

  return            factory.Create2DMultiple(name, dataType,
					     parameters,
					     xChannels, xLow, xHigh,
					     yChannels, yLow, yHigh);
}
/*!
  Create a stripchart spectrum. Strip charts count some number of things
  as a function of time.  The user can define a parametre that is presumably
  (but not required to be) monotonically increasing, as time and another parameter
  as the number of times some event happens.  For each event that has both parameters,
  The time is converted to an x channel in the spectrum, and that channel incremented by
  the value of the other spectrum.

  \param name        - Name of the new spectrum.
  \param dataType    - Data type of the channels (determines count limits).
  \param counts      - Parameter that will have the increment count.
  \param time        - Time parameter.
  \param channels    - Number of channels on the axis.
  \param xLow        - Initial low value of the axis (the axis will shift as needed
                       to ensure that the time is in the spectrum.
  \param xHigh       - Initial high value of the axis (the axis will shift as needed 
                       to ensure the time is displayable.
 
   \return CSpectrum*
   \retval Pointer to the newly created spectrum object.
   \throw CSpectrumFactoryException
*/
CSpectrum*
SpecTcl::CreateStripChart(string name, DataType_t dataType,
			  CParameter counts, CParameter time,
			  UInt_t channels,   Float_t xLow, Float_t xHigh)
{
  CSpectrumFactory factory;
  return           factory.CreateStrip(name, dataType,
				       counts, time,
				       channels, xLow, xHigh);
}


/*!
  Adds a spectrum to the spectrum dictionary.  The spectrum
  dictionary does not manage the spectrum object.  If the spectrum
  was created via the Createxxx operations in this class, for example
  you must delete the object if it is removed from the dictionary in order
  to avoid memory leaks.
  @param spectrum    Reference to a spectrumto add to SpecTcl's spectrum database.
  
*/
void 
SpecTcl::AddSpectrum(CSpectrum& spectrum)
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  pHistogrammer->AddSpectrum(spectrum);
}


/*!
  Removes the named spectrum from the spectrum dictionary.
  @param name    Name of the spectrum to remove from the dictionary.
 
  \return CSpectrum*
  \retval pointer to removed spectrum.
  \retval  NULL No spectrum by that name to delete.
*/
CSpectrum* 
SpecTcl::RemoveSpectrum(string name)
{
  CHistogrammer*  pHistogrammer = GetHistogrammer();
  return          pHistogrammer->RemoveSpectrum(name);
}


/*!
  Locates the named spectrum in the spectrum dictionary.
  @param name    Name of the spectrum to locate.
 
  \return CSpectrum*
  \retval pointer to spectrum found.
  \retval  NULL No spectrum by that name to delete.
 
 */
CSpectrum* 
SpecTcl::FindSpectrum(string name)
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  return         pHistogrammer->FindSpectrum(name);
}
/*!
  Locates the named spectrum in the spectrum dictionary.
  @param id    id of the spectrum to find.
 
  \return CSpectrum*
  \retval pointer to spectrum found.
  \retval  NULL No spectrum by that name to delete.
 
 */
CSpectrum*
SpecTcl::FindSpectrum(UInt_t nid)
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  return pHistogrammer->FindSpectrum(nid);
}

/*!
  Returns a begin of iteration iterator into the spectrum dictionary.
*/
SpectrumDictionaryIterator 
SpecTcl::SpectrumBegin()
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  return         pHistogrammer->SpectrumBegin();
}


/*!
  Returns an end of spectrum dictinoary iterator.
 */
SpectrumDictionaryIterator 
SpecTcl::SpectrumEnd()
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  return         pHistogrammer->SpectrumEnd();
}

/*!
   Add an an observer to track changes in the spectrum 
   dictionary.  Observers get invoked on additions and deletions of
   spectra.
   \param observer : SpectrumDictionaryObserver*
      Pointer to the observer to add.
*/
void
SpecTcl::addSpectrumDictionaryObserver(SpectrumDictionaryObserver* observer)
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  pHistogrammer->addSpectrumDictionaryObserver(observer);
}
/*!
  Remove an observer from the spectrum dictionary.
  \param observer : SpectrumDictionaryObserver*
*/
void
SpecTcl::removeSpectrumDictionaryObserver(SpectrumDictionaryObserver* observer)
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  pHistogrammer->removeSpectrumDictionaryObserver(observer);

}
/*!
  Returns the number of spectra in the spectrum dictionary.
 */
UInt_t 
SpecTcl::SpectrumCount()
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  return         pHistogrammer->SpectrumCount();  
}


/*!
  Zeroes the channels of the named spectrum.
  @param name
    Name of the spectrum we want to clear.

  \throw CDictionaryException - If the spectrum named does not exist.
  
*/
void 
SpecTcl::ClearSpectrum(string name)
{
  CHistogrammer* pHistogram = GetHistogrammer();
  pHistogram->ClearSpectrum(name);
}


/*!
  Clears all spectra in the spectrum dictionary.  Note that if you
  are maintaining spectra outside the spectrum dictinoary,
  these are not known to the ClearAllSpectra operation.
*/
void 
SpecTcl::ClearAllSpectra()
{
  CHistogrammer* pHistogram = GetHistogrammer();
  return         pHistogram->ClearAllSpectra();
}


/*!
  Creates a compound gate of the specified type.  Compound gates
  are gates that are made up of other gates.  A pointer to the gate created
  is returned.
  @param gateType
    Type of gate to create. Must be a compound gate type.
  @param names
    Names of the gates that will make up this compound gate.

  \return CGate*
  \retval Pointer to the newly crated gate.  It is up to the caller to 
          enter the gate into the gate dictionary where the histogrammer can
	  operate on it.. It is also up to the caller to delete the gate
	  when done.
  \throw CGateFactoryException - If there's a problem creating the gate.
 
*/
CGate* 
SpecTcl::CreateGate(CGateFactory::GateType gateType, vector<string> names)
{
  CGateFactory factory(GetHistogrammer());
  return       factory.CreateGate(gateType, names);
}
/*!
  Creates a compound gate of the specified type.  Compound gates
  are gates that are made up of other gates.  A pointer to the gate created
  is returned.
  @param gateType
    Type of gate to create. Must be a compound gate type.
  @param names
    Names of the gates that will make up this compound gate.

  \return CGate*
  \retval Pointer to the newly crated gate.  It is up to the caller to 
          enter the gate into the gate dictionary where the histogrammer can
	  operate on it.. It is also up to the caller to delete the gate
	  when done.
  \throw CGateFactoryException - If there's a problem creating the gate.
 
*/


CGate* 
SpecTcl::CreateGate(CGateFactory::GateType gateType, vector<string> parameters, 
		    vector<FPoint> points)
{
  CGateFactory factory(GetHistogrammer());
  return       factory.CreateGate(gateType, parameters, points);
}
/*!
  Creates a mask gate of the specified type.  Compound gates
  are gates that are made up of other gates.  A pointer to the gate created
  is returned.
  @param gateType
    Type of gate to create. Must be a compound gate type.
  @param names
    Names of the gates that will make up this compound gate.

  \return CGate*
  \retval Pointer to the newly crated gate.  It is up to the caller to 
          enter the gate into the gate dictionary where the histogrammer can
	  operate on it.. It is also up to the caller to delete the gate
	  when done.
  \throw CGateFactoryException - If there's a problem creating the gate.
 
*/
CGate* 
SpecTcl::CreateGate(CGateFactory::GateType gateType, 
		    vector<string> parameters, 
		    long comparison)
{
  CGateFactory factory(GetHistogrammer());
  return       factory.CreateGate(gateType, parameters, comparison);
}

/*!
  Creates a gamma  gate.
  @param gateType
    Type of gate to add.
  @param points
    Set of points that make up the gate.
  @param parameters
    List of parameters on the spectrum on which the gate was defined.

  \return CGate*
  \retval Pointer to the newly crated gate.  It is up to the caller to 
          enter the gate into the gate dictionary where the histogrammer can
	  operate on it.. It is also up to the caller to delete the gate
	  when done.
  \throw CGateFactoryException - If there's a problem creating the gate.
  
*/
CGate*
SpecTcl::CreateGate(CGateFactory::GateType   gateType, STD(vector)<FPoint> points,
		    STD(vector)<UInt_t> parameters)
{
  CGateFactory factory(GetHistogrammer());
  return       factory.CreateGate(gateType, points, parameters);
}

/*!
  Creates a True gate and returns a pointer to it.  A TRUE gate is a gate that is
  always made.  One use of true gates is to serve as placeholders for gates that
  you will specify later in the analysis.  Placeholder gates allow you to define
  gated spectra before you know how to define the gates.

  \return CGate*
  \retval Pointer to the newly crated gate.  It is up to the caller to 
          enter the gate into the gate dictionary where the histogrammer can
	  operate on it.. It is also up to the caller to delete the gate
	  when done.
  \throw CGateFactoryException - If there's a problem creating the gate.

*/
CGate* 
SpecTcl::CreateTrueGate()
{
  CGateFactory factory(GetHistogrammer());
  return       static_cast<CGate*>(factory.CreateTrueGate());
}


/*!
  Creates a false gate and returns a pointer to it.  One use of false gates is as
  a place holder for gates you may later create.  Creating placeholder gates
  allows you to create gated spectra before you know how you will define the gate
  that conditionalizes it.  SpecTcl uses a false gate to replace gates that you
  'delete'.

  \return CGate*
  \retval Pointer to the newly crated gate.  It is up to the caller to 
          enter the gate into the gate dictionary where the histogrammer can
	  operate on it.. It is also up to the caller to delete the gate
	  when done.
  \throw CGateFactoryException - If there's a problem creating the gate.
*/
CGate* 
SpecTcl::CreateFalseGate()
{
  CGateFactory factory(GetHistogrammer());
  return       static_cast<CGate*>(factory.CreateFalseGate());
}


/*!
  Creates a band gate and returns a pointer to it.  A band gate is a polyline in
  a two dimensional parameter space.  All of the area below the polyline is
  consideredin the gate.  Area to the left of the leftmost gate point and to the
  right of the right most gate point is considered outside of the gate.   Band
  gates are capable of correctly handling pathalogical band such as those that
  are multivalued in X.
  @param xparameter    Name of the band's X parameter
  
  @param yparameter    Name of the band's y parameter.
  @param points    Points that define the band.

  \return CGate*
  \retval Pointer to the newly crated gate.  It is up to the caller to 
          enter the gate into the gate dictionary where the histogrammer can
	  operate on it.. It is also up to the caller to delete the gate
	  when done.
  \throw CGateFactoryException - If there's a problem creating the gate.
  
*/
CGate* 
SpecTcl::CreateBand(string xparameter, string yparameter, 
		    vector<FPoint> points)
{
  vector<string> names;
  names.push_back(xparameter);
  names.push_back(yparameter);

  CGateFactory factory(GetHistogrammer());
  return       static_cast<CGate*>(factory.CreateBand(names, points));
}


/*!
  Creates a contour gate and returns a pointer to it.  A counter gate defines a
  region in a 2-d space defined by two parameters.   The points in a contour
  define a closed polygon.  The contour gate is capable of handling pathalogical
  cases such as polygons with 'windings'   Insidedness is defined by the number
  of edge crossings taken by a line that extends from the point in question to
  infinity.  An odd number of crossings defines an interior while an even number
  an exterior point.
  @param xParameter
    Name of the x paramter in the contour.
  @param yParameter
    Name of the contour's y parameter.
  @param points
    Vector of points that define the shape of the contour.

  \return CGate*
  \retval Pointer to the newly crated gate.  It is up to the caller to 
          enter the gate into the gate dictionary where the histogrammer can
	  operate on it.. It is also up to the caller to delete the gate
	  when done.
  \throw CGateFactoryException - If there's a problem creating the gate.
  
*/
CGate* 
SpecTcl::CreateContour(string xParameter, string yParameter, 
		       vector<FPoint> points)
{
  vector<string> params;
  params.push_back(xParameter);
  params.push_back(yParameter);

  CGateFactory factory(GetHistogrammer());
  return       static_cast<CGate*>(factory.CreateContour(params, points));
}


/*!
  Creates a contour from a pair of bands.   A pointer is returned to the new
  contour. This is a slightly pathalogical 2-d gate that takes two band gates and
  creates a contour defined by joining each gates first and last points.  This
  sort of gate is very useful
  for some types of particle ID spectra where bands can be used to delimit
  particle groups. Having done this, alternate bands get joined to create
  contours that enclose and identify each particle group.
  @param firstBand
    Name of the first band of the contour.
  @param secondBand
    Name of the second band to turn into a contour.
 

  \return CGate*
  \retval Pointer to the newly crated gate.  It is up to the caller to 
          enter the gate into the gate dictionary where the histogrammer can
	  operate on it.. It is also up to the caller to delete the gate
	  when done.
  \throw CGateFactoryException - If there's a problem creating the gate.
*/
CGate* 
SpecTcl::CreateBandContour(string firstBand, string secondBand)
{
  vector<string> bands;
  bands.push_back(firstBand);
  bands.push_back(secondBand);

  CGateFactory factory(GetHistogrammer());
  return       static_cast<CGate*>(factory.CreateBandContour(bands));

}


/*!
  Creates a not gate and returns a pointer to it.  Not gates are defined on an
  existing gate of any type and return true when the existing gate evalutates
  false and vica-versa.
  @param name
    The name of the gate that will make up this gate.  The resulting
    gate is the inverse of the parameter.

  \return CGate*
  \retval Pointer to the newly crated gate.  It is up to the caller to 
          enter the gate into the gate dictionary where the histogrammer can
	  operate on it.. It is also up to the caller to delete the gate
	  when done.
  \throw CGateFactoryException - If there's a problem creating the gate.
  
 */
CGate* 
SpecTcl::CreateNotGate(string name)
{
  CGateFactory  factory(GetHistogrammer());
  return        static_cast<CGate*>(factory.CreateNotGate(name));
}


/*!
  Creates an and gate and returns a pointer to it.  And gates are defined on
  arbitrary list of existing gates of any types.  The gate types in an And gate
  may be heterogenous in type.  An AND gate returns true only if all of its
  constituent gates returns true.
  @param gateNames
    names of the gates that will make up the new and gate.  The
    gate will be true whenever all of the
    named gates are true

  \return CGate*
  \retval Pointer to the newly crated gate.  It is up to the caller to 
          enter the gate into the gate dictionary where the histogrammer can
	  operate on it.. It is also up to the caller to delete the gate
	  when done.
  \throw CGateFactoryException - If there's a problem creating the gate.
  
*/
CGate* 
SpecTcl::CreateAndGate(vector<string> gateNames)
{
  CGateFactory factory(GetHistogrammer());
  return       static_cast<CGate*>(factory.CreateAndGate(gateNames));
}


/*!
  Creates a new OR gate and returns a pointer to it.  An OR gate is a compound
  gate that is defined on an arbitrary list of existing gates.  The gates may be
  heterogenous in type.  An OR gate returns TRUE if any of its constituents is
  true.
  @param gateNames    Names of the gates that will make up the new OR gate.

  \return CGate*
  \retval Pointer to the newly crated gate.  It is up to the caller to 
          enter the gate into the gate dictionary where the histogrammer can
	  operate on it.. It is also up to the caller to delete the gate
	  when done.
  \throw CGateFactoryException - If there's a problem creating the gate.
  
 */
CGate* 
SpecTcl::CreateOrGate(vector<string> gateNames)
{
  CGateFactory factory(GetHistogrammer());
  return       static_cast<CGate*>(factory.CreateOrGate(gateNames));
}


/*!
  Creates a cut gate and returns a pointer to it.  A cut gate is a primitive gate
  that is defined on a single parameter.  If the parameter is between the lower
  and upper limit points of this gate, it returns True.
  @param parameter
    Name of the parameter on which the cut is defined.
  @param low
    Low level cut off of the gate.
  @param high
    High level cutoff of the gate.

  \return CGate*
  \retval Pointer to the newly crated gate.  It is up to the caller to 
          enter the gate into the gate dictionary where the histogrammer can
	  operate on it.. It is also up to the caller to delete the gate
	  when done.
  \throw CGateFactoryException - If there's a problem creating the gate.
  
  
*/
CGate* 
SpecTcl::CreateCut(string parameter, Float_t low, Float_t high)
{
  CGateFactory factory(GetHistogrammer());
  return       static_cast<CGate*>(factory.CreateCut(parameter, low, high));
}


/*!
  Creates a gamma cut and returns a pointer to it.  A gamma cut is a lower and
  upper limit on a set of parameters.  Gamma gates are true if any of the
  parameters is within the cut.  Gamma cuts are also used to create folds on one
  or 2-d gamma spectra.
  @param low
    Low Limit of the cut.
  @param high
    High limit of the cut.
  @param constituents
    Names of constituents 

  \return CGate*
  \retval Pointer to the newly crated gate.  It is up to the caller to 
          enter the gate into the gate dictionary where the histogrammer can
	  operate on it.. It is also up to the caller to delete the gate
	  when done.
  \throw CGateFactoryException - If there's a problem creating the gate.
    
*/
CGate* 
SpecTcl::CreateGammaCut(Float_t low, Float_t high, vector<string> constituents)
{
  CGateFactory factory(GetHistogrammer());
  vector<UInt_t> paramIds;
  try {
    paramIds = parameterIds(constituents);
  }
  catch (string badname) {
      string doing("Looking up gamma  cut  parameter: ");
      doing  += badname;
      throw CGateFactoryException(CGateFactoryException::NoSuchParameter,
				  CGateFactory::gammacut,
				  doing);
  }

  return       static_cast<CGate*>(factory.CreateGammaCut(low, high, paramIds));
}


/*!
  Creates a gamma band and returns a pointer to it.  A gamma band is a gate that
  is defined on a set of parameters.  If any parameter is inside the band, the
  gate returns true.  Gamma bands can also be used to create folds on Gamma
  spectra.
  @param points
    Vector of points that define the shape of the band.
  @param constituents
    The names of the constituents of the band. At present,
    these are names of spectra.

  \return CGate*
  \retval Pointer to the newly crated gate.  It is up to the caller to 
          enter the gate into the gate dictionary where the histogrammer can
	  operate on it.. It is also up to the caller to delete the gate
	  when done.
  \throw CGateFactoryException - If there's a problem creating the gate.
  
*/
CGate* 
SpecTcl::CreateGammaBand(vector<FPoint> points, 
			 vector<string> constituents)
{
  CGateFactory factory(GetHistogrammer());
  vector<UInt_t> paramIds;
  try {
    paramIds = parameterIds(constituents);
  }
  catch (string badname) {
      string doing("Looking up gamma band parameter: ");
      doing  += badname;
      throw CGateFactoryException(CGateFactoryException::NoSuchParameter,
				  CGateFactory::gammaband,
				  doing);
  }

  return       factory.CreateGammaBand(points, paramIds);
}


/*!
  Creates a gamma contour and returns a pointer to it.  A gamma contour is a
  contour that is defined on an arbitrary list of parameters.  If any ordered
  pair of parameters is inside the contour, the gate is true.  Gamma gates can
  also be used to create folds on gamma spectra.
  @param points    Points that defined the contour.
  @param constituents    Set of spectra on which the gate is defined.

  \return CGate*
  \retval Pointer to the newly crated gate.  It is up to the caller to 
          enter the gate into the gate dictionary where the histogrammer can
	  operate on it.. It is also up to the caller to delete the gate
	  when done.
  \throw CGateFactoryException - If there's a problem creating the gate.

 
*/
CGate* 
SpecTcl::CreateGammaContour(vector<FPoint> points, 
			    vector<string> constituents)
{
  CGateFactory factory(GetHistogrammer());
  vector<UInt_t> paramIds;
  try {
    paramIds = parameterIds(constituents);
  }
  catch (string badname) {
      string doing("Looking up gamma  contour parameter: ");
      doing  += badname;
      throw CGateFactoryException(CGateFactoryException::NoSuchParameter,
				  CGateFactory::gammacontour,
				  doing);
  }
  return       factory.CreateGammaContour(points, paramIds);
}

/*!
  Creates a Mask Equal Gate
  \return CGate*
  \retval Pointer to the newly created gate.  It is up to the caller to 
          enter the gate into the gate dictionary where the histogrammer can
	  operate on it.. It is also up to the caller to delete the gate
	  when done.
  \throw CGateFactoryException - If there's a problem creating the gate.

 
*/
CGate* 
SpecTcl::CreateMaskEqualGate(vector<string> parameters, 
			     long Compare)
{
  CGateFactory factory(GetHistogrammer());
  return       factory.CreateMaskEqualGate(parameters, Compare);
}


/*!
  Creates a Mask Equal Gate
  \return CGate*
  \retval Pointer to the newly created gate.  It is up to the caller to 
          enter the gate into the gate dictionary where the histogrammer can
	  operate on it.. It is also up to the caller to delete the gate
	  when done.
  \throw CGateFactoryException - If there's a problem creating the gate.

 
*/
CGate* 
SpecTcl::CreateMaskAndGate(vector<string> parameters, 
			     long Compare)
{
  CGateFactory factory(GetHistogrammer());
  return       factory.CreateMaskAndGate(parameters, Compare);
}


/*!
  Creates a Mask Equal Gate
  \return CGate*
  \retval Pointer to the newly created gate.  It is up to the caller to 
          enter the gate into the gate dictionary where the histogrammer can
	  operate on it.. It is also up to the caller to delete the gate
	  when done.
  \throw CGateFactoryException - If there's a problem creating the gate.

 
*/
CGate* 
SpecTcl::CreateMaskNotGate(vector<string> parameters, 
			     long Compare)
{
  CGateFactory factory(GetHistogrammer());
  return       factory.CreateMaskNotGate(parameters, Compare);
}

/*!
  Adds a gate to the gate dictionary.  The gate dictionary does not manage
  storage.  If the gate is removed from the gate dictionary and was dynamically
  created, the caller must delete it to prevent memory leaks.
  @param name
    Name of the new gate.
  @param gate
    Pointer to the gate to add to the gate dictionary.
  
 \throw CDictionaryException if a gate by this name already exists. 
  
*/
void 
SpecTcl::AddGate(string name, CGate* gate)
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  CGateFactory   factory(pHistogrammer);

  pHistogrammer->AddGate(name, CGateFactory::AssignId(), *gate);
}


/*!
  Deletes the named gate from the gate dictionary.   This only removes the gate
  from the  dictionary.  If the gate was dynamically created, you must delete the
  gate object.
  @param gateName
    Name of the gate to delete.
  \throw CDictionaryException - if the gate does not exist.
*/
void SpecTcl::DeleteGate(string gateName)
{
  CHistogrammer* pHistogrammer = GetHistogrammer();;
  pHistogrammer->DeleteGate(gateName);
}


/*!
  Replaces a gate within the gate dictionary.  A pointer to the original gate is
  returned.  If the original gate
  was dynamically created, you must delete this object to prevent memory leaks.
  Two typical uses of ReplaceGate are:
  - To redefine the points of an existing gate.
  - To replace a placeholder true or false gate with the actual gate once you
  know how to set it.
  
  @param gateName
    Name of the gate to replace.
  @param newGate
    Reference to the new gate that replaces the existing gate
  definition for this gate name.
  \throw CDictionaryException if the gate does not yet exist.
  
*/
void 
SpecTcl::ReplaceGate(string gateName, CGate& newGate)
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  pHistogrammer->ReplaceGate(gateName, newGate);
}


/*!
  Locates a gate in the gate dictionary.  Note that a pointer to a gate container
  is returned to the
  gate rather than a pointer to the gate itself.  Gate containers are pointer
  like objects the pointer you
  receive will point to a gate container that will continue to point to the named
  gate even if the definition of the gate itself changes.
  @param gateName
    Name of the gate to locate.
 
  \return CGateContainer*
  \retval  A pointer to the gate container that stands in for the gate you
           are looking for.
  \retval NULL - no gate by this name exists.
*/
CGateContainer* 
SpecTcl::FindGate(string gateName)
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  return pHistogrammer->FindGate(gateName);
}


/*!
  Returns a begin iterator into the gate dictionary.  GateDictionaryIterators are
  pointer like objects that point to a pair consisting of the name of the gate
  and a pointer to the GateContainer object that is an invariant pointer to the
  named gate.
  \return CGateDictionaryIterator - a pointer like value that 
              'points' to a pair<string, CGateContainer*>
  \retval The iterator for the first element in the collection.  For maps, this
          is alphabetically first.
*/
CGateDictionaryIterator 
SpecTcl::GateBegin()
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  return         pHistogrammer->GateBegin();
}

/*!
  Returns an end of iteration iterator for the gate dictionary.
  \return CGateDictionaryIterator - a pointer like value that 
              'points' to a pair<string, CGateContainer*>
  \retval The iterator for the end of the collection.  The prior
          iterator points to the alphabetically last entry in the collection.
*/
CGateDictionaryIterator 
SpecTcl::GateEnd()
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  return         pHistogrammer->GateEnd();
}


/*!
  Returns the number of gates in the gate dictionary.
  \return UInt_t number of gates in the gate dictionary.
*/
UInt_t 
SpecTcl::GateCount()
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  return         pHistogrammer->GateCount();

}
/*!
   Adds a gate observer to the list:

*/
void
SpecTcl::addGateDictionaryObserver(CGateObserver* observer)
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  pHistogrammer->addGateObserver(observer);
}

/*!
   Removes a gate observer:
*/
void
SpecTcl::removeGateDictionaryObserver(CGateObserver* observer)
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  pHistogrammer->removeGateObserver(observer);
}

/*!
  Applies the specified gate to the specified histogram.  Once a gate is applied
  to a histogram it is only incremented if the gate is true for the event.
  @param gateName
    Name of the gate to apply.
  @param spectrumName
    Name of the spectrum to which to apply the gate.
  
  \throw CDictionaryException if the gate or spectrum do not exist.
 
*/
void 
SpecTcl::ApplyGate(string gateName, string spectrumName)
{
  CHistogrammer* pHistogrammer = GetHistogrammer();
  pHistogrammer->ApplyGate(gateName, spectrumName);
}


/*!
  Adds an event processor to the end of the event processor pipeline.  The event
  processor pipeline is an ordered set of objects that are invoked to transform
  events into parameters, which are then passed to the event sink pipeline.  If
  provided, the name can identify the pipeline element at a later time.  If not
  provided, a unique name is created for the event processor.  Unique names are
  of the form __EventProcessor_nnn
  where nnn is a serial number.  In the unlikely event you create an event
  processor name with that form, SpecTcl will just skip that serial number and
  locate an unused serial number.
  @param eventProcessor
    Reference to the new event processor to add.
  @param name
    Name of the event processor to add.
  
 */
void 
SpecTcl::AddEventProcessor(CEventProcessor& eventProcessor, const char* name)
{
  CTclAnalyzer* pAnalyzer = GetAnalyzer();
  pAnalyzer->AddEventProcessor(eventProcessor, name);
}


/*!
 Locates an event processor in the pipeline.  EventProcessorIterator is a
  pointer like object that 'points' to a pair consisting of the name of the event
  processor and a pointer to the event processor. If this event procesor does not
  exist, you will receive the same value returned by ProcessingPipelineEnd().
  @param name
    Name of the event processor.
  
   \return EventProcessorIterator - this is a pointer like object to a 
                                     pair<string, CEventProcesor*>
   \retval 'Pointer' to pair whose string element matches the name parameter.
   \retval EventProcessorPipelineEnd()  - If the processor could not be found.
 */
CTclAnalyzer::EventProcessorIterator 
SpecTcl::FindEventProcessor(string name)
{
  CTclAnalyzer* pAnalyzer = GetAnalyzer();
  return pAnalyzer->FindEventProcessor(name);
}


/*!
  Returns an iterator into the event processing pipeline given that you hold a
  pointer/reference to the event processor already.  If you are managing the
  event processor, often you already know where it is, but need to get an
  iterator to it.
  @param processor
    Reference to the processor to locate.
  
   \return EventProcessorIterator - this is a pointer like object to a 
                                     pair<string, CEventProcesor*>
   \retval 'Pointer' to pair whose CEventProcessor* element has the same address
                     as the processor parameter.
   \retval EventProcessorPipelineEnd()  - If the processor could not be found.  
*/
CTclAnalyzer::EventProcessorIterator 
SpecTcl::FindEventProcessor(CEventProcessor& processor)
{
  CTclAnalyzer* pAnalyzer  = GetAnalyzer();
  return        pAnalyzer->FindEventProcessor(processor);
}


/*!
  Inserts an event processor in the pipeline at an arbitrary position.  The event
  processor is added to the pipeline just prior to the processor 'pointed to' by
  the iterator.  
  @param processor
    The new ewvent processor to add.
  @param where
    The new event processor is inserted just prior to the position
    described by this iterator.
  @param name
    Name of the new event processor.
  
*/
void 
SpecTcl::InsertEventProcessor(CEventProcessor& processor, 
			      CTclAnalyzer::EventProcessorIterator where, 
			      const char*  name)
{
  CTclAnalyzer* pAnalyzer = GetAnalyzer();
  pAnalyzer->InsertEventProcessor(processor, where, name);
}


/*!
  Removes the event processor 'pointed to' by the iterator.  Removing the end
  iterator is a no-op.
  @param here
    'Points' to the event processor to remove.
  
*/
void 
SpecTcl::RemoveEventProcessor(CTclAnalyzer::EventProcessorIterator here)
{
  CTclAnalyzer* pAnalyzer = GetAnalyzer();
  pAnalyzer->RemoveEventProcessor(here);
}

/*!
   Remove an event processor given its name
   \param name 
       Name of the procesor to remove.
*/
void
SpecTcl::RemoveEventProcessor(string name)
{
  CTclAnalyzer* pAnalyzer = GetAnalyzer();
  pAnalyzer->RemoveEventProcessor(name);

}
/*!
  Returns the number of elements in the event processing pipeline.
*/
UInt_t 
SpecTcl::ProcessingPipelineSize()
{
  CTclAnalyzer* pAnalyzer = GetAnalyzer();
  return pAnalyzer->size();

}


/*!
  Returns a begin iterator into the event processing pipeline.
  \return EventProcessorIterator - 'pointer' like object to a 
            pair<string, CEventProcessor*>
  \retval  The iterator 'points' to the first stage of the pipeline. 
          
*/
CTclAnalyzer::EventProcessorIterator 
SpecTcl::ProcessingPipelineBegin()
{
  CTclAnalyzer*  pAnalyzer = GetAnalyzer();

  return pAnalyzer->begin();

}


/*!
  Returns an end of iteration iterator to the event procesing pipeline.
  \return EventProcessorIterator - 'pointer' like object to a 
            pair<string, CEventProcessor*>
  \retval  The iterator 'points' just past the last stage in the pipeline.

*/
CTclAnalyzer::EventProcessorIterator 
SpecTcl::ProcessingPipelineEnd()
{
  CTclAnalyzer* pAnalyzer = GetAnalyzer();
  return pAnalyzer->end();
}


/*!
  Adds a new sread/swrite spectrum formatter. Spectrum formatters extend the
  ability of SpecTcl to read and write spectrum formats that are not built in to
  the program.  This registration not only makes the format available to sread an
  swrite, but also adds it's one-line documentation to the usage help that swrite
  provides.
  @param name
    Name of the new spectrum formatter.
  @param formatter
    Reference to the spectrum I/O formatter to register.
  
*/
void 
SpecTcl::AddSpectrumFormatter(string name, CSpectrumFormatter& formatter)
{
  CSpectrumFormatterFactory::AddFormatter(name, &formatter);
}


/*!
  Adds an event sink to the event sink pipeline.  Each event sink in the pipeline
  must have a unique name.  If the user does not supply a name, a unique name of
  the form __EventSink_nnn is created, where nnn is an integer 'uniquifier'.  If,
  by chance the user has already defined an __EventSink_nnn the name assigner
  will keep trying serial numbers until an unused one is located.
  @param sink
    Reference to the event sink to append to the pipeline.
  @param name
    Pointer to the name of the new event sink.
  
*/
void 
SpecTcl::AddEventSink(CEventSink& sink, const char* name)
{
  CEventSinkPipeline* pPipeline = GetEventSinkPipeline();
  pPipeline->AddEventSink(sink, name);
}


/*!
  Locates an event sink in the event sink pipeline by name.  If the event sink is
  not found,
  the value returned by EventSinkdPipelineEnd() is returned.   The event sink
  iterator is
  a 'pointer' to a pair consiting of the event sink's name in the pipeline and a
  pointer to the
  event sink itself.  Note that the histogrammer is inserted in to the pipeline
  with the name
  "Histogrammer".
  @param sinkName
    Name of the event sink pipeline to locate.
  
  \return EventSinkIterator A pointer like object that 'points' to a 
       pair<string, CEventSink*>
  \retval An iterator to a pair whose string component is sinkName.
  \retval EventSinkPipelineEnd - If there is no matching pair.
 
 */
CEventSinkPipeline::EventSinkIterator 
SpecTcl::FindEventSink(string sinkName)
{
  CEventSinkPipeline*  pPipeline = GetEventSinkPipeline();
  return pPipeline->FindSink(sinkName);
}


/*!
  Given that you already have a pointer or reference to an event sink in the
  pipeline, returns the event sink iterator that 'points to it'.   If there is no
  matching event sink in the pipeline, returns the value returned by
  EventSinkPipelineEnd().
  @param sink
    Reference to the event sink we are looking for.

  \return CEventSinkPipeline::EventSinkIterator A pointer like object that 'points' to a 
       pair<string, CEventSink*>
  \retval An iterator to a pair whose CEventSink* has the same address as 'sink'.
  \retval EventSinkPipelineEnd - If there is no matching pair.
   
*/
CEventSinkPipeline::EventSinkIterator 
SpecTcl::FindEventSink(CEventSink& sink)
{
  CEventSinkPipeline* pPipeline = GetEventSinkPipeline();
  return              pPipeline->FindSink(sink);
}


/*!
  Inserts the event sink just prior to the event sink that is 'pointed to' by the
  iterator.
  @param sink
    Reference to the  new sink to add.
  @param here
    The event sink is inserted just prior to the element 'pointed
    to' by here.
  @param name
    The name to associate with the event sink.
  
*/
void 
SpecTcl::InsertEventSink(CEventSink& sink, CEventSinkPipeline::EventSinkIterator here,
			 const char* name)
{
  CEventSinkPipeline* pPipeline = GetEventSinkPipeline();
  pPipeline->InsertSink(sink, here, name);
}

/*!
   Removes an event sink from the event sink pipeline by name.
   @param name
      The name of the event sink to remove.
   \return CEventSink*  Pointer to an event sink.
   \retval  Pointer to the removed event sink.
   \retval  NULL if there was no matching event sink.
*/
CEventSink*
SpecTcl::RemoveEventSink(string name)
{
  CEventSinkPipeline* pPipeline = GetEventSinkPipeline();
  return              pPipeline->RemoveEventSink(name);
}

/*!
  Removes the selected event sink from the event sink pipeline and returns a
  pointer to it.
  @param here
    'points' to the event sink to remove from the pipeline.

   \return CEventSink*  Pointer to an event sink.
   \retval  Pointer to the removed event sink.

   \note since we have an iterator, there can be no failures that don't result
         in program death (e.g. an unitialized iterator).
*/
CEventSink* 
SpecTcl::RemoveEventSink(CEventSinkPipeline::EventSinkIterator here)
{
  CEventSink* pSink = here->second;
  CEventSinkPipeline* pPipeline = GetEventSinkPipeline();
  pPipeline->RemoveEventSink(here);

  return pSink;
}


/*!
  Returns the number of sinks in the event sink pipeline.
  \return UInt_t
  \retval Number of elements in the pipeline.
 */
UInt_t 
SpecTcl::EventSinkPipelineSize()
{
  CEventSinkPipeline* pPipeline = GetEventSinkPipeline();
  return pPipeline->size();
}


/*!
  Returns a begin iteration iterator for the event sink pipeline
 
*/
CEventSinkPipeline::EventSinkIterator 
SpecTcl::EventSinkPipelineBegin()
{
  CEventSinkPipeline* pPipeline = GetEventSinkPipeline();
  return pPipeline->begin();
}


/*!
  Returns an end of iteration iterator to the event sink pipeline.

 */
CEventSinkPipeline::EventSinkIterator 
SpecTcl::EventSinkPipelineEnd()
{
  CEventSinkPipeline* pPipeline = GetEventSinkPipeline();
  return pPipeline->end();
}


/*!
    Add a new filter to the filter dictionary.  The filter becomes
    the property of the filter dictionary and cannot be destroyed until
    it is removed from the filter dictionary by command or by
    deleteFilter. The filter should be dynamically created in order to allow
    filter -delete to work correctly.

    \param name     - Name of the event filter.
    \param pFilter  - pointer to the filter to add.

    \throw CDictionaryException - if the filter already exists.

*/
void 
SpecTcl::createFilter(string name, CGatedEventFilter* pFilter)
{
  CFilterDictionary* pDict = CFilterDictionary::GetInstance();
  if (findFilter(name)) {
    throw CDictionaryException(static_cast<Int_t>(CDictionaryException::knDuplicateKey),
			       "SpecTcl::createFilter - making a new filter",
			       name.c_str());
  }
  pDict->Enter(name, pFilter);

}
/*!
  Find and return a pointer to a filter given its name.  This allow syou to manipulate
  filters by name.

  \param name  - Name of the filter.
  \return CGatedEventFilter*
  \retval NULL - no filter found.
  \retval other - Pointer to the filter. 
*/
CGatedEventFilter*
SpecTcl::findFilter(string name)
{
  CFilterDictionary* pDict    = CFilterDictionary::GetInstance();
  CFilterDictionaryIterator p = pDict->Lookup(name);
  if (p == pDict->end()) {
    return reinterpret_cast<CGatedEventFilter*>(NULL);
  } 
  else {
    return p->second;
  }
}
/*!
   Determines if a specific filter, by pointer, exists in the dictionary.
   In means I can find an entry for which the filter object has the same
   address as pFilter.

   \param pFilter  - Pointer to the filter to fine.
   \return bool
   \retval true     - pFilter is in the dictionary.
   \retval false    - pFilter is not in the dictionary.
*/
bool
SpecTcl::filterExists(CGatedEventFilter* pFilter)
{
  CFilterDictionary* pDict   = CFilterDictionary::GetInstance();
  CFilterDictionaryIterator p= pDict->begin();
  while(p != pDict->end()) {
    if (p->second == pFilter) {
      return true;
    }
    p++;
  }
  return false;
}
/*!
     Given a pointer to a filter, delete it from the dictionary.
     this is a no-op if the filter does not exist.
     \param pFilter - Pointer to the filter to remvoe.
*/
void
SpecTcl::deleteFilter(CGatedEventFilter* pFilter)
{
  CFilterDictionary* pDict  = CFilterDictionary::GetInstance();
  CFilterDictionaryIterator p= pDict->begin();
  while( p != pDict->end()) {
    if (p->second == pFilter) {
      pDict->Remove(p->first);
      return;
    }
  }
}

/*!
   Remove a filter by name from the dictionary.
   if the filter does not exist, this is a silent no-op.
   \param name - name of the filter to delete.

   \note The storage is not deleted, only the dictionary entry.
         It is up to the caller to decide if the filter should
	 be deleted, in which case, typical code will be:

\verbatim
   CGatedEventFileter* pFilter = api.findFilter(name);
   if (pFilter) {
      api.deleteFilter(name);
      delete pFilter;
   }
\endverabtim

*/
void 
SpecTcl::deleteFilter(string filterName)
{
  CFilterDictionary* pDict   = CFilterDictionary::GetInstance();
  pDict->Remove(filterName);
}
/*!
   Add an output filter format to the set understood by 
   the filter subsystem.  This is done by adding a creator
   which can be consulted by the filter output stage factory
   when a filter needs to have a specific output format.

   \param creator  - The creator for the format we want to support.

   \note If a format by this name is already supported, then 
         this creator is registered, but will never be consulted because
         the previously registered creator will be consulted first.

    \note A clone of the creator you are registering will be created so you
          can dispose of the parameter as you desire in the caller.
*/
void
SpecTcl::addFilterOutputFormat(CFilterOutputStageCreator& creator)
{
  CFilterOutputStageFactory& fact(CFilterOutputStageFactory::getInstance());
  fact.Register(creator);
}

/*!
   Get a pointer to the TCLinterpreter.
*/
CTCLInterpreter*
SpecTcl::getInterpreter()
{
  return gpInterpreter;
}


/*!
  Returns a pointer to the histogrammer (utility function).
 */
CHistogrammer* 
SpecTcl::GetHistogrammer()
{
  return dynamic_cast<CHistogrammer*>(gpEventSink);
}


/**
 * Locate and returna pointer to the TCL Analyzer.
 */
CTclAnalyzer* SpecTcl::GetAnalyzer()
{

  // Use dynamic casts to ensure that we have the right type.
  // If not an exception gets thrown.

  return dynamic_cast<CTclAnalyzer*>(gpAnalyzer);

}


/*!
  Locates and returns a pointer to the event sink pipeline.
 */
CEventSinkPipeline* SpecTcl::GetEventSinkPipeline()
{
  return gpEventSinkPipeline;
}


/*
   Returns a vector of parameter ids for a list of parameter
   names.  Throws a string exception of the name of the failing parameter
   if the lookup failed.
*/
vector<UInt_t>
SpecTcl::parameterIds(vector<string> names)
{
  // Need to build the vector of parameter ids:
  vector<UInt_t> paramIds;
  vector<string>::iterator i = names.begin();
  while(i != names.end()) {
    CParameter* pParameter = FindParameter(*i);
    if(!pParameter) {
      throw *i;
    }
    paramIds.push_back(pParameter->getNumber());
  }
  
}
