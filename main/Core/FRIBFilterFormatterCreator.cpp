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

/** Implement the CFilterFormatterCreator class
 * 
 * 
*/
#include "FRIBFilterFormatterCreator.h"
#include "FRIBFilterFormatter.h"

static CFRIBFilterFormat afilter;   // So we can match the type,

/**
 * operator()
 * We're asked if a specific filer type is outs to create and do so if so:
 * 
 * @param type = the desired filter type
 * @return CFilterOutputStage* - null if we're not the right creator else a dynamically
 *      Created FRIBFilterFormatter.
*/
CFilterOutputStage*
CFRIBFilterFormatterCreator::operator()(std::string type) {
    if (type == afilter.type()) {
        return new CFRIBFilterFormat;
    } else {
        return nullptr;
    }
}

/** 
 * doument
 *    Return a string that documents what we can create.
 * 
 * @return std::string
*/
std::string
CFRIBFilterFormatterCreator::document() const {
    std::string result = afilter.type();
    result += "   - FRIB Analysis pipeline compatible output";
    return result;
}

/** clone
 *    @return FRIBFilterFormatterCreator - duplicate of me.
*/
CFilterOutputStageCreator*
CFRIBFilterFormatterCreator::clone() {
    return new CFRIBFilterFormatterCreator;
}