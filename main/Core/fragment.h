/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2009.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#ifndef FRAGMENT_H
#define FRAGMENT_H

#include <config.h>
#include <stdint.h>
#include <sys/types.h>

/**
 * The conditional directives in this file are for two reasons:
 * - In C++ sources, the headers will qualify the type and function
 *   definitions with the ::EVB:: namepsace.
 * - The implementations of the support functions are in C
 *
 * All of this is to support C programmers as well as C++.
 */

#ifdef __cplusplus
namespace EVB {
#endif
  /*
   *  Below are valid barrier types.
   *  These are #defines rather than enums so that the
   *  known width data types are used to make data transportable
   *  between 32/64 bit system.
   */

#define BARRIER_NOTBARRIER   0	/* Not a barrier event. */
#define BARRIER_START        1	/* Data taking starting (BEGIN/RESUME) */
#define BARRIER_END          2	/* Data taking endng (END/PAUSE) */
#define BARRIER_SYNCH        3  /* time synchronization barrier */


  /* Define the null timestamp as a 64 bits with all bits set  */

  static const uint64_t NULL_TIMESTAMP = 0xffffffffffffffff;

  /**
   *  The typedef below defines a fragment header.
   */ 
  
  typedef struct __attribute__((__packed__))_FragmentHeader {
    uint64_t       s_timestamp;	//< Fragment time relative to globally synchronized clock.
    uint32_t       s_sourceId ;	//< Unique source identifier.
    uint32_t       s_size;	// Bytes in fragment payload.
    uint32_t       s_barrier;   // Non zero for barrier events - the barrier type.
  } FragmentHeader, *pFragmentHeader;


  
  /**
   * Within the event builder fragments and payloads get bundled
   * together into something that looks like:
   */
  typedef struct __attribute__((__packed__)) _Fragment {
    FragmentHeader   s_header;
    uint8_t*           s_pBody;
  } Fragment, *pFragment;


  /**
   * Linked list of fragments:
   */
  typedef struct __attribute__((__packed__)) _FragmentChain {
    struct _FragmentChain*    s_pNext;
    pFragment         s_pFragment;
  } FragmentChain, *pFragmentChain;


  typedef struct __attribute__((__packed__)) _FlatFragment {
    FragmentHeader s_header;
    uint8_t            s_body[];
  } FlatFragment, *pFlatFragment;

#ifdef __cplusplus
}
#endif
  /**
   * Below are convenience functions for fragments:
   */

#ifdef __cplusplus
  extern "C" {
#define NS(type) EVB::type
#else
#define NS(type) type
#endif
    void freeFragment(NS(pFragment) p);
    NS(pFragment) allocateFragment(NS(pFragmentHeader) pHeader);
    NS(pFragment) newFragment(uint64_t timestamp, uint32_t sourceId, uint32_t size);

    size_t fragmentChainLength(NS(pFragmentChain) p);
#ifdef __cplusplus
  }
#endif
 #undef NS

#endif
