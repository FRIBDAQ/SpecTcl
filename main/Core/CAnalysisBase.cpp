/**

#    This software is Copyright by the Board of Trustees of Michigan
#    State University (c) Copyright 2013.
#
#    You may use this software under the terms of the GNU public license
#    (GPL).  The terms of this license are described at:
#
#     http://www.gnu.org/licenses/gpl.txt
#
#    Author:
#            Ron Fox
#            NSCL
#            Michigan State University
#            East Lansing, MI 48824-1321

##
# @file   CAnalysisBase.cpp
# @brief  Implementation of the analysis base class.  All methods are stubs.
# @author <fox@nscl.msu.edu>
*/

#include "CAnalysisBase.h"

/**
 * onStateChange
 *    Called for a state change item:
 *
 *    @param runNumber    - The run number
 *    @param absoluteTime - The absolute time at which the state change occured.
 *    @param runTime      - Number of seconds into the run at which the state
 *                          change occured.  Note that in 11.0 and later this
 *                          may not be an integer as there can be a time divisor.
 *    @param title        - The run title.
 *    @param clientData   - Data passed without interpretation by the caller.
 *
 *   @note  Overrides for this method can throw either
 *          - CAnalysisBase::NonFatalException to report an error for which the
 *             framework can continue processing the next item.
 *          - CAnalysisBase::FatalException to report an error that does not
 *             allow for further processing.
 */
void CAnalysisBase::onStateChange(
    CAnalysisBase::StateChangeType type, int runNumber, time_t absoluteTime,
    float runTime, std::string title, void* clientData
)
{}
/**
 * onScalers
 *
 *    Called for a scaler item.
 *
 *  @param absoluteTime   - The absolute time at which the item was emitted.
 *                          note that for data where this is not directly available,
 *                          no attempt is made to compute it but, instead, this
 *                          value will be a zero.
 *  @param startOffset    - The number of seconds into the run at which the
 *                          scalers started counting for this interval.
 *  @param endOffset      - The number of seconds into the run at the end of the
 *                          scaler counting interval.
 *  @param scalers        - Vector of scalers.
 *  @param incremental    - True if the scalers are incremental, false otherwise.
 *  @param clientData     - Data passed without interpretation by the framework.
 *
 *  See the note for onStateChange.
 */
void CAnalysisBase::onScalers(
    time_t absoluteTime, float startOffset, float endOffset,
    std::vector<unsigned> scalers, bool incremental, void* clientData
)
{}

/**
 * onStringLists
 *
 *     Called for string list items.
 *
 * @param type         - The type of string list item presented.
 * @param absoluteTime - If available directly from the data the absolute time
 *                      at which the item was generated, if not, 0.
 * @param runTime      - Number of seconds into the run at which the item was
 *                        created.
 * @param strings      - The strings in the item.
 * @param clientData   - Data passed from the framework without any interpretation.
 * 
 */
void CAnalysisBase::onStringLists(
    CAnalysisBase::StringListType type, time_t absoluteTime, float runTime,
    std::vector<std::string> strings, void* clientData
)
{}
/**
 * onEvent
 *    Called on an event item.
 *
 *  @param pEvent      - Pointer to the event body.  Note that normally this would
 *                       be a pointer to an event size of some sort.  Whether
 *                       the size is inclusive or exclusive depends on the
 *                       source of the data.
 *  @param             - clientData - data passed without interpretation to the
 *                       method by the framework.
 * 
 *  @return unsigned   - Number of bytes in the event.
 */
unsigned CAnalysisBase::onEvent(void* pEvent, void* clientData)
{}