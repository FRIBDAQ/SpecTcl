/*
** Facility:
**   Xamine - NSCL display program.
** Abstract:
**   specalloc.c:
**      This file contains functions which control the allocation and 
**      deallocation of spectrum numbers.  For the most part we assume
**      that we have complete control over the spectrum numbers being
**      allocated.  However we do double check the allocation scheme
**      each time we assign a spectrum to be sure.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/

/*
** Includes:
*/
#include "client.h"

/*
** External and static varibles.
*/

static int allocated[XAMINE_MAXSPEC];
extern Xamine_shared *Xamine_memory;



/*
** Functional Description:
**   Xamine_AllocateSpectrum:
**     Returns the spectrum number of the lowest numbered free spectrum.
** Returns:
**   0  - No free spectra.
**  >0  - The number of the allocated spectrum.
*/
int Xamine_AllocateSpectrum()
{
  int i;

  for(i = 0; i < XAMINE_MAXSPEC; i++) {
    if(!allocated[i]) {
      allocated[i] = -1;	/* Mark allocated */
      if(Xamine_memory->dsp_types[i] == undefined) { /* Return if undefed. */
	return i+1;
      }
    }
  }
  return 0;			/* No free spectra. */
}
int f77Xamine_AllocateSpectrum_()
{
  return Xamine_AllocateSpectrum();
}

/*
** Functional Description:
**   Xamine_ReserveSpectrum:
**     This function attempts to reserve a particular spectrum number.
** Formal Parameters:
**   int spno:
**     Number of the spectrum:
** Returns:
**     spno - if it was free, else the results of Xamine_AllocateSpectrum.
*/
int Xamine_ReserveSpectrum(int spno)
{
  if( (!allocated[spno-1]) && 
     (Xamine_memory->dsp_types[spno-1] == undefined) 
     )
    return spno;

  return Xamine_AllocateSpectrum();
    
}
int f77Xamine_ReserveSpectrum_(int *spno)
{
  return Xamine_ReserveSpectrum(*spno);
}


/*
** Functional Description:
**   Xamine_FreeSpectrum:
**      Release a spectrum to the free pool.
** Formal Parameters:
**    int spno:
**      The number of the spectrum to release.
*/
void Xamine_FreeSpectrum(int spno)
{
  allocated[spno-1] = 0;
  Xamine_memory->dsp_types[spno-1] = undefined;
}    

void f77xamine_freespectrum_(int *spno)
{
  Xamine_FreeSpectrum(*spno);
}
