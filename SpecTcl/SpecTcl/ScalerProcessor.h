
/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

*/
/*
  Author:
      Ron Fox
      NSCL
      Michigan State University
      East Lansing, MI 48824-1321
*/
#ifndef __SCALERPROCESSOR_H
#define __SCALERPROCESSOR_H

#include <EventProcessor.h>

// Forward definitions:

class CTCLInterpreter;
class CSpecTclControlBufferCallback;
class CSpecTclScalerBufferCallback;
class CBufferProcessor;

class CAnalyzer;
class CBufferDecoder;

/*!
    A scaler processor is an event processor that interacts with the
    buffer analysis framework to maintain a set of variables and TCL Callbacks
    that are roughly identical to those maintained by sclclient in a TclServer.
    To review the following variables are maintained:
    - Scaler_Totals - An array indexed by counting numbers starting from 0.
       Elements are the total scaler counts seen in channel specified by the index.
    - Scaler_Increments - an array indexed by counting numbers startig from 0.
       Elements contain the increments read from the scalers in the most recent
       scaler accumulation inteval.
    - ScalerDeltaTime - The amount of time covered by the most recent scaler
       accumulation interval (seconds)
    - ElapsedRunTime  - How long the run has been active (in seconds).
    - RunNumber       - The current run number.
    - ScalerRunState  - The state of the run {Active | Paused | Halted}
        The name of this variable is different that that used by sclclient
        (RunState) because SpecTcl maintains a RunState already that has a 
	different meaning.
     - RunTitel      - The title of the run.

    The following procs are called (if they exist):
    - Update   -  called to inform the user of an update in the variables.
    - BeginRun -  Called at the begin run.
    - EndRun   - Called at the end run.
    - ResumeRun - Called when a run resumes.
    - PauseRun  - Called at pause run.

    \note I'm too lazy to implement the state to call RunInProgress

    \note This class assumes NSCL buffer format and should not be registered as
          an event processor on systems that do not conform to that buffer format.
*/

class CScalerProcessor : public CEventProcessor
{
private:
  CBufferProcessor*              m_pProcessor;          //!< Buffer processing object. 
  CTCLInterpreter*               m_pInterpreter;       //!< Interact with this interp
  CSpecTclControlBufferCallback* m_pControlCallback;   //!< Callback for control buf 
  CSpecTclScalerBufferCallback*  m_pScalerCallback;    //!< Callback for scaler bufs. 
public:
  // Constructors and canonicals...

  CScalerProcessor();
  CScalerProcessor(const CScalerProcessor& rhs);
  virtual ~CScalerProcessor();
  
  CScalerProcessor& operator=(const CScalerProcessor& rhs);
  int operator==(const CScalerProcessor& rhs) const;
  int operator!=(const CScalerProcessor& rhs) const;


  // Overrides:

  virtual Bool_t OnAttach(CAnalyzer& rAnalyzer); // Called on registration.
  virtual Bool_t OnBegin(CAnalyzer& rAnalyzer,
			 CBufferDecoder& rDecoder); // Begin Run.
  virtual Bool_t OnEnd(CAnalyzer& rAnalyzer,
		       CBufferDecoder& rBuffer); // End Run.
  virtual Bool_t OnPause(CAnalyzer& rAnalyzer,
			 CBufferDecoder& rDecoder); // Pause Run.
  virtual Bool_t OnResume(CAnalyzer& rAnalyzer,
			  CBufferDecoder& rDecoder); // Resume Run.
  virtual Bool_t OnOther(UInt_t nType,
			 CAnalyzer& rAnalyzer,
			 CBufferDecoder& rDecoder); // Unrecognized buftype.
  
  // utility functions:
private:
  void ProcessBuffer(CBufferDecoder& rDecoder); // It all goes through here anyway.
  template<class T>
    bool CallbacksEqual(T* c1, T* c2) const;
};

#endif
