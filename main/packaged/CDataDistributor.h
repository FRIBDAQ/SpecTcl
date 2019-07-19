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

/** @file:  CDataDistributor.h
 *  @brief: Mechanism for distributing data from a data getter
 */
#ifndef CDATADISTRIBUTOR_H
#define CDATADISTRIBUTOR_H
#include <utility>
/**
 * @class CDataDistributor
 *    This is an ABC that distributes data from a data getter to a data sink.
 *    In serial batch spectcl, for example, a file data getter paired
 *    with a data distributor that shoves data into the analyzer
 *    does the trick,
 *
 *    For MPI e.g. rank 0 would run a file data source and an MPI distributor
 *    while all other ranks would run an MPI data source and a distributor
 *    that shoves data into the analyzer.
 *
 * @note This class only establishes the interfaces.
 */
class CDataDistributor
{
public:
    virtual ~CDataDistributor();
    virtual void handleData(std::pair<size_t, void> info) = 0;
};



#endif