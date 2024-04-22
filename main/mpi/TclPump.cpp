/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerizza
             Simon Giraud
             Aaron Chester
             Jin Hee Chang
	     FRIB
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
#include <config.h>
#include "TclPump.h"
#include <stdlib.h>

#ifdef WITH_MPI
#include <mpi.h>
#endif

/** 
 * @return bool -true if we are not only built with MPI but were run with mpirun or mpiexec
 * 
 *    If run with mpirun or mpi exec:
 * If openmpi OMPI_COMM_WORLD_SIZE will be in the env, if MPICH, MPI_RANK will be: 
*/

bool isMpiApp() {
#ifdef WITH_MPI
    return getenv("OMPI_COMM_WORLD_SIZE") || getenv("PMI_RANK");
#else  
    return false;
#endif
}