/*
** Test program for Xamine
*/
#include <stdio.h>
#include <errno.h>
#include <math.h>
//
//   For OSF, we need to turn off the UAC warnings:
//   We'll do this for both parent and us:
//
#ifdef OSF
#include <sys/sysinfo.h>
#include <sys/proc.h>
#include <machine/hal_sysinfo.h>
#endif


#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE -1
#endif

#include <Xamine.h>

/* #include "clientops.h" */

#ifdef VMS
Xamine_shared _align(page) shared;
#endif


/*
** setupspectra:
**   sets up the classical test spectra.
*/
static void setupspectra(Xamine_shared *sp)
{
  int spn;
  int i,j;
  caddr_t spec;
  unsigned char    *spec_b;
  unsigned short   *spec_w;
  unsigned int     *spec_l;

  Xamine_ManageMemory();

  /* Set up the ramp spectrum as spectrum 1: */

  spec = Xamine_Allocate1d(&spn, 64, "Upward Ramp", FALSE);
  spec_l = (unsigned int *)spec;
  for(i = 0; i < 64; i++) {
    *spec_l++ = i;
  }
  printf("Spectrum %d filled in\n", spn);

  /* set up downward ramp function as spectrum 2: */

  spec = Xamine_Allocate1d(&spn, 64, "Downward Ramp", FALSE);
  spec_l = (unsigned int *)spec;

  for(i = 0; i < 64; i++) {

    *spec_l++ = 96 - i/2;
  }
  printf("Spectrum %d filled in\n", spn);

  /* Set up spectrum the step function: */

  spec = Xamine_Allocate1d(&spn, 128, "Step Function", TRUE);
  spec_w = (unsigned short *)spec;

  for(i = 0; i < 128; i++) {
    if(i < 64)
      *spec_w++ = 256;
    else
      *spec_w++ = 0;
  }
  printf("Spectrum %d filled in\n", spn);

  /* Set up 2d word spectrum Z = X * Y as spectrum 4 */

  spec = Xamine_Allocate2d(&spn, 64, 64, "Z = X*Y [word]", FALSE);
  spec_w = (unsigned short *)spec;

  for(j = 0; j < 64; j++) {
    for(i = 0; i < 64; i++) {
      *spec_w++ = i * j;
    }
  }
  /* Set up 2-d byte spectrum z = x + y on upper diagonal */

  spec = Xamine_Allocate2d(&spn, 128,128, "Z = X+Y on upper diagonal",
			   TRUE);
  spec_b = (unsigned char *)spec;
  for(j = 0; j < 128; j++) {
    for(i = 0; i < 128; i++) {
      if( i < j )
	*spec_b++ = i + j;
      else
	*spec_b++ = 0;
    }
  }

  /* 
  ** Set up 2-d word spectrum.  This is used to test the 2-d integration
  ** software.  The gaussian parameters are as follows:
  ** Histogram is 256 * 256 words. 
  **   Centroid is at (128,170).
  **   sigmas   are   (20, 10).
  **   Weight is such that maximum value is 1024.0
  */

  spec = Xamine_Allocate2d(&spn, 512, 512, 
			   "Gaussian C = (256,240), S = (40,20)", FALSE);
  spec_w = (unsigned short *)spec;

  for(j = 0; j < 512; j++) {
    for(i = 0; i < 512; i ++) {
      float x = ((float)i - 256.0);
      float y = ((float)j - 240.0);
      float value = 1024.0 * exp(-(x*x)/(2*80.0*80.0)) * 
	                     exp(-(y*y)/(2.0*40.0*40.0));
      *spec_w++ = (unsigned short)value; 
    }
  }

  for(i = 0; i < 8; i++) {
    spec_title name;
    sprintf(name, "Overlay spectrum %d", i);
    spec = Xamine_Allocate1d(&spn, 64, name, FALSE);
    spec_l = (unsigned int *)spec;
    for(j = 0; j < 64; j++)
      *spec_l++ = (i+1)*10;
  }

}
/*
** Analyze a button descriptor:
*/
static char *btypes[] = {
  "NoPrompt",
  "Confirmation",
  "Text",
  "Points",
  "Spectrum",
  "Filename"
};
static char *bsense[] = {
  "Anytime",
  "InSpectrum",
  "In1dSpectrum",
  "In2dSpectrum"
};
char *stypes[] = 
{
  "Any",
  "Oned",
  "Twod",
  "Compatible"
};
static void AnalyzeButton(struct msg_InquireButtonAck *ack)
{
  printf("Button grid is %d x %d\n", ack->size.row, ack->size.column);
  printf("Button info is %s\n", ack->valid ? "Valid" : "Invalid");
  if(ack->valid) {
    ButtonDescription *b = &ack->button;

    printf("Code = %d\n", b->button_code);
    printf("Label = '%s'\n", b->label);
    printf("Type  = '%s'\n", (b->type == Push) ?  "Push" : "Toggle");
    if(b->type == Toggle)
      printf("Initial toggle state is %s\n", b->state ? "True" : "False");
    printf("Button is initially %s\n", b->sensitive ? "Sensitive" : 
	                                              "InSensitive");
    printf("Prompter employed is %s\n", btypes[b->prompter]);
    printf("Button is available %s\n",  bsense[b->whenavailable]);
    printf("Prompt string = '%s'\n", b->promptstr);
    if(b->prompter == Spectrum ) {
	printf("Spectrum type value %d\n", b->spectype);    
	printf("Spectrum type = %s\n", stypes[b->spectype]);
    }
    if(b->prompter == Points) {
	printf("Point requirments: [%d, %d]\n", b->minpts, b->maxpts);
    }
  }
}


main()
{
  Xamine_shared *spectra;
  char junk[100];
  int id = 0;			/* Gate Id serial number */
  int  deleted = 0;
  ButtonDescription button;
  struct msg_InquireButtonAck binfo;
  int stat;
#ifdef OSF
  {
    int buf[2];			// Buffer to contain sysinfo requests.
    buf[0] = SSIN_UACPROC;	// This and the next turn off our UAC print.
    buf[1] = UAC_NOPRINT;

    if(setsysinfo(SSI_NVPAIRS, buf, 1, 0, 0) < 0) { // Not fatal.. just noisy
      perror("-OSF Specific- Failed to turn off UAC printing");
    }

  }
#endif

#ifdef VMS
  spectra = &shared;
#endif

  if(!Xamine_CreateSharedMemory(2*MEG, &spectra)) {
    perror("Failed to make shared memory region");
    exit(errno);
  }
  Xamine_GetMemoryName(junk);
  printf("Made shared memory named %s at %x\n",
	 junk,
	 spectra);

  setupspectra(spectra);	/* Set up the classical test spectra. */
  printf("Created test spectra\n");

  if(!Xamine_Start()) {
    perror("Failed to start Xamine subprocess\n");
    exit(errno);
  }
  printf("Xamine is started up\n");

  /*
  ** Create a button box:
  */
  stat = Xamine_DefineButtonBox(5,5);
  if(stat < 0) {                      /* Create the button box. */
    fprintf(stderr, "Failed to create button box %d\n", stat);
    exit(errno);
  }
  /* Create a simple push button */

  memset(button.promptstr, 0, BUTTON_PROMPTSIZE);
  memset(button.label, 0, BUTTON_LABELSIZE);

  button.button_code = 1;	/* It's the first button so use code 1 */
  strncpy(button.label, "Exit", 
	  BUTTON_LABELSIZE-1); /* String displayed in button */
  button.type = Push;
  button.sensitive = T;		/* Start up with button available. */
  button.prompter  = Confirmation; /* Confirm exit request */
  strncpy(button.promptstr, "Really Exit? ",
	 BUTTON_PROMPTSIZE-1);
  button.whenavailable = Anytime; /* Always allowed. */
  stat = Xamine_DefineButton(0,0, &button);
  if(stat < 0) {
    fprintf(stderr, "Failed to create a button %d\n", stat);
    exit(errno);
  }

  button.whenavailable = InSpectrum;
  button.button_code = 3;
  strncpy(button.label, "Delete",
	  BUTTON_LABELSIZE-1);
  strncpy(button.promptstr, 
	 "Are you sure you want to\ndelete the selected spectrum?",
	  BUTTON_PROMPTSIZE-1);
  stat = Xamine_DefineButton(0,1, &button);
  if(stat < 0) {
    fprintf(stderr, "Failed to create a button %d\n", stat);
    exit(errno);
  }


  button.whenavailable = In1dSpectrum;
  button.button_code = 4;
  button.prompter    = Text;
  strncpy(button.promptstr, "Enter Something",
	  BUTTON_PROMPTSIZE-1);
  strncpy(button.label, "1d only",
	  BUTTON_LABELSIZE-1);
  stat = Xamine_DefineButton(0,2, &button);
  if(stat < 0) {
    fprintf(stderr, "Failed to create a button %d\n", stat);
    exit(errno);
  }

  button.whenavailable = In2dSpectrum;
  button.prompter = Spectrum;
  button.button_code = 5;
  strncpy(button.promptstr, "Choose Spectrum",
	 BUTTON_PROMPTSIZE-1);
  strncpy(button.label, "2d only",
	  BUTTON_LABELSIZE-1);
  button.spectype    = Any;
  stat = Xamine_DefineButton(0,3, &button);
  if(stat < 0) {
    fprintf(stderr, "Failed to create a button %d\n", stat);
    exit(errno);
  }

  button.spectype = Twod;
  button.prompter = Filename;
  strncpy(button.label, "Read File",
	  BUTTON_LABELSIZE-1);
  button.button_code = 6;
  stat = Xamine_DefineButton(0,4, &button);
  if(stat < 0) {
    fprintf(stderr, "Failed to create a button %d\n", stat);
    exit(errno);
  }

  button.button_code = 7;		/* New code for point prompter: */
  strncpy(button.label, "Points",
	  BUTTON_LABELSIZE-1);
  button.type = Push;
  button.sensitive = T;
  button.prompter  = Points;
  button.whenavailable = InSpectrum;
  strncpy(button.promptstr, "Gimme points",
	  BUTTON_PROMPTSIZE-1);
  button.minpts = 2;
  button.maxpts = 10;
  stat = Xamine_DefineButton(1,3, &button);
  if(stat < 0) {
    fprintf(stderr, "Failed to create button %d\n", stat);
    exit(errno);
  }

  /* Generate a simple toggle button. */

  button.button_code = 2;
  strncpy(button.label, "Disable Exit",
	  BUTTON_LABELSIZE-1);
  button.type = Toggle;
  button.sensitive = T;
  button.prompter = NoPrompt;
  button.whenavailable = Anytime;
  button.state = F;

  stat = Xamine_DefineButton(1,0, &button);
  if(stat < 0) {
    fprintf(stderr, "Failed to create a button %d\n", stat);
    exit(errno);
  }

  /* Generate a button to 'mark a peak' */

  button.button_code = 10;
  strncpy(button.label, "Peak", BUTTON_LABELSIZE-1);
  button.type = Push;
  button.sensitive = T;
  button.prompter  = Points;
  button.whenavailable = In1dSpectrum;
  strncpy(button.promptstr, "Peak and width", BUTTON_PROMPTSIZE-1);
  button.minpts = 2;
  button.maxpts = 2;
  stat = Xamine_DefineButton(2,0, &button);
  if(stat < 0) {
    fprintf(stderr, "Faile dto create button %d\n", stat);
    perror("Exiting");
    exit(errno);
  }
    

  stat = Xamine_InquireButton(0,0, &binfo);
  if(stat < 0) {
    fprintf(stderr, "Failed to inq button\n");
    exit(errno);
  }
  AnalyzeButton(&binfo);

  stat =  Xamine_InquireButton(1,0, &binfo);
  if(stat < 0) {
    fprintf(stderr, "Failed to inquire about button info.\n");
    exit(errno);
  }
  AnalyzeButton(&binfo);

  printf("To Exit, select Exit from the Xamine menu\n");
  while(Xamine_Alive()) {
    msg_XamineEvent event;
    msg_ButtonPress *bp;
    if(Xamine_PollForEvent(2, &event) > 0) { /* Got an event from Xamine */
      msg_object *g = &event.data.gate;
      switch(event.event) {	/* Break out based on event type: */
      case Gate:
	printf("Gate received, attempting to enter it back into Xamine\n");
	if(Xamine_EnterGate(g->spectrum,
			 g->id,
			 g->type,
			 g->name,
			 g->npts,
			 (Xamine_point *)g->points) < 0) {
	  printf("Unable to enter gate in Xamine\n");
	}
	break;
      case Button_Press:
	bp = &event.data.button;
	printf("Button %d pressed Selected spectrum = %d:\n", 
	       bp->buttonid, bp->selected);
	switch(bp->buttonid) {
	case 1:			/* Simple Push button... */
	  Xamine_Stop();
	  exit(0);
	  break;		/* Nothing extra to print.  */
	case 2:			/* Simple Toggle Button...  */
	  printf("   Toggle is: %s\n", bp->togglestate ? "Set" : "Not Set");
	  if(bp->togglestate) {
	    
	    printf("       Disabling Exit\n");
	    Xamine_DisableButton(0,0);
	    button.state = T;
	    strncpy(button.label, "Enable Exit",
		    BUTTON_LABELSIZE-1);
	  }
	  else {
	    printf("       Enabling Exit\n");
	    Xamine_EnableButton(0,0);
	    button.state = F;
	    strncpy(button.label, "Disable Exit",
		    BUTTON_LABELSIZE-1);
	  }
	  button.button_code = 2;
	  button.type = Toggle;
	  button.sensitive = T;
	  button.prompter = NoPrompt;
	  button.whenavailable = Anytime;
	  Xamine_ModifyButton(1,0, &button);
	  break;
	case 3:			/* Delete selected spectrum. */
	  printf("  Deleteting spectrum %d\n", bp->selected);
	  Xamine_FreeSpectrum(bp->selected);
	  break;
	case 4:			/* Requires 1-d */
	  printf("   Received string: '%s'\n", bp->text);
	  printf("   (Only allowed if 1-d spectrum selected.\n");
	  if(!deleted) {
	    printf("Deleting button number 5\n");
	    Xamine_DeleteButton(0,3);
	    deleted = TRUE;
	  }
	  break;
	case 5:			/* Requires 2-d */
	  printf("   (Only allowed if 2-d spectrum selected.\n");
	  printf("   User chose spectrum # %d\n", bp->spectrum);
	  break;
	case 6:
	  printf("   Received filename '%s'\n", bp->text);
	  printf("Killing the button box for show\n");
	  Xamine_DeleteButtonBox();
	  break;
	case 7:
	  printf("   Received a point list '%s'\n", bp->text);
	  printf("      %d points received\n", bp->npts);
	  {
	    int i;
	    for(i = 0; i < bp->npts; i++) {
	      printf("       Point %d = [%d, %d]\n",
		     i, bp->points[i].x, bp->points[i].y);
	    }
	  }
	  break;
	case 10:
	  printf("Peak position button\n");
	  {
	    float centroid = (float)bp->points[0].x;
	    float width    = (float)bp->points[1].x - centroid;
	    if(width < 0.0) width = -width;
	    stat = Xamine_EnterPeakMarker(bp->selected,
					   id++,
					   bp->text,
					   centroid, width*2.0);
	    if(stat < 0) {
	      perror("EnterPeak position failed");
	      exit(errno);

	    }
	  }
					 
	  break;
	default:
	  printf("   Unrecognized button \n");
	}
	break;
      default:
	fprintf(stderr, "Invalid event type: %d\n", event.event);
      }
    }
  }
}


