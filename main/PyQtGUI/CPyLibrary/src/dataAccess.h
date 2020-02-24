#ifndef DATAACCESS_H
#define DATAACCESS_H

#include "dataTypes.h"

#include <string.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>

#include <string>

typedef void (*Address_t);          
static const Address_t kpNULL = (Address_t)0;

struct spec_shared : shared_memory
{
  // general info on size
  int getspectrumcount() {return MAXSPEC; }
  int getmaxchannels() {return SPECBYTES; }
  
  // dimensions and n channels
  int getxdim(int id) { return dsp_xy[id]._xchans; }
  int getxdim(char *name) 
  { 
    int id;
    id = getspecid(name); 
    return ((id < 0) ? -1: getxdim(id)); 
  }

  int getydim(int id) { return dsp_xy[id]._ychans; }
  int getydim(char *name) 
  { 
    int id;
    id = getspecid(name);
    return ((id < 0) ? -1: getydim(id)); 
  }

  int getchannels(int id) 
  { 
    return (((dsp_types[id] == _onedlong) ||
	     (dsp_types[id] == _onedword))? 
	    dsp_xy[id]._xchans : dsp_xy[id]._xchans * dsp_xy[id]._ychans); 
  }
  int getchannels(char *name) 
  {
    int id;
    id = getspecid(name);
    return ((id < 0)? -1: getchannels(id)); 
  }
  
  // axis max/min and label
  float getxmax_map(int id) { return dsp_map[id]._xmax; }
  float getxmin_map(int id) { return dsp_map[id]._xmin; }
  float getymax_map(int id) { return dsp_map[id]._ymax; }
  float getymin_map(int id) { return dsp_map[id]._ymin; }

  char *getxlabel_map(spec_label label, int id);
  char *getylabel_map(spec_label label, int id);

  // spectrum type
  spec_type gettype(int id);
  spec_type gettype(char *name) 
  { 
    int id;
    id = getspecid(name);
    return ((id < 0)? _undefined : gettype(id)); 
  }

  // other info
  int getspecid(char *name);
  char *getname(spec_title name, int id);
  char *getinfo(spec_title info, int id);

  // summary info
  int GetSpectrumList(char ***list);
  int GetSpectrumId(char *name);
  int GetSpectrumId(std::string name);
  spec_type GetSpectrumType(int id);
  
  Address_t CreateSpectrum(int id);
  
};



#endif
