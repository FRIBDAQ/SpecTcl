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
 * Provide a creator for the extensible filter format factory to create FRIBFilterFormatters
 * that write data in FRIB data analysis pipline form.
 * This can be registered with the filter format factory singleton to make SpecTcl able to write 
 * that format.
*/

#ifndef FRIBFILTERFORMATTERCREATOR_H
#define FRIBFILTERFORMATTERCREATOR_H

#include "CFilterOutputStageCreator.h"
#include <string>

class CFRIBFilterFormatterCreator : public CFilterOutputStageCreator {
public:
    virtual CFilterOutputStage* operator()(std::string type);
    virtual std::string document() const;
    virtual CFilterOutputStageCreator* clone();
};


#endif