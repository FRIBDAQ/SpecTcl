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

/** @file:  spectra.cc
 *  @brief: Methods and functions accessing spectrum shared storage see dispshare.h
 */
static const char* Copyright = "(C) Copyright Michigan State University 1994, All rights reserved";
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
static const char *version="@(#)spectra.cc	8.1 6/23/95 ";
/*
** Include files:
*/

#include <config.h>

#include <stdio.h>
#include <strings.h>
#include <memory.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include <string>
#include <map>

#include "dispshare.h"

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif

/*
** Additional references:
*/
#ifndef LINUX
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

typedef char chooser_name[128];
chooser_name names[XAMINE_MAXSPEC];
char *(namelist[XAMINE_MAXSPEC]);

int comp(const void *s1,const void *s2)
{
  return strcmp((char *)s1, (char *)s2);
}

// Local type definitions.

typedef pair<int, string> NumberAndName;


/*
** Method description:
**   getversion - Gets the SCCS version of the code.  This is mostly
**                used to ensure that there will not be warnings about
**                lack of references to version above.
** Returns:
**   Pointer to the version string filled in by SCCS.
*/
const char *spec_shared::getversion() volatile
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
** Assumptions:
**    The destination is of type spec_title or at least that large.
*/
static char *cvttitle(char *dest, volatile char *src, int upper)
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
          if(upper) p = upcase(dest);
	  else       p = dest;
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
  
  for (id = 0; id < XAMINE_MAXSPEC; id++) {
    if(dsp_types[id] != undefined) {
      cvttitle(current, dsp_titles[id], 1);
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
  cvttitle(name, dsp_titles[id-1],1);
  return name;
}
volatile char* spec_shared::getinfo(spec_title info, int id) volatile
{
  cvttitle(info, dsp_info[id-1], 1);
  return info;
}
volatile char* spec_shared::getxlabel_map(spec_label label, int id) volatile
{
  cvttitle(label, dsp_map[id-1].xlabel, 0);
  return label;
}
volatile char* spec_shared::getylabel_map(spec_label label, int id) volatile
{
  cvttitle(label, dsp_map[id-1].ylabel, 0);
  return label;
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
** Note: with SpecTcl 5.3, spectra have an extra (invisible) channel.
**       on each end; channel 0 of the channel soup represents
**       underflow increment counts and channel xdim represents
**       overflows. We're going to:
**       - Increment ix first to make channel 0 invisible
**       - Only allow the visibility of xdim-2 channels.
**/
unsigned int spec_shared::getchannel(int id, int ix) volatile
{
  unsigned int *lptr;
  unsigned short *sptr;
  ix++;                   // Make underflows invisible.
  int xdim = getxdim(id);
  
  switch(gettype(id)) {
  case undefined:
    
    return 0;
  case twodlong:      // Ignore 2-d spectra.
  case twodbyte:
  case twodword:
    
    return 0;
  case onedlong:
  case onedword:

    sptr = (unsigned short *)getbase(id);
    lptr = (unsigned int *)getbase(id);
    
    if(lptr == NULL) {
      fprintf(stderr, "Invalid spectrum base %d\n", id);
      return 0;
    }

    if( (ix < (xdim-2)) && (ix >= 1)) {
      // what we do now depends on the detailed spectrum type:
      
      if (gettype(id) == onedlong) {
        if((char *)(lptr+ix) > Xamine_MemoryTop()) {
          return 0;
        }
        return (unsigned int)lptr[ix];
      } else {
        if ((char*)(sptr+ix) > Xamine_MemoryTop()) {
          return 0;
        }    
        return (unsigned int)(sptr[ix]);
      }
    }
    
    return 0;
  
  default:
    
    return 0;
  }
}
unsigned int spec_shared::getchannel(int id, int ix, int iy) volatile
{
  unsigned int* lptr;
  unsigned short *sptr;
  unsigned char  *bptr;
  int idx;
  int xdim = getxdim(id);
  int ydim = getydim(id);
  
  // Make the underflow invisible:
  
  ix++;
  iy++;

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
  case twodlong:
  case twodword:          // The -2's below make the overflow invisible
  case twodbyte:
    if( (ix >= 0) && (ix < xdim-2) &&
        (iy >= 0) && (iy < ydim-2)) {
      idx = ix + iy*getxdim(id);
      lptr =(uint32_t *)getbase(id);
      sptr = (uint16_t*)(lptr);
      bptr = (uint8_t *)(lptr);
      if(lptr == NULL) {
        fprintf(stderr, "Invalid spectrum base %d \n", id);
        return 0;
      }
      // What we do now depend son the detailed type:
      
      if (gettype(id) == twodlong) {
        if((char *)(lptr+idx) > Xamine_MemoryTop()) {
            return 0;
      }
      return (unsigned int)lptr[idx];
    } else if (gettype(id) == twodword) {
        if ((char*)(sptr+idx) > Xamine_MemoryTop()) {
            return 0;
        }
        return (unsigned int)(sptr[idx]);
      } else {  // byte
        if ((char*)(bptr+idx) > Xamine_MemoryTop()) {
            return 0;
        }
        return (unsigned int)(bptr[idx]);
    }
    // Out of range:
    return 0;
  }
  default:
    
	  
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
  size_t offset = dsp_offsets[id-1];
  volatile unsigned int *base;
  switch(gettype(id)) {
  case twodlong:
  case onedlong:
    base =  &(dsp_spectra.XAMINE_l[offset]);
    break;
  case onedword:
  case twodword:
    base =  (unsigned int *)&(dsp_spectra.XAMINE_w[offset]);
    break;
  case twodbyte:
    base = (unsigned int *)&(dsp_spectra.XAMINE_b[offset]);
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
    if( (id < 0) || (id > XAMINE_MAXSPEC))
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
/**
 * Xamine_GetSpectrumId
 *     This overload gets the id of a spectrum given just its title string.
 *
 *  @param name - the title string for the spectrum.
 *  @return int - Slot for the spectrum else -1 if there's no match.
 */
int
Xamine_GetSpectrumId(std::string name)
{
    
    for (int i = 0; i < XAMINE_MAXSPEC; i++) {
        int index = i+1;
        if (xamine_shared->gettype(index) != undefined)  {
            spec_title aname;
            xamine_shared->getname(aname, index);
            if (strcasecmp(aname, name.c_str()) == 0) {
                return index;
            }
        }
    }
    return -1;
}
/*
** Functional Description:
**   Xamine_GetSpectrumList:
**     This function retrieves a sorted list of defined spectra in the shared
**     memory region.  The purpose is to support chooser lists.
**     
** Formal Parameters:
**     char ***list:
**       Pointer to list pointer buffer.
** Returns:
**     number of defined spectra
*/
int Xamine_GetSpectrumList(char ***list)
{

  spec_title aname;
  map<string, NumberAndName> NameInfo;

  /* First a list of spectrum name pointers is generated for the defined 
  ** spectra:
  */
  int i;
  for(i = 0; i < XAMINE_MAXSPEC; i++) {
    if(xamine_shared->gettype(i+1) != undefined) {
       xamine_shared->getname(aname, i+1);     /* Make nulll filled string */
       if(strlen(aname) == 0) {	               // Spectra don't require names...
	 strcpy(aname, "<Untitled>");
       }

       NameInfo[string(aname)] = NumberAndName(i+1, string(aname));
     }
  }
  // Pull the names out of the NumberAndNae map (they'll be sorted).
  // and encode them into names:

  int nspec =0;
  for(map<string,NumberAndName>::iterator i = NameInfo.begin();
      i != NameInfo.end(); i++) {

    sprintf(names[nspec], "[%05d] %s", i->second.first, i->first.c_str());
    nspec++;
  }

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

