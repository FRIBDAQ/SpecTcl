/*
** Facility:
**   NSCL display program
** Abstract:
**   spectra.cc  - This file contains code for methods and functions to access
**                 shared spectrum storage regions.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** SCCS information:
**    @(#)spectra.cc	8.1 6/23/95 
*/
static char *version="@(#)spectra.cc	8.1 6/23/95 ";
/*
** Include files:
*/
#include <stdio.h>
#ifdef unix
#include <strings.h>
#include <memory.h>
#endif
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "dispshare.h"

/*
** Additional references:
*/
#ifndef Linux
extern "C" {
  void exit(int);
}
#endif
/*
** Exported storage:
*/
spec_shared *xamine_shared;
spec_shared *spectra;
/*
** Local storage:
*/

typedef char chooser_name[80];
chooser_name names[DISPLAY_MAXSPEC];
char *(namelist[DISPLAY_MAXSPEC]);

int comp(const void *s1,const void *s2)
{
  return strcmp((char *)s1, (char *)s2);
}


/*
** Method description:
**   getversion - Gets the SCCS version of the code.  This is mostly
**                used to ensure that there will not be warnings about
**                lack of references to version above.
** Returns:
**   Pointer to the version string filled in by SCCS.
*/
char *spec_shared::getversion() volatile
{
  return version;
}

/*
** Functional Description:
**
**    upcase - Convert a string in place to an uppercase equivalent of itself.
** Formal Parameters:
**   char *s:
**      The string to convert to upper case.
** Returns:
**   Pointer to the string.
*/
char *upcase(char *s)
{
  char *st;
  st = s;
  while(*st != '\0') {
    *st = toupper(*st);
    st++;
  }
  return s;
}

/*
** Functional Description:
**    cvttitle   - Convert a title from display title format to C ASCIZ form.
**                 display title form is typically FORTRAN format where
**                 the spectrum name is blank filled.  Once the spectrum
**                 name is copied, trailing blanks are removed and it is
**                 upcased.
** Formal Parameters:
**     char *dest:
**        Buffer for the resulting ASCIZ string.
**     char *src:
**        Source of AED title string.
** Returns:
**    Pointer to dest.
*/
static char *cvttitle(char *dest, volatile char *src)
{
  char *p;

  memcpy(dest,(char*)src, sizeof(spec_title));  /* Grab the whole string */
  dest[sizeof(spec_title)-1] = '\0';     /* Ensure null termination */
  if(strlen(dest) > 0) {
	  p = dest + strlen(dest)-1;	/* Point to last character of string */

	  while(p != (dest)) {
	    if(isspace(*p)) {
	      *p--  = '\0';
	    }
	    else
	      break;
	  }
          p = upcase(dest);
  }
  return dest;
}
      

/*
** Method Description:
**   getspecid  - Search the set of defined spectrum for a spectrum with
**                a name which matches the requested name.  This search will
**                be case blind.  The search is complicated by the fact
**                that we cannot assume that the display titles are
**                in fact null terminated strings. Therefore we convert them
**                to null terminated strings removing trailing whitespace.
** Formal Parameters:
**     char *name:
**        Name of the spectrum to look for.
** Returns:
**    -1   - Search failed.
**   Otherwise: 
**     ID of spectrum.
**     Note that the ID of the spectrum is the FORTRAN index number of the
**     spectrum, not the C index number.
**/
int spec_shared::getspecid(char *name) volatile
{
  spec_title search;
  spec_title current;
  int id;

  memcpy(search, name, sizeof(search));
  search[sizeof(search)-1] = '\0';
  upcase(search);		/* Make an upper case version of the name */
  
  for(id = 0; id < DISPLAY_MAXSPEC; id++) {
    if(dsp_types[id] != undefined) {
      cvttitle(current, dsp_titles[id]);
      if(strncmp(current, search, sizeof(spec_title)) == 0) return id+1;
    }
  }
  return -1;
}


/*
** Method description:
**    getname     - Gets the name of a spectrum in ASCIZ form.
** Formal Parameters:
*     spec_title dest:
**       Pointer to buffer to hold the name.
**    int id:
**       Spectrum id.
** Returns: 
**   Pointer to dest.
*/
volatile char *spec_shared::getname(spec_title name, int id) volatile
{
  cvttitle(name, dsp_titles[id-1]);
  return name;
}

/*
** Method Description:
**   getchannel - Get the value of a spectrum channel.
**                there is a 1-d and a 2-d version ... depends on how
**                many channel indices.
** Formal Parameters:
**    int id   - Spectrum id.
**    int ix   - X channel number.
**    int iy   - Y channel number.
** Returns:
**    value of associated channel.
**    Prints bug message to stderr if channel out of range, or
**    if wrong type of spectrum... in that case returns 0 to allow execution
**    to continue with 'reasonable' results.
**/
unsigned int spec_shared::getchannel(int id, int ix) volatile
{
  unsigned int *lptr;
  unsigned short *sptr;

  switch(gettype(id)) {
  case undefined:
    fprintf(stderr, "Attempted getchannel from undefined spectrum %d\n",id-1);
    return 0;
  case onedlong:
    lptr = (unsigned int *)getbase(id);
    if(lptr == NULL) {
      fprintf(stderr, "Invalid spectrum base %d\n", id);
      return 0;
    }
    if((char *)&lptr[ix] > Xamine_MemoryTop()) {
      fprintf(stderr, "Invalid spectrum channel %d,%d\n", id, ix);
      return 0;
    }
    if( (ix < getxdim(id)) && (ix >= 0))
      return (unsigned int)lptr[ix];
    fprintf(stderr, "Xindex out of range 0-%d (was %d) in 1d getchannel\n",
	    getxdim(id), ix);
    return 0;
  case twodword:
    fprintf(stderr,"Attempted 1d getchannel from 2-d %d\n",id-1);
    return 0;
  case twodbyte:
    fprintf(stderr, "Attempted 1d getchannel from 2-d %d\n", id-1);
    return 0;
  case onedword:
    sptr = (unsigned short *)getbase(id);
    if(sptr == NULL) {
      fprintf(stderr, "Invalid spectrum base %d\n", id);
      return 0;
    }
    if((char *)&sptr[ix] > Xamine_MemoryTop()) {
      fprintf(stderr, "Invalid spectrum channel %d,%d\n", id, ix);
      return 0;
    }
    if( (ix < getxdim(id)) && (ix >= 0))
      return (unsigned int)sptr[ix];
    fprintf(stderr, "Xindex out of range 0-%d (was %d) in 1d getchannel\n",
	    getxdim(id), ix);
    return 0;
  default:
    fprintf(stderr,"Invalid spectrum type %d for spectrum %d in 1d getchannel\n", 
	    gettype(id), id);
    return 0;
  }
}
unsigned int spec_shared::getchannel(int id, int ix, int iy) volatile
{
  unsigned short *sptr;
  unsigned char  *bptr;
  int idx;

  switch(gettype(id)) {
  case undefined:
    fprintf(stderr,"Attempted 2d getchannel for undefined spectrum %d\n", id);
    return 0;
  case onedlong:
    fprintf(stderr,"Attempted 2d getchannel for 1d spectrum %d\n", id);
    return 0;
  case onedword:
    fprintf(stderr, "Attempted 2d getchannel for 1d spectrum %d\n", id);
    return 0;
  case twodword:
    if( (ix >= 0) && (ix < getxdim(id)) &&
        (iy >= 0) && (iy < getydim(id))) {
      idx = ix + iy*getxdim(id);
      sptr = (unsigned short *)getbase(id);
      if(sptr == NULL) {
	fprintf(stderr, "Invalid spectrum base %d \n", id);
	return 0;
      }
      if((char *)&sptr[idx] > Xamine_MemoryTop()) {
	fprintf(stderr, "Invalid spectrum channel %d,%d\n", id, idx);
	return 0;
      }
      return (unsigned int)sptr[idx];
    }
    fprintf(stderr,"Invalid index on 2d getchannel 0-%d, 0-%d, %d,%d\n",
	    getxdim(id), getydim(id), ix, iy);
    return 0;
  case twodbyte:
    if( (ix >= 0) && (ix < getxdim(id)) &&
        (iy >= 0) && (iy < getydim(id))) {
      idx = ix + iy*getxdim(id);
      bptr = (unsigned char *)getbase(id);
      if(bptr == NULL){ 
	fprintf(stderr, "Invalid spectrum base %d\n", id);
	return 0;
      }
      if((char *)&bptr[idx] > Xamine_MemoryTop()) {
	fprintf(stderr, "Invalid spectrum channel %d,%d\n", id, idx);
	return 0;
      }
      return (unsigned int)bptr[idx];
    }
    fprintf(stderr,"Invalid index on 2d getchannel 0-%d, 0-%d, %d,%d\n",
	    getxdim(id), getydim(id), ix, iy);
    return 0;
  default:
    fprintf(stderr,"Invalid spectrum type %d for spectrum %d\n",
	    gettype(id), id);
    return 0;
  }
}

/**
** Method Description:
**    getbase   - This function returns the base address of the requested
**                spectrum.
** Formal Parameters:
**   int id:
**    Id of the spectrum in question.
**/
volatile unsigned int *spec_shared::getbase(int id) volatile
{
  volatile unsigned int *base;
  switch(gettype(id)) {
  case onedlong:
    base =  &(dsp_spectra.display_l[dsp_offsets[id-1]]);
    break;
  case onedword:
  case twodword:
    base =  (unsigned int *)&(dsp_spectra.display_w[dsp_offsets[id-1]]);
    break;
  case twodbyte:
    base = (unsigned int *)&(dsp_spectra.display_b[dsp_offsets[id-1]]);
    break;
  case undefined:
  default:
    return NULL;
  }

  /* Don't allow illegal bases. */

  if(base < (unsigned int *)xamine_shared) return NULL;
  if(base > (unsigned int *)Xamine_MemoryTop()) return NULL;

  return base;
}
/*									    */
/* Method Description:							    */
/*   gettype:								    */
/*      Gets the type of a spectrum given the id.			    */
/* Formal Parameers:							    */
/*    int id:								    */
/*       I.D of the spectrum being examined.				    */
/* Returns:								    */
/*    The type of the spectrum.						    */
spec_type spec_shared::gettype(int id) volatile
{
    if( (id < 0) || (id > DISPLAY_MAXSPEC))
	return undefined;
    return dsp_types[id-1];
}


/*
** Functional Description:
**   Xamine_GetSpectrumid:
**      Returns the id of a spectrum given the name from a chooser list.
**      We rely on the fact that chooser names are of the form:
**      title [id].
** Formal Parameters:
**    char *name:
**      Name of the spectrum.
*/
int Xamine_GetSpectrumId(char *name)
{

  char *valstart;
  int  id;
  chooser_name n;

  strcpy(n, name);
  valstart = strchr(n, '[');	/* Look for open [ */
  if(valstart == NULL) return -1; /* not a valid choosername. */
  valstart += 1;		/* Point to the digits: */
  id = atoi(valstart);
  if(id <= 0)
    return -1;			/* there is no spectrum 0 or less.. */
  return id;
}


/*
** Functional Description:
**   Xamine_GetSpectrumList:
**     This function retrieves a sorted list of defined spectra in the shared
**     memory region.  The purpose is to support chooser lists.
** Formal Parameters:
**     char ***list:
**       Pointer to list pointer buffer.
** Returns:
**     number of defined spectra
*/
int Xamine_GetSpectrumList(char ***list)
{
  int nspec;
  spec_title aname;

  /* First a list of spectrum name pointers is generated for the defined 
  ** spectra:
  */
  int i;
  nspec = 0;
  for(i = 0; i < DISPLAY_MAXSPEC; i++) {
    if(xamine_shared->gettype(i+1) != undefined) {
       xamine_shared->getname(aname, i+1);     /* Make nulll filled string */
       if(strlen(aname) == 0) strcpy(aname, "<Untitled>");
       sprintf(names[nspec], "[%03d]  %s",   i+1, aname);
       nspec++;			              /* count a defined spectrum    */
     }
  }
  /*  Next we sort the names alphabetically */

  qsort((char *)names, nspec, sizeof(chooser_name), comp);

  /* Build the pointer name list */
  
  for(i = 0; i < nspec; i ++)
    namelist[i] = names[i];
  *list = namelist;
  return nspec;

}

/* Functional Description:						    */
/*   Xamine_SpectrumType:						    */
/*     returns the type of a spectrum.					    */
/* Formal Parameters:							    */
/*    int id:								    */
/*       I.D. of the spectrum to return.				    */
/* Return:								    */
/*    spectrum type of spectrum.					    */

spec_type Xamine_SpectrumType(int id)
{
   return (spec_type)(xamine_shared->gettype(id));
}

