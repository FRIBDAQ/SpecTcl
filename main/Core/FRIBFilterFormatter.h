/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
             Giordano Cerizza
             Simon Giraud
	     FRIB
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/**
 *  Provide a filter output formatter for SpecTcl that writes data in the output
 * format of the FRIB analysis pipeline.  These event files can be directly read by
 * SpecTcl as of the time this is being written and Rustogramer.
 * 
 * The output format of these files are essentially V12 ring items with no body header.
 * For the full format, see https://docs.nscl.msu.edu/daq/newsite/apipeline/index.html and
 * look at the contents of the AnalysiysRingItems.h file. From that set,
 * We'll write the following ring items types:
 * 
 * *  PARAMETER_DEFINITIONS - at the time the filter file is opened.
 * *  VARIABLE_VALUES       - At the time the filter file is opened.
 * *  PARAMETR_DATA         - For each event we are asked to write.
 * 
 * 
*/
#ifndef FRIBFILTERFORMAT_H
#define FRIBFILTERFORMAT_H
#include "CFilterOutputStage.h"    // Base class.
#include <histotypes.h>
#include <vector>

class CFRIBFilterFormat : public CFilterOutputStage {
private:
    int m_fd;                       // Output file descriptor.
    unsigned long m_trigger;        // Trigger number for output.
    std::vector<UInt_t> m_ids;    // Ids to write.
public:
    CFRIBFilterFormat();
    virtual ~CFRIBFilterFormat();
private:
    CFRIBFilterFormat(const CFRIBFilterFormat& rhs);
    CFRIBFilterFormat& operator=(const CFRIBFilterFormat& rhs);
    int operator==(const CFRIBFilterFormat& rhs);
    int operator!=(const CFRIBFilterFormat& rhs);


public:
    virtual void open(std::string filename) ;
    virtual void close() ;
    virtual void DescribeEvent(std::vector<std::string> parameterNames,
			     std::vector<UInt_t>      parameterIds) ;
    virtual void operator()(CEvent& event) ;
    virtual std::string  type() const;
private:
    void writeParameterDescriptions(const std::vector<std::string>& names, const std::vector<UInt_t> ids);
    void writeVariableDefs();

};


#endif