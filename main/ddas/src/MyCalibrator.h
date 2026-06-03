#ifndef MYCALIBRATOR_H
#define MYCALIBRATOR_H

#include <EventProcessor.h>
#include <TreeParameter.h>

class MyParameters;

/**
 * @class MyCalibrator
 * @brief This class is responsible for calibrating raw parameters and filling
 * calibrated parameters.
 * @details
 * The `operator()` function is called for each physics event seen by the event
 * processor. In this function, you can access the raw parameters, perform
 * calibrations, and fill the calibrated parameters. The example code shows a
 * simple linear calibration. The "calibrated" energy and calibration parameters
 * are defined as member variables of this class, but can easily be split into
 * their own classes similar to `MyParameters` and `MyParameterMapper` if
 * desired.
 * @note The calibrator class maintains a reference to the raw parameters but
 * does not own them.
 */
class MyCalibrator : public CEventProcessor {
private:
  MyParameters &m_params;      //!< Reference to raw parameters.
  CTreeParameterArray m_ecal;  //!< Calibrated energy parameters.
  CTreeVariableArray m_slope;  //!< Slope for linear calibration.
  CTreeVariableArray m_offset; //!< Offset for linear calibration.

public:
  /**
   * @brief Construct using MyParameters.
   * @param rParams References the raw parameter struct.
   */
  MyCalibrator(MyParameters &rParams);

  /**
   * @brief Called for each physics event seen by the event processor.
   * @param pEvent Pointer to the raw data.
   * @param rEvent Array-like storage for parameter values created by the
   * event-processing pipeline.
   * @param rAnalyzer Analyzer object (rarely used).
   * @param rDecoder Buffer decoder object (rarely used).
   * @return Bool indicating status. False will terminate the event processing
   * pipeline.
   */
  Bool_t operator()(const Address_t pEvent, CEvent &rEvent,
                    CAnalyzer &rAnalyzer, CBufferDecoder &rDecoder);
};

#endif
