#ifndef DATATYPES_H
#define DATATYPES_H

#ifndef MB
#define MB 1024*1024
#endif

#include <sys/types.h>

#ifndef PAGESIZE
#ifdef HAVE_DECL_PAGESIZE
#include <limits.h>
#else
#define PAGESIZE 8192
#endif
#endif

#ifndef __CRT_STDINT_H
#include <stdint.h>
#ifndef __CRT_STDINT_H
#define __CRT_STDINT_H
#endif
#endif

#ifndef PAGESIZE
#define PAGESIZE 8192
#endif

#define MAXSPEC 10000
#ifndef SPECBYTES
#define SPECBYTES 8*MB
#endif

#define WORDS     (SPECBYTES)/sizeof(short)
#define LONGS     (SPECBYTES)/sizeof(int)

// spectrum dimension type
typedef struct {
  unsigned int _xchans;
  unsigned int _ychans;
} spec_dimension;

// spectrum name
typedef char spec_title[128];
typedef spec_title spec_label; //spectrum info

typedef struct _statistics {
  unsigned int _overflows[2];
  unsigned int _underflows[2];
} Statistics, *pStatistics;

typedef enum {
  _undefined = 0,
  _twodlong = 5,
  _onedlong = 4,
  _onedword = 2,
  _twodword = 3,
  _twodbyte = 1
} spec_type;

typedef struct {
  float _xmin;
  float _xmax;
  float _ymin;
  float _ymax;
  spec_label _xlabel;
  spec_label _ylabel;
} spec_map;

// spectrum storage type
typedef union {
  uint8_t   _b[SPECBYTES];
  uint16_t  _w[WORDS];
  uint32_t  _l[LONGS];
} spec_storage;

typedef struct _shared {
  spec_dimension  dsp_xy[MAXSPEC];
  spec_title      dsp_titles[MAXSPEC];
  spec_title      dsp_info[MAXSPEC];
  unsigned int    dsp_offsets[MAXSPEC];
  spec_type       dsp_types[MAXSPEC];
  spec_map        dsp_map[MAXSPEC];
  Statistics      dsp_statistics[MAXSPEC];
  spec_storage    dsp_spectra;
  char            page_pad[PAGESIZE];
} shared_memory;

#endif
