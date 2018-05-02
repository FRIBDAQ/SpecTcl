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

#include <histotypes.h>
#include <EventProcessor.h>
#include <EventSink.h>
#include <Point.h>
#include <string>

#include <vector>
#include <Histogrammer.h>
#include <GateFactory.h>
#include <TCLAnalyzer.h>
#include <EventSinkPipeline.h>
#include <AttachCommand.h>

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
class CDisplayInterface;

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
  CParameter* AddParameter(std::string name, 
			   UInt_t Id, 
			   std::string Units);
  CParameter* AddParameter(std::string name, 
			   UInt_t id, 
			   UInt_t scale);
  CParameter* AddParameter(std::string name, 
			   UInt_t id, 
			   UInt_t scale, 
			   Float_t low, Float_t high, 
			   std::string units);
  CParameter* RemoveParameter(std::string name);
  CParameter* FindParameter(std::string name);
  CParameter* FindParameter(UInt_t Id);
  ParameterDictionaryIterator BeginParameters();
  ParameterDictionaryIterator EndParameters();
  UInt_t ParameterCount();


  // Create spectra.

  CSpectrum* CreateSpectrum(std::string Name, 
			    SpectrumType_t type, 
			    DataType_t dataType, 
			    std::vector<std::string> parameters, 
			    std::vector<UInt_t> channels, 
			    std::vector<Float_t>* pLows, 
			    std::vector<Float_t>* pHighs);
  CSpectrum* CreateSpectrum(std::string Name,
			    SpectrumType_t type,
			    DataType_t     dataType,
			    std::vector<std::string> xParameters,
			    std::vector<std::string> yParameters,
			    std::vector<UInt_t>      channels,
			    std::vector<Float_t>*    pLows,
			    std::vector<Float_t>*    pHighs);

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

  CSpectrum* CreateG2DDeluxe(std::string Name,
			DataType_t     dataType,
			std::vector<std::string> xParameters,
			std::vector<std::string> yParameters,
			std::vector<UInt_t>      channels,
			std::vector<Float_t>*    pLows,
			std::vector<Float_t>*    pHighs);

  CSpectrum* Create1D(std::string name, 
		      DataType_t dataType, 
		      CParameter& parameter, 
		      UInt_t channels);
  CSpectrum* Create1D(std::string name, 
		      DataType_t dataType, 
		      CParameter& parameter, 
		      UInt_t channels, 
		      Float_t lowLimit, Float_t hiLimit);
  CSpectrum* Create2D(std::string name, 
		      DataType_t dataType, 
		      CParameter& xParameter, 
		      CParameter& yParmaeter, 
		      UInt_t xChannels, UInt_t yChannels);
  CSpectrum* Create2D(std::string name, 
		      DataType_t dataType, 
		      CParameter& xParameter, CParameter& yParameter, 
		      UInt_t xChannels, Float_t xLow, Float_t xHigh, 
		      UInt_t yChannels, Float_t yLow, Float_t yHigh);
  CSpectrum* CreateG1D(std::string name, 
		       DataType_t dataType,
		       std::vector<CParameter> parameters, 
		       UInt_t channels);
  CSpectrum* CreateG1D(std::string name, 
		       DataType_t dataType, 
		       std::vector<CParameter> parameters, 
		       UInt_t channels, 
		       Float_t lowLimit, 
		       Float_t hiLimit);
  CSpectrum* CreateG2D(std::string name, 
		       DataType_t dataType, 
		       std::vector<CParameter> parameters, 
		       UInt_t xChannels, UInt_t yChannels);
  CSpectrum* CreateG2D(std::string name, 
		       DataType_t dataType, 
		       std::vector<CParameter> parameters, 
		       UInt_t xChannesl, Float_t xLow, Float_t xHigh, 
		       UInt_t yChannesl, Float_t yLow, Float_t yHigh);
  CSpectrum* CreateBit(std::string name, 
		       DataType_t dataType, 
		       CParameter& parameter, 
		       UInt_t channels);
  CSpectrum* CreateBit(std::string name, 
		       DataType_t dataType, 
		       CParameter& parameter, 
		       UInt_t channels, UInt_t lowBit);
  CSpectrum* CreateSummary(std::string name, 
			   DataType_t dataType, 
			   std::vector<CParameter> parameters, 
			   UInt_t channels);
  CSpectrum* CreateSummary(std::string name, 
			   DataType_t dataType, 
			   std::vector<CParameter> parameters, 
			   UInt_t nChannels, Float_t low, Float_t high);

  
  CSpectrum* CreateGamma2DD(std::string name,
			    DataType_t dataType,
			    std::vector<CParameter> xParameters,
			    std::vector<CParameter> yParameters,
			    UInt_t xChannels, Float_t xLow, Float_t xHigh,
			    UInt_t yChannels, Float_t yLow, Float_t yHigh);
  CSpectrum* Create2DSum(std::string name,
			 DataType_t  dataType,
			 std::vector<CParameter> xParameters,
			 std::vector<CParameter> yParameters,
			 UInt_t xChans, Float_t xLow, Float_t xHigh,
			 UInt_t yChans, Float_t yLow, Float_t yHigh);
  CSpectrum* CreateStripChart(std::string name,
			      DataType_t  dataType,
			      CParameter  counts,
			      CParameter  time,
			      UInt_t      channels, Float_t xLow, Float_t xHigh);
    CSpectrum* CreateM2Projection(
        std::string name, DataType_t dataType,
        const std::vector<CParameter>& parameters, CGateContainer* roi,
        Bool_t xproj, UInt_t nChannels, Float_t low, Float_t high
    );
    CSpectrum* CreateM2Projection(
      std::string name, DataType_t dataType,
        const std::vector<CParameter>& parameters,
        const std::vector<CGateContainer*>& roi,
        Bool_t xproj, UInt_t nChannels, Float_t low, Float_t high
    );
    

  // Manipulate the spectrum dictionary:

  void AddSpectrum(CSpectrum& spectrum);
  CSpectrum* RemoveSpectrum(std::string name);
  CSpectrum* FindSpectrum(std::string name);
  CSpectrum* FindSpectrum(UInt_t id);
  SpectrumDictionaryIterator SpectrumBegin();
  SpectrumDictionaryIterator SpectrumEnd();
  UInt_t SpectrumCount();

  void addSpectrumDictionaryObserver(SpectrumDictionaryObserver* observer);
  void removeSpectrumDictionaryObserver(SpectrumDictionaryObserver* observer);

  // Misc spectrum utilities:

  void ClearSpectrum(std::string name);
  void ClearAllSpectra();

  // Create gates of various types:

  CGate* CreateGate(CGateFactory::GateType gateType, 
		    std::vector<std::string> names);
  CGate* CreateGate(CGateFactory::GateType gateType, 
		    std::vector<std::string> parameters, 
		    std::vector<FPoint> points);
  CGate* CreateGate(CGateFactory::GateType   gateType,
		    std::vector<FPoint>      points,
		    std::vector<UInt_t> parameters);
  CGate* CreateGate(CGateFactory::GateType gateType, 
		    std::vector<std::string> rparameters,
		    long comparison);
  CGate* CreateTrueGate();
  CGate* CreateFalseGate();
  CGate* CreateBand(std::string xparameter, std::string yparameter, 
		    std::vector<FPoint> points);
  CGate* CreateContour(std::string xParameter, std::string yParameter, 
		       std::vector<FPoint> points);
  CGate* CreateBandContour(std::string firstBand, std::string secondBand);
  CGate* CreateNotGate(std::string name);
  CGate* CreateAndGate(std::vector<std::string> gateNames);
  CGate* CreateOrGate(std::vector<std::string> gateNames);
  CGate* CreateCut(std::string parameter, Float_t low, Float_t high);
  CGate* CreateGammaCut(Float_t low, Float_t high, std::vector<std::string> constituents);
  CGate* CreateGammaBand(std::vector<FPoint> points, 
			 std::vector<std::string> constituents);
  CGate* CreateGammaContour(std::vector<FPoint> points, 
			    std::vector<std::string> constituents);
  CGate* CreateMaskEqualGate(std::vector<std::string> rParameterName,
			     long Compare);
  CGate* CreateMaskAndGate(std::vector<std::string> rParameterName,
			     long Compare);
  CGate* CreateMaskNotGate(std::vector<std::string> rParameterName,
			     long Compare);


  // Gate dictionary manipulation:

  void AddGate(std::string name, CGate* gate);
  void DeleteGate(std::string gateName);
  void ReplaceGate(std::string gateName, CGate& newGate);
  CGateContainer* FindGate(std::string gateName);
  CGateDictionaryIterator GateBegin();
  CGateDictionaryIterator GateEnd();
  UInt_t GateCount();

  void addGateDictionaryObserver(CGateObserver* observer);
  void removeGateDictionaryObserver(CGateObserver* observer);
  


  void ApplyGate(std::string gateName, std::string spectrumName);

  // Manipulating the event processor pipeline.

  void CreatePipeline(std::string name);
  void ListPipelineList();
  void ListCurrentPipeline();  
  void ListAll();
  void GetPipeline(std::string name);
  void AddEventProcessor(std::string name_pipe, CEventProcessor& eventProcessor, const char* name_proc = 0);  
  CTclAnalyzer::EventProcessorIterator FindEventProcessor(std::string name_pipe, std::string name);
  CTclAnalyzer::EventProcessorIterator FindEventProcessor(std::string name_pipe, CEventProcessor& processor);
  void InsertEventProcessor(std::string name_pipe, CEventProcessor& processor,
			    CTclAnalyzer::EventProcessorIterator where, 
			    const char*  name = 0);
  void RemoveEventProcessor(std::string name_pipe, std::string name);
  void RemoveEventProcessor(std::string name_pipe, CTclAnalyzer::EventProcessorIterator here);
  void RemovePipeline(std::string name_pipe);
  void ClearPipeline(std::string name_pipe);
  void RestorePipeline(std::string name_pipe);    
  std::string GetCurrentPipeline();
  UInt_t ProcessingPipelineSize(std::string name_pipe);
  CTclAnalyzer::EventProcessorIterator ProcessingPipelineBegin(std::string name_pipe);
  CTclAnalyzer::EventProcessorIterator ProcessingPipelineEnd(std::string name_pipe);

  // These methods are provided for compatibility with pipeline manipulation prior to the
  // pipeline manager.  They manipulate the current pipeline:

  void AddEventProcessor(CEventProcessor& eventProcessor, const char* name_proc = 0);
  CTclAnalyzer::EventProcessorIterator FindEventProcessor(std::string name);
  CTclAnalyzer::EventProcessorIterator FindEventProcessor(CEventProcessor& processor);
  void InsertEventProcessor(CEventProcessor& processor,
			    CTclAnalyzer::EventProcessorIterator where, 
			    const char*  name = 0);
  void RemoveEventProcessor(std::string name);
  void RemoveEventProcessor(CTclAnalyzer::EventProcessorIterator here);
  UInt_t ProcessingPipelineSize();
  CTclAnalyzer::EventProcessorIterator ProcessingPipelineBegin();
  CTclAnalyzer::EventProcessorIterator ProcessingPipelineEnd();

  
  
  // Spectrum I/O formatting control:

  void AddSpectrumFormatter(std::string name, CSpectrumFormatter& formatter);


  // Event sink pipeline control:

  void AddEventSink(CEventSink& sink, const char* name = 0);
  CEventSinkPipeline::EventSinkIterator FindEventSink(std::string sinkName);
  CEventSinkPipeline::EventSinkIterator FindEventSink(CEventSink& sink);
  void InsertEventSink(CEventSink& sink, CEventSinkPipeline::EventSinkIterator here,
		       const char* name = 0);
  CEventSink* RemoveEventSink(std::string name);
  CEventSink* RemoveEventSink(CEventSinkPipeline::EventSinkIterator here);
  UInt_t EventSinkPipelineSize();
  CEventSinkPipeline::EventSinkIterator EventSinkPipelineBegin();
  CEventSinkPipeline::EventSinkIterator EventSinkPipelineEnd();



  // Filter API:

  void               createFilter(std::string name, CGatedEventFilter* pFilter);
  CGatedEventFilter* findFilter(std::string name);
  bool               filterExists(CGatedEventFilter* pFilter);
  void               deleteFilter(CGatedEventFilter* pFilter);
  void               deleteFilter(std::string filterName);
  void               addFilterOutputFormat(CFilterOutputStageCreator& creator);
  

  // Expose the guts for the really curious and those who need more
  // than we can offer.

  CTCLInterpreter*    getInterpreter();
  CHistogrammer*      GetHistogrammer();
  CTclAnalyzer*       GetAnalyzer();
  CEventSinkPipeline* GetEventSinkPipeline();
  CDisplayInterface *GetDisplayInterface();
  void SetDisplayInterface(CDisplayInterface& rInterface);
  std::vector<UInt_t>      parameterIds(std::vector<std::string> names);

  

  
};


#endif 
