/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             
	     Facility for Rare Isotope Beams.
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/**
 * Gates that are accdepted via Xamine need to be propageted to all of the other ranks of
 * an MPI parallel, SpecTcl as they are not entered via the 'gate' command in the root rank
 * but via gate dictionary manipulations in the event sink pipeline rank.
 * 
 * We'll use the broacast mechanism to send the gate around, Since gate acceptance is a relatively
 * rare operation, we can send several messages for each gate.  In order;
 * 
 * -  The gate name and type (MPI struct).
 * -  parameters (chares as with dependent gates).
 * -  number of points.
 * -  Array of points (MPI struct).
 * 
 * note that for cuts, we send two points, where the x coordinates matter and y don't.
 * note that all Xamine accdepted gates will be point gates.
*/
#ifndef GATEPUMP
#define GATEPUMP
#include <string>

class CGate;


void startGatePump();
void broadcastGate(std::string name, CGate* pGate);
#endif