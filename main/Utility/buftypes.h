/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


/*
**++
**  FACILITY:
**
**      Data acquisition system front end
**
**  ABSTRACT:
**
**      This file symbolically defines all of the buffer types which
**	can be sent back from the front end.
**
**  AUTHORS:
**
**      Ron Fox
**
**
**  CREATION DATE:     8-Oct-1987
**
**  MODIFICATION HISTORY:
**      @(#)buftypes.h	1.2 3/25/94 Include
**--
**/

#ifndef BUFTYPES_H
#define BUFTYPES_H

#define DATABF	1
#define SCALERBF  2
#define SNAPSCBF	3
#define STATEVARBF      4
#define RUNVARBF        5
#define PKTDOCBF        6
#define BEGRUNBF  11
#define ENDRUNBF  12
#define PAUSEBF   13
#define RESUMEBF  14
#define LINKLOSTBF 15
#define PARAMDESCRIP  30	/* Filter data parameter descr. */
#define MAXSYSBUFTYPE 31
#define PACKBUFBASE     32
#define TAPEDPACKEDBASE 32
#define USERBUF1	32
#define USERBUF2	33
#define USERBUF3	34
#define USERBUF4	35
#define USERBUF5	36
#define USERBUF6	37
#define USERBUF7	38
#define USERBUF8	39
#define TAPEDPACKEDTOP	39
#define USERBUF9	40
#define USERBUF10	41
#define USERBUF11	42
#define USERBUF12	43
#define USERBUF13	44
#define USERBUF14	45
#define USERBUF15	46
#define USERBUF16	47
#define PACKBUFTOP      47
#define UPACKBUFBASE	48
#define TAPEDUPACKBASE  48
#define USERBUF17	48
#define USERBUF18	49
#define USERBUF19	50
#define USERBUF20	51
#define USERBUF21	52
#define USERBUF22	53
#define USERBUF23	54
#define USERBUF24	55
#define USERBUF25	56
#define USERBUF26	57
#define TAPEDUPACKTOP   57
#define USERBUF27	58
#define USERBUF28	59
#define USERBUF29	60
#define USERBUF30	61
#define USERBUF31	62
#define USERBUF32	63
#define USERBUF33	64
#define UPACKBUSTOP     64
#define MAXBUFTYPE    65	/* Support up to this many buffer types	*/ /*2*/

#endif