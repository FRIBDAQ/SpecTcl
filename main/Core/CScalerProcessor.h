/******************************************************************************
*
* CAEN SpA - Front End Division
* Via Vetraia, 11 - 55049 - Viareggio ITALY
* +390594388398 - www.caen.it
*
***************************************************************************//**
* \note TERMS OF USE:
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free Software
* Foundation. This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. The user relies on the
* software, documentation and results solely at his own risk.
******************************************************************************/


/**
 * @file  CScalerProcessor.h
 * @brief Analysis processor for scaler display
 * @author Ron Fox <ron@caentechnologies.com>
 */

#ifndef CSCALERPROCESSOR_H
#define CSCALERPROCESSOR_H
#include "CAnalysisBase.h"
#include <vector>


class CTCLInterpreter;
class CRingFormatHelper;
class BufferTranslator;
class CRingBufferDecoder;

class ScalerProcessor : public CAnalysisBase
{
  CTCLInterpreter& m_Interp;
  std::vector<unsigned>      m_totals;

public:
  ScalerProcessor(CTCLInterpreter& interp);
  virtual ~ScalerProcessor();

public:
    virtual void onStateChange(
        StateChangeType type, int runNumber, time_t absoluteTime, float runTime,
        std::string title, void* clientData
    );
    virtual void onScalers(
        time_t absoluteTime, float startOffset, float endOffset,
        std::vector<unsigned> scalers, bool incremental, void* clientData
    );  

private:
  // Tcl Variable setting:

  void Set(const char* varName, std::string value);
  void Set(const char* varName, int value);
  void Set(const char* varName, int index, int value);

  uint32_t getSid();
  uint32_t getScalerSid();
  CRingBufferDecoder* getDecoder();
  void*    getItem();
  BufferTranslator* getTranslator();
  CRingFormatHelper* getHelper();
  
};


#endif
