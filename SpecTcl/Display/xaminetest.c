/*
** Test program for Xamine
*/
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include "client.h"

#ifdef VMS
Xamine_shared _align(PAGE) specs;
#endif
/*
** setupspectra:
**   sets up the classical test spectra.
*/
static void setupspectra(Xamine_shared *sp)
{
  unsigned int *spl;
  unsigned short *spw;
  unsigned char  *spb;
  int i,j;

  /* Set up the ramp spectrum as spectrum 1: */

  strcpy(sp->dsp_titles[0], "Upward ramp");
  sp->dsp_types[0]   = onedlong;
  sp->dsp_offsets[0] = 0;
  sp->dsp_xy[0].xchans = 64;
  spl   = sp->dsp_spectra.XAMINE_l;
  for(i = 0; i < 64; i++) {
    *spl++ = i;
  }

  /* set up downward ramp function as spectrum 2: */

  strcpy(sp->dsp_titles[1], "Downward Ramp");
  sp->dsp_types[1]     = onedlong;
  sp->dsp_offsets[1]   = 64;
  sp->dsp_xy[1].xchans = 64;
  for(i = 0; i < 64; i++) {
    *spl++ = 96 - i/2;
  }

  /* Set up spectrum the step function: */

  strcpy(sp->dsp_titles[2], "Step Function");
  sp->dsp_types[2]      = onedword; /* Another diff from XMWtest specs */
  sp->dsp_offsets[2]    = 128*2; /* Word offset like AEDTSK. */
  sp->dsp_xy[2].xchans  = 128;
  spw  = (unsigned short *)spl;
  for(i = 0; i < 128; i++) {
    if(i < 64)
      *spw++ = 256;
    else
      *spw++ = 0;
  }

  /* Set up 2d word spectrum Z = X * Y as spectrum 4 */

  strcpy(sp->dsp_titles[3], "Z = X*Y [word]");
  sp->dsp_types[3]      = twodword;
  sp->dsp_offsets[3]    = 128+128*2; /* Word offset. */
  sp->dsp_xy[3].xchans  = 64;
  sp->dsp_xy[3].ychans  = 64;
  for(j = 0; j < 64; j++) {
    for(i = 0; i < 64; i++) {
      *spw++ = i * j;
    }
  }
  /* Set up 2-d byte spectrum z = x + y on upper diagonal */

  strcpy(sp->dsp_titles[4], "Z = X+Y on upper diagonal");
  sp->dsp_types[4]     = twodbyte;
  sp->dsp_offsets[4]   = (64*64 + 128 + 128*2)*2; /* Byte offset */
  sp->dsp_xy[4].xchans = 128;
  sp->dsp_xy[4].ychans = 128;
  spb = (unsigned char *)spw;
  for(j = 0; j < 128; j++) {
    for(i = 0; i < 128; i++) {
      if( i < j )
	*spb++ = i + j;
      else
	*spb++ = 0;
    }
  }
}

int
main()
{
  Xamine_shared *spectra;
  char junk[100];

#ifdef VMS
    spectra = &specs;
#endif
  if(!Xamine_CreateSharedMemory(1*MEG, &spectra)) {
    perror("Failed to make shared memory region");
    exit(errno);
  }
  printf("Made shared memory at %x\n",spectra);

  setupspectra(spectra);	/* Set up the classical test spectra. */
  printf("Created test spectra\n");

  if(!Xamine_Start()) {
    perror("Failed to start Xamine subprocess\n");
    exit(errno);
  }
  printf("Xamine is started up\n");
  printf("Press return to exit\n");
  scanf("%s", junk);

  if(!Xamine_Stop()) {
    perror("Couldn't stop Xamine");
    exit(errno);
  }

  printf("Xamine stopped\n");
  
  return 0;
}


