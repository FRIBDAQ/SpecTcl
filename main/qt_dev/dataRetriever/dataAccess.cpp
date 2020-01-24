#include <stdio.h>
#include <strings.h>
#include <memory.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <map>

#include "dataAccess.h"
#include "dataRetriever.h"

typedef char chooser_name[128];
chooser_name names[MAXSPEC];
char *(namelist[MAXSPEC]);

typedef std::pair<int, std::string> NumberAndName;

// title formatting
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

static char* cvttitle(char *dest, volatile char *src, int upper)
{
  char *p;

  memcpy(dest,(char*)src, sizeof(spec_title));  /* Grab the whole string */
  dest[sizeof(spec_title)-1] = '\0';     /* Ensure null termination */
  if(strlen(dest) > 0) {
    p = dest + strlen(dest)-1;/* Point to last character of string */

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

char*
spec_shared::getxlabel_map(spec_label label, int id)
{
  cvttitle(label, dsp_map[id]._xlabel, 0);
  return label;
}
char*
spec_shared::getylabel_map(spec_label label, int id)
{
  cvttitle(label, dsp_map[id]._ylabel, 0);
  return label;
}

spec_type
spec_shared::gettype(int id) 
{
  if( (id < 0) || (id > MAXSPEC))
    return _undefined;
  return dsp_types[id];
}

int
spec_shared::getspecid(char *name)
{
  spec_title search;
  spec_title current;
  int id;

  memcpy(search, name, sizeof(search));
  search[sizeof(search)-1] = '\0';
  upcase(search); // Make an upper case version of the name 
  
  for (id = 0; id < MAXSPEC; id++) {
    if(dsp_types[id] != _undefined) {
      cvttitle(current, dsp_titles[id], 1);
      if(strncmp(current, search, sizeof(spec_title)) == 0) return id+1;
    }
  }
  return -1;
}

char*
spec_shared::getname(spec_title name, int id)
{
  cvttitle(name, dsp_titles[id],1);
  return name;
}

char*
spec_shared::getinfo(spec_title info, int id)
{
  cvttitle(info, dsp_info[id], 1);
  return info;
}

unsigned int
spec_shared::getchannel(int id, int ix)
{
  unsigned int *lptr;
  unsigned short *sptr;

  switch(gettype(id)) {
  case _undefined:
    fprintf(stderr, "Attempted getchannel from undefined spectrum %d\n",id);
    return 0;
  case _onedlong:
    lptr = (unsigned int *)getbase(id);
    if(lptr == NULL) {
      fprintf(stderr, "Invalid spectrum base %d\n", id);
      return 0;
    }
    if((char *)&lptr[ix] > dataRetriever::getInstance()->MemoryTop()) {
      fprintf(stderr, "Invalid spectrum channel %d,%d\n", id, ix);
      return 0;
    }
    if( (ix < getxdim(id)) && (ix >= 0))
      return (unsigned int)lptr[ix];
    fprintf(stderr, "Index out of range 0-%d (was %d) in 1d getchannel\n",
	    getxdim(id), ix);
    return 0;
  case _twodlong:    
  case _twodbyte:
  case _twodword:
    fprintf(stderr,"Attempted 1d getchannel from 2-d %d\n",id-1);
    return 0;

  case _onedword:
    sptr = (unsigned short *)getbase(id);
    if(sptr == NULL) {
      fprintf(stderr, "Invalid spectrum base %d\n", id);
      return 0;
    }
    if((char *)&sptr[ix] > dataRetriever::getInstance()->MemoryTop()) {
      fprintf(stderr, "Invalid spectrum channel %d,%d\n", id, ix);
      return 0;
    }
    if( (ix < getxdim(id)) && (ix >= 0))
      return (unsigned int)sptr[ix];
    fprintf(stderr, "Index out of range 0-%d (was %d) in 1d getchannel\n",
	    getxdim(id), ix);
    return 0;
  default:
    fprintf(stderr,"Invalid spectrum type %d for spectrum %d in 1d getchannel\n", 
	    gettype(id), id);
    return 0;
  }
}

unsigned int spec_shared::getchannel(int id, int ix, int iy)
{
  unsigned int* lptr;
  unsigned short *sptr;
  unsigned char  *bptr;
  int idx;

  switch(gettype(id)) {
  case _undefined:
    fprintf(stderr,"Attempted 2d getchannel for undefined spectrum %d\n", id);
    return 0;
  case _onedlong:
    fprintf(stderr,"Attempted 2d getchannel for 1d spectrum %d\n", id);
    return 0;
  case _onedword:
    fprintf(stderr, "Attempted 2d getchannel for 1d spectrum %d\n", id);
    return 0;
  case _twodlong:
    if( (ix >= 0) && (ix < getxdim(id)) &&
        (iy >= 0) && (iy < getydim(id))) {
      idx = ix + iy*getxdim(id);
      lptr = (unsigned int *)getbase(id);
      if(lptr == NULL) {
	fprintf(stderr, "Invalid spectrum base %d \n", id);
	return 0;
      }
      if((char *)&lptr[idx] > dataRetriever::getInstance()->MemoryTop()) {
	fprintf(stderr, "Invalid spectrum channel %d,%d\n", id, idx);
	return 0;
      }
      return (unsigned int)lptr[idx];
    }
    fprintf(stderr,"Invalid index on 2d getchannel 0-%d, 0-%d, %d,%d\n",
	    getxdim(id), getydim(id), ix, iy);
    return 0;
  case _twodword:
    if( (ix >= 0) && (ix < getxdim(id)) &&
        (iy >= 0) && (iy < getydim(id))) {
      idx = ix + iy*getxdim(id);
      sptr = (unsigned short *)getbase(id);
      if(sptr == NULL) {
	fprintf(stderr, "Invalid spectrum base %d \n", id);
	return 0;
      }
      if((char *)&sptr[idx] > dataRetriever::getInstance()->MemoryTop()) {
	fprintf(stderr, "Invalid spectrum channel %d,%d\n", id, idx);
	return 0;
      }
      return (unsigned int)sptr[idx];
    }
    fprintf(stderr,"Invalid index on 2d getchannel 0-%d, 0-%d, %d,%d\n",
	    getxdim(id), getydim(id), ix, iy);
    return 0;
  case _twodbyte:
    if( (ix >= 0) && (ix < getxdim(id)) &&
        (iy >= 0) && (iy < getydim(id))) {
      idx = ix + iy*getxdim(id);
      bptr = (unsigned char *)getbase(id);
      if(bptr == NULL){ 
	fprintf(stderr, "Invalid spectrum base %d\n", id);
	return 0;
      }
      if((char *)&bptr[idx] > dataRetriever::getInstance()->MemoryTop()) {
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


unsigned int*
spec_shared::getbase(int id)
{
  unsigned int *base;
  switch(gettype(id)) {
  case _twodlong:
  case _onedlong:
    base =  &(dsp_spectra._l[dsp_offsets[id]]);
    break;
  case _onedword:
  case _twodword:
    base =  (unsigned int *)&(dsp_spectra._w[dsp_offsets[id]]);
    break;
  case _twodbyte:
    base = (unsigned int *)&(dsp_spectra._b[dsp_offsets[id]]);
    break;
  case _undefined:
  default:
    return NULL;
  }

  if(base < (unsigned int *)dataRetriever::getInstance()->GetShMem()) return NULL;
  if(base > (unsigned int *)dataRetriever::getInstance()->MemoryTop()) return NULL;

  return base;
}

// returns the id of a spectrum given the name

int
spec_shared::GetSpectrumId(char *name)
{

  char *valstart;
  int  id;
  chooser_name n;

  strcpy(n, name);
  valstart = strchr(n, '['); // Look for open 
  if(valstart == NULL) return -1; // not a valid name
  valstart += 1; // point to the digits
  id = atoi(valstart);
  if(id < 0)
    return -1; 
  return id;
}

int
spec_shared::GetSpectrumId(std::string name)
{
  for (int i = 0; i < MAXSPEC; i++) {
    if (dataRetriever::getInstance()->GetShMem()->gettype(i) != _undefined)  {
      spec_title aname;
      dataRetriever::getInstance()->GetShMem()->getname(aname, i);
      if (strcasecmp(aname, name.c_str()) == 0) {
	return i;
      }
    }
  }
  return -1;
}

// returns size and spectrum list
int
spec_shared::GetSpectrumList(char ***list)
{

  spec_title aname;
  std::map<std::string, NumberAndName> NameInfo;

  // First a list of spectrum name pointers is generated for the defined spectra:
  int i;
  for(i = 0; i < MAXSPEC; i++) {
    if(dataRetriever::getInstance()->GetShMem()->gettype(i) != _undefined) {
      dataRetriever::getInstance()->GetShMem()->getname(aname, i); 
      if(strlen(aname) == 0) {        // spectra don't require names...
	strcpy(aname, "<Untitled>");
      }
      NameInfo[std::string(aname)] = NumberAndName(i, std::string(aname));
    }
  }
  // Pull the names out of the NumberAndName map (they'll be sorted) and encode them into names:

  int nspec =0;
  for(std::map<std::string,NumberAndName>::iterator i = NameInfo.begin(); i != NameInfo.end(); i++) {
    int type = dataRetriever::getInstance()->GetShMem()->GetSpectrumType(i->second.first);
    int nbins_x = dataRetriever::getInstance()->GetShMem()->getxdim(i->second.first)-2;
    float xmin = dataRetriever::getInstance()->GetShMem()->getxmin_map(i->second.first)+1;
    float xmax = dataRetriever::getInstance()->GetShMem()->getxmax_map(i->second.first)-1;    
    int nbins_y = 0;
    float ymin = 0;
    float ymax = 0;    
    if (dataRetriever::getInstance()->GetShMem()->getydim(i->second.first) != 0){
      nbins_y = dataRetriever::getInstance()->GetShMem()->getydim(i->second.first)-2;    
      ymin = dataRetriever::getInstance()->GetShMem()->getymin_map(i->second.first)+1;
      ymax = dataRetriever::getInstance()->GetShMem()->getymax_map(i->second.first)-1;
    }
    //    sprintf(names[nspec], "[%05d] %s %d (%d,%.1f,%.1f),(%d,%.1f,%.1f)", i->second.first, i->first.c_str(), type, nbins_x, xmin, xmax, nbins_y, ymin, ymax);
    sprintf(names[nspec], "%s",i->first.c_str());
    nspec++;
  }

  // Build the pointer name list
  
  for(i = 0; i < nspec; i ++)
    namelist[i] = names[i];
  *list = namelist;
  return nspec;

}

spec_type
spec_shared::GetSpectrumType(int id)
{
  return (spec_type)(dataRetriever::getInstance()->GetShMem()->gettype(id));
}

Address_t
spec_shared::CreateSpectrum(int id)
{
  uint32_t nOffset = dataRetriever::getInstance()->GetShMem()->dsp_offsets[id];
  
  switch(dataRetriever::getInstance()->GetShMem()->dsp_types[id]) { 
  case _twodlong:
  case _onedlong:// Scale the offset appropriately.
    nOffset = nOffset*sizeof(int32_t);
    break;
  case _onedword:
  case _twodword:
    nOffset = nOffset*sizeof(int16_t);
    break;
  case _twodbyte:
    break;
  default:// Spectrum undefined...
    return NULL;// Don't do anything else.
  }

  Address_t p_storage = (Address_t)(dataRetriever::getInstance()->GetShMem()->dsp_spectra._b + nOffset);

  char title[sizeof(spec_title) + 1];
  memset(title, 0, sizeof(spec_title)+1);
  strncpy(title, (char*)dataRetriever::getInstance()->GetShMem()->dsp_titles[id], sizeof(spec_title));

  std::cout << "Title: " << title << std::endl;
  return p_storage;
}
