/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/*
** Facility:
**   Display spectrum access manager
** Abstract:     dispshare.h
**   This include file describes the shared memory regions which contain
**   spectrum and control information for the spectrum display program.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** Version:
**     @(#)dispshare.h	2.3 5/26/94 
*/

#ifndef DISPSHARE_H_INSTALLED_
#define DISPSHARE_H_INSTALLED_

#include <config.h>
#include "xamineDataTypes.h"

#include <string.h>
#include <stdio.h>
#include <sys/types.h>

#include <string>


struct spec_shared : Xamine_shared 
{
  void setdim(int id, int xd) volatile {
    dsp_xy[id-1].xchans = xd;
    dsp_xy[id-1].ychans = 0;
  }
  void setdim(int id, int xd, int yd) volatile {
    dsp_xy[id-1].xchans = xd;
    dsp_xy[id-1].ychans = yd;
  }
  
  float getxmax_map(int id) volatile { return dsp_map[id-1].xmax; }
  float getxmin_map(int id) volatile { return dsp_map[id-1].xmin; }
  float getymax_map(int id) volatile { return dsp_map[id-1].ymax; }
  float getymin_map(int id) volatile { return dsp_map[id-1].ymin; }

  volatile char *getxlabel_map(spec_label label, int id) volatile;
  volatile char *getylabel_map(spec_label label, int id) volatile;

  // Gets and sets for the other non-mapping stuff...
  int getspecid(char *name) volatile;
  int getspectrumcount() volatile  {return XAMINE_MAXSPEC; }
  int getmaxchannels() volatile    {return XAMINE_SPECBYTES; }
  spec_type gettype(int id) volatile;
  void settype(int id, spec_type tp) volatile {
    dsp_types[id-1] = tp;
  }
  spec_type gettype(char *name) volatile
  { 
    int id;
    id = getspecid(name);
    return ((id < 0)? undefined :
	    gettype(id)); 
  }
  int getchannels(int id)  volatile
  { 
    return (((dsp_types[id-1] == onedlong) ||
	     (dsp_types[id-1] == onedword))? 
	    dsp_xy[id-1].xchans :
	    dsp_xy[id-1].xchans * dsp_xy[id-1].ychans); 
  }
  int getchannels(char *name) volatile 
  {
    int id;
    id = getspecid(name);
    return ((id < 0)? -1:
	    getchannels(id)); 
  }
  int getxdim(int id) volatile  { return dsp_xy[id-1].xchans; }
  int getxdim(char *name) volatile 
  { 
    int id;
    id = getspecid(name);
    return ((id < 0) ? -1:
	    getxdim(id)); 
  }
  int getydim(int id) volatile  { return dsp_xy[id-1].ychans; }
  int getydim(char *name) volatile 
  { 
    int id;
    id = getspecid(name);
    return ((id < 0) ? -1:
	    getydim(id)); 
  }
  volatile char *getname(spec_title name, int id) volatile;
  volatile char *getinfo(spec_title info, int id) volatile;
  unsigned int getchannel(int id, int ix) volatile;
  unsigned int getchannel(int id, int ix, int iy) volatile;
  volatile unsigned int *getbase(int id) volatile;
  const char *getversion() volatile;
  
  volatile const Statistics& getStatistics(int spid) volatile const {
    return dsp_statistics[spid-1];
  }
};

void Xamine_initspectra();
int Xamine_GetSpectrumList(char ***list);
int Xamine_GetSpectrumId(char *name);
int Xamine_GetSpectrumId(std::string name);
spec_type Xamine_SpectrumType(int id);
char *Xamine_MemoryTop();
#endif

