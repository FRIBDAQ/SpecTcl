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

///////////////////////////////////////////////////////////
//  SpecTcl.h
//  Implementation of the Class SpecTcl
//  Created on:      08-Mar-2005 09:57:35 AM
//  Original author: Ron Fox
///////////////////////////////////////////////////////////

#if !defined(SPECTCL_API_H)
#define SPECTCL_API_H



// Headers required:

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

#ifndef __EVENTPROCESSOR_H
#include <EventProcessor.h>
#endif

#ifndef __EVENTSINK_H
#include <EventSink.h>
#endif

#ifndef __POINT_H
#include <Point.h>
#endif



#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif

#ifndef __HISTOGRAMMER_H
#include <Histogrammer.h>
#endif

#ifndef __GATEFACTORY_H
#include <GateFactory.h>
#endif

#ifndef __TCLANALYZER_H
#include <TCLAnalyzer.h>
#endif

#ifndef __EVENTSINKPIPELINE
#include <EventSinkPipeline.h>
#endif


#ifndef __ATTACHCOMMAND_H
#include <AttachCommand.h>
#endif


// Forward class definitions

class CParameter;
class CParameterDictionaryIterator;
class CSpectrum;
class CGate;
class CGateContainer;
class CEventProcessor;
class CSpectrumFormatter;
class CEventSink;
class CTclAnalyzer;
class CEventSinkPipeline;
class CTCLInterpreter;

class CFilterOutputStageCreator;
class CGatedEventFilter;


/*!
  Top level class that provides the user's application programming interface to
  SpecTcl.
  Note that this is a singleton patterned object and therefore has private
  constructors.
  Clients must use the getInstance() member to get a pointer to the unique
  instance of the
  object.
  @author Ron Fox
  @version 1.0
  @updated 08-Mar-2005 03:32:26 PM

*/
class SpecTcl
{
  // Class /object data:

private:
  static SpecTcl* m_pInstance;

  // Constructors and other canonicals

private:
  SpecTcl();			// Private to ensure singleton.
  virtual ~SpecTcl();

  // Below are unimplemented and therefore prohibited.

  SpecTcl(const SpecTcl& rhs);
  SpecTcl& operator=(const SpecTcl& rhs);
  int operator==(const SpecTcl& rhs) const;
  int operator!=(const SpecTcl& rhs) const;
  
public:
  //! Call this to get the singleton  instance of the SpecTcl API class

  static SpecTcl* getInstance();


  // Allow the definition of a new buffer decoder.
  // This requires that the analyzer is already defined.

  void addBufferDecoder(std::string                      type,
			CAttachCommand::CDecoderCreator* creator);


  // Manipulate the parameter diectionary etc.

  UInt_t AssignParameterId();
  CParameter* AddParameter(STD(string) name, 
			   UInt_t Id, 
			   STD(string) Units);
  CParameter* AddParameter(STD(string) name, 
			   UInt_t id, 
			   UInt_t scale);
  CParameter* AddParameter(STD(string) name, 
			   UInt_t id, 
			   UInt_t scale, 
			   Float_t low, Float_t high, 
			   STD(string) units);
  CParameter* RemoveParameter(STD(string) name);
  CParameter* FindParameter(STD(string) name);
  CParameter* FindParameter(UInt_t Id);
  ParameterDictionaryIterator BeginParameters();
  ParameterDictionaryIterator EndParameters();
  UInt_t ParameterCount();


  // Create spectra.

  CSpectrum* CreateSpectrum(STD(string) Name, 
			    SpectrumType_t type, 
			    DataType_t dataType, 
			    STD(vector)<STD(string)> parameters, 
			    STD(vector)<UInt_t> channels, 
			    STD(vector)<Float_t>* pLows, 
			    STD(vector)<Float_t>* pHighs);
  CSpectrum* CreateSpectrum(STD(string) Name,
			    SpectrumType_t type,
			    DataType_t     dataType,
			    STD(vector)<STD(string)> xParameters,
			    STD(vector)<STD(string)> yParameters,
			    STD(vector)<UInt_t>      channels,
			    STD(vector)<Float_t>*    pLows,
			    STD(vector)<Float_t>*    pHighs);

  CSpectrum* CreateSpectrum(std::string           Name,
			    SpectrumType_t        type,
			    DataType_t            dataType,
			    std::vector<std::vector<std::string> > parameters,
			    std::vector<UInt_t>   channels,
			    std::vector<Float_t>* lows,
			    std::vector<Float_t>* highs);

  CSpectrum* CreateGammaSummary(std::string                      Name,
				DataType_t                       dataType,
				std::vector<std::vector<std::string> > parameters,
				UInt_t                           nChannels,
				std::vector<Float_t>*            low,
				std::vector<Float_t>*            high);

  CSpectrum* CreateG2DDeluxe(STD(string) Name,
			DataType_t     dataType,
			STD(vector)<STD(string)> xParameters,
			STD(vector)<STD(string)> yParameters,
			STD(vector)<UInt_t>      channels,
			STD(vector)<Float_t>*    pLows,
			STD(vector)<Float_t>*    pHighs);

  CSpectrum* Create1D(STD(string) name, 
		      DataType_t dataType, 
		      CParameter& parameter, 
		      UInt_t channels);
  CSpectrum* Create1D(STD(string) name, 
		      DataType_t dataType, 
		      CParameter& parameter, 
		      UInt_t channels, 
		      Float_t lowLimit, Float_t hiLimit);
  CSpectrum* Create2D(STD(string) name, 
		      DataType_t dataType, 
		      CParameter& xParameter, 
		      CParameter& yParmaeter, 
		      UInt_t xChannels, UInt_t yChannels);
  CSpectrum* Create2D(STD(string) name, 
		      DataType_t dataType, 
		      CParameter& xParameter, CParameter& yParameter, 
		      UInt_t xChannels, Float_t xLow, Float_t xHigh, 
		      UInt_t yChannels, Float_t yLow, Float_t yHigh);
  CSpectrum* CreateG1D(STD(string) name, 
		       DataType_t dataType,
		       STD(vector)<CParameter> parameters, 
		       UInt_t channels);
  CSpectrum* CreateG1D(STD(string) name, 
		       DataType_t dataType, 
		       STD(vector)<CParameter> parameters, 
		       UInt_t channels, 
		       Float_t lowLimit, 
		       Float_t hiLimit);
  CSpectrum* CreateG2D(STD(string) name, 
		       DataType_t dataType, 
		       STD(vector)<CParameter> parameters, 
		       UInt_t xChannels, UInt_t yChannels);
  CSpectrum* CreateG2D(STD(string) name, 
		       DataType_t dataType, 
		       STD(vector)<CParameter> parameters, 
		       UInt_t xChannesl, Float_t xLow, Float_t xHigh, 
		       UInt_t yChannesl, Float_t yLow, Float_t yHigh);
  CSpectrum* CreateBit(STD(string) name, 
		       DataType_t dataType, 
		       CParameter& parameter, 
		       UInt_t channels);
  CSpectrum* CreateBit(STD(string) name, 
		       DataType_t dataType, 
		       CParameter& parameter, 
		       UInt_t channels, UInt_t lowBit);
  CSpectrum* CreateSummary(STD(string) name, 
			   DataType_t dataType, 
			   STD(vector)<CParameter> parameters, 
			   UInt_t channels);
  CSpectrum* CreateSummary(STD(string) name, 
			   DataType_t dataType, 
			   STD(vector)<CParameter> parameters, 
			   UInt_t nChannels, Float_t low, Float_t high);

  
  CSpectrum* CreateGamma2DD(STD(string) name,
			    DataType_t dataType,
			    STD(vector)<CParameter> xParameters,
			    STD(vector)<CParameter> yParameters,
			    UInt_t xChannels, Float_t xLow, Float_t xHigh,
			    UInt_t yChannels, Float_t yLow, Float_t yHigh);
  CSpectrum* Create2DSum(STD(string) name,
			 DataType_t  dataType,
			 STD(vector)<CParameter> xParameters,
			 STD(vector)<CParameter> yParameters,
			 UInt_t xChans, Float_t xLow, Float_t xHigh,
			 UInt_t yChans, Float_t yLow, Float_t yHigh);
  CSpectrum* CreateStripChart(STD(string) name,
			      DataType_t  dataType,
			      CParameter  counts,
			      CParameter  time,
			      UInt_t      channels, Float_t xLow, Float_t xHigh);
			 

  // Manipulate the spectrum dictionary:

  void AddSpectrum(CSpectrum& spectrum);
  CSpectrum* RemoveSpectrum(STD(string) name);
  CSpectrum* FindSpectrum(STD(string) name);
  CSpectrum* FindSpectrum(UInt_t id);
  SpectrumDictionaryIterator SpectrumBegin();
  SpectrumDictionaryIterator SpectrumEnd();
  UInt_t SpectrumCount();

  void addSpectrumDictionaryObserver(SpectrumDictionaryObserver* observer);
  void removeSpectrumDictionaryObserver(SpectrumDictionaryObserver* observer);

  // Misc spectrum utilities:

  void ClearSpectrum(STD(string) name);
  void ClearAllSpectra();

  // Create gates of various types:

  CGate* CreateGate(CGateFactory::GateType gateType, 
		    STD(vector)<STD(string)> names);
  CGate* CreateGate(CGateFactory::GateType gateType, 
		    STD(vector)<STD(string)> parameters, 
		    STD(vector)<FPoint> points);
  CGate* CreateGate(CGateFactory::GateType   gateType,
		    STD(vector)<FPoint>      points,
		    STD(vector)<UInt_t> parameters);
  CGate* CreateGate(CGateFactory::GateType gateType, 
		    STD(vector)<STD(string)> rparameters,
		    long comparison);
  CGate* CreateTrueGate();
  CGate* CreateFalseGate();
  CGate* CreateBand(STD(string) xparameter, STD(string) yparameter, 
		    STD(vector)<FPoint> points);
  CGate* CreateContour(STD(string) xParameter, STD(string) yParameter, 
		       STD(vector)<FPoint> points);
  CGate* CreateBandContour(STD(string) firstBand, STD(string) secondBand);
  CGate* CreateNotGate(STD(string) name);
  CGate* CreateAndGate(STD(vector)<STD(string)> gateNames);
  CGate* CreateOrGate(STD(vector)<STD(string)> gateNames);
  CGate* CreateCut(STD(string) parameter, Float_t low, Float_t high);
  CGate* CreateGammaCut(Float_t low, Float_t high, STD(vector)<STD(string)> constituents);
  CGate* CreateGammaBand(STD(vector)<FPoint> points, 
			 STD(vector)<STD(string)> constituents);
  CGate* CreateGammaContour(STD(vector)<FPoint> points, 
			    STD(vector)<STD(string)> constituents);
  CGate* CreateMaskEqualGate(STD(vector)<STD(string)> rParameterName,
			     long Compare);
  CGate* CreateMaskAndGate(STD(vector)<STD(string)> rParameterName,
			     long Compare);
  CGate* CreateMaskNotGate(STD(vector)<STD(string)> rParameterName,
			     long Compare);


  // Gate dictionary manipulation:

  void AddGate(STD(string) name, CGate* gate);
  void DeleteGate(STD(string) gateName);
  void ReplaceGate(STD(string) gateName, CGate& newGate);
  CGateContainer* FindGate(STD(string) gateName);
  CGateDictionaryIterator GateBegin();
  CGateDictionaryIterator GateEnd();
  UInt_t GateCount();

  void addGateDictionaryObserver(CGateObserver* observer);
  void removeGateDictionaryObserver(CGateObserver* observer);
  


  void ApplyGate(STD(string) gateName, STD(string) spectrumName);

  // Manipulating the event processor pipeline.

  void AddEventProcessor(CEventProcessor& eventProcessor, 
			 const char* name = 0);
  CTclAnalyzer::EventProcessorIterator FindEventProcessor(STD(string) name);
  CTclAnalyzer::EventProcessorIterator FindEventProcessor(CEventProcessor& processor);
  void InsertEventProcessor(CEventProcessor& processor,
			    CTclAnalyzer::EventProcessorIterator where, 
			    const char*  name = 0);
  void RemoveEventProcessor(STD(string) name);
  void RemoveEventProcessor(CTclAnalyzer::EventProcessorIterator here);
  UInt_t ProcessingPipelineSize();
  CTclAnalyzer::EventProcessorIterator ProcessingPipelineBegin();
  CTclAnalyzer::EventProcessorIterator ProcessingPipelineEnd();

  // Spectrum I/O formatting control:

  void AddSpectrumFormatter(STD(string) name, CSpectrumFormatter& formatter);


  // Event sink pipeline control:

  void AddEventSink(CEventSink& sink, const char* name = 0);
  CEventSinkPipeline::EventSinkIterator FindEventSink(STD(string) sinkName);
  CEventSinkPipeline::EventSinkIterator FindEventSink(CEventSink& sink);
  void InsertEventSink(CEventSink& sink, CEventSinkPipeline::EventSinkIterator here,
		       const char* name = 0);
  CEventSink* RemoveEventSink(STD(string) name);
  CEventSink* RemoveEventSink(CEventSinkPipeline::EventSinkIterator here);
  UInt_t EventSinkPipelineSize();
  CEventSinkPipeline::EventSinkIterator EventSinkPipelineBegin();
  CEventSinkPipeline::EventSinkIterator EventSinkPipelineEnd();



  // Filter API:

  void               createFilter(STD(string) name, CGatedEventFilter* pFilter);
  CGatedEventFilter* findFilter(STD(string) name);
  bool               filterExists(CGatedEventFilter* pFilter);
  void               deleteFilter(CGatedEventFilter* pFilter);
  void               deleteFilter(STD(string) filterName);
  void               addFilterOutputFormat(CFilterOutputStageCreator& creator);
  

  // Expose the guts for the really curious and those who need more
  // than we can offer.

  CTCLInterpreter*    getInterpreter();
  CHistogrammer*      GetHistogrammer();
  CTclAnalyzer*       GetAnalyzer();
  CEventSinkPipeline* GetEventSinkPipeline();
  STD(vector)<UInt_t>      parameterIds(STD(vector)<STD(string)> names);

  

  
};


#endif 
