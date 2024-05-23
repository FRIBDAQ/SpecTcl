#ifndef MYCALIBRATOR_H
#define MYCALIBRATOR_H

#include <EventProcessor.h>
#include <TreeParameter.h>

class MyParameters;

//________________________________________________________________________
// Calibrated parameters using TreeParameters.
//
class MyCalibrator : public CEventProcessor
{
private:
    MyParameters&       m_params; // Raw parameters.
    CTreeParameterArray m_ecal;   // Calibrated energy parameters.
    CTreeVariableArray  m_slope;  // Slope for linear calibration.
    CTreeVariableArray  m_offset; // Offset for linear calibration.

public:
    /**
     * @brief Construct using MyParameters.
     *
     * @param rParams References the raw parameter struct.
     */
    MyCalibrator(MyParameters& rParams);

    /**
     * @brief Called for each physics event seen by the event processor. 
     *
     * @param pEvent Pointer to the raw data.
     * @param rEvent Array-like storage for parameter values created by the 
     *   event-processing pipeline.
     * @param rAnalyzer Analyzer object (rarely used). 
     * @param rDecoder Buffer decoder object (rarely used).
     * 
     * @return Bool indicating status. False will terminate the event 
     *   processing pipeline.
     */
    Bool_t operator()(
	const Address_t pEvent, CEvent& rEvent,
	CAnalyzer& rAnalyzer, CBufferDecoder& rDecoder
	);

};

#endif
