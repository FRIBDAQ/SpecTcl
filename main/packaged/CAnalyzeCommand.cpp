/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  CAnalyzeCommand.cpp
 *  @brief: Implement the analysis driver for batch spectcl.
 */

#include "CAnalyzeCommand.h"
#include "TCLInterpreter.h"
#include "CDataGetter.h"
#include "CDataDistributor.h"

#include <Globals.h>
#include <Exception.h>

#include <stdexcept>
#include <string>
#include <iostream>

CAnalyzeCommand* CAnalyzeCommand::m_pInstance(0);          // Singleton instance.
CDataGetter*     CAnalyzeCommand::m_pDataSource(0);
CDataDistributor* CAnalyzeCommand::m_pDataSink(0);

/**
 * constructor
 *   @param interp - references the interpreter on which we'll register.
 *
 */
CAnalyzeCommand::CAnalyzeCommand(CTCLInterpreter& interp) :
    CTCLObjectProcessor(interp, "analyze", true)
{
        
}
/**
 * getInstance()
 *   @return CAnalyzeCommand* - instance pointer, construting if necessary.
 */
CAnalyzeCommand*
CAnalyzeCommand::getInstance()
{
    if (m_pInstance == 0) {
        if (gpInterpreter) {
            m_pInstance = new CAnalyzeCommand(*gpInterpreter);
        } else {
            // ooops no interpreter yet!!!
            
            throw std::logic_error("FATAL! - attempted to create the analyze command before the interpreter");
        }
    }
    return m_pInstance;
}
/**
 * setDataGetter
 *    @param pGetter - The data getter to establish for the next analysis.
 */
void
CAnalyzeCommand::setDataGetter(CDataGetter* pGetter)
{
    delete m_pDataSource;
    m_pDataSource = pGetter;
}

/**
 * setDistributor
 *   @param pDistrib - new data distributor.
 */
void
CAnalyzeCommand::setDistributor(CDataDistributor* pDistrib)
{
    delete m_pDataSink;
    m_pDataSink = pDistrib;
}
/**
 * operator()
 *    Runs the "analyze" command.
 * @param interp -the interpreter running the command (SpecTcl's).
 * @param objv   - vector of command objects.
 * @return int - Tcl status.
 * @note exception handilng is used to simplify error handling.
 */
int
CAnalyzeCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    try {
        requireExactly(objv, 1);                // No addtional parameters.
        
        // To analyze, both the data source and data sink must have been set.
        
        if (!m_pDataSource) {
            throw std::logic_error("No method to get data has been set");
        }
        if (!m_pDataSink) {
            throw std::logic_error("No method to define what's done with data has been set");
        }
        // We have everything we need:
        
        while (1) {
            std::pair<size_t, void*> data = m_pDataSource->read();
            m_pDataSink->handleData(data);
            size_t nBytes = data.first;
            m_pDataSource->free(data);
            if (nBytes == 0) break;       // End of data source.
        }
        
    } catch (CException& e) {
        interp.setResult(e.ReasonText());
        return TCL_ERROR;
    } catch (std::exception& e) {
        interp.setResult(e.what());
        return TCL_ERROR;
    } catch (std::string msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    } catch (const char* msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    } catch(...) {
        interp.setResult("Unanticipated exception type thrown");
        return TCL_ERROR;
    }
    
    return TCL_OK;
}

