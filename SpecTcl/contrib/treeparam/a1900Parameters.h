/* 
	Parameter definitions for the A1900
	The root parameter name is a1900
	Author: D. Bazin
	Date: July 2001
*/

#ifndef __TREEPARAMETER_H
#include "TreeParameter.h"
#endif

#ifndef __A1900PARAMETERS_H
#define __A1900PARAMETERS_H

/* focal plane parameters */
struct fpppac
{
	CTreeParameter down;
	CTreeParameter up;
	CTreeParameter left;
	CTreeParameter right;
	CTreeParameter anode;
	CTreeParameter time;
	CTreeParameter sumx;
	CTreeParameter sumy;
	CTreeParameter sumxy;
	CTreeParameter x;
	CTreeParameter y;
};

struct fppin
{
	CTreeParameter de;
	CTreeParameter time;
	CTreeParameter mev;
};

struct fpscint
{
	CTreeParameter enorth;
	CTreeParameter esouth;
	CTreeParameter timenorth;
	CTreeParameter timesouth;
	CTreeParameter e;
};

struct silicon
{
	CTreeParameter de;
	CTreeParameter mev;
};

struct focalplane
{
	struct fpppac ppac0;
	struct fpppac ppac1;
	CTreeParameter x;
	CTreeParameter y;
	CTreeParameter theta;
	CTreeParameter phi;
	struct fppin pin;
	struct silicon stack[4];
	struct fpscint scint;
};

/* image 2 parameters */
struct im2ppac
{
	CTreeParameterArray anode;
	CTreeParameterArray time;
	CTreeParameterArray cathode;
	CTreeParameter x;
	CTreeParameter y;
};

struct im2scint
{
	CTreeParameter denorth;
	CTreeParameter desouth;
	CTreeParameter timenorth;
	CTreeParameter timesouth;
};

struct image2
{
	struct im2ppac ppac0;
	struct im2ppac ppac1;
	struct im2scint scint;
	CTreeParameter x;
	CTreeParameter y;
	CTreeParameter theta;
	CTreeParameter phi;
};

/* particle identification parameters */
struct timeofflight
{
	CTreeParameter rfpin;
	CTreeParameter rfscint;
	CTreeParameter im2fp;
};

struct particleidentification
{
	struct timeofflight tof;
	CTreeParameter Z;
	CTreeParameter AoQ;
	CTreeParameter A;
};

/* now the whole fragment separator */
struct fragmentseparator
{
	CTreeParameter rf;
	struct focalplane fp;
	struct image2 im2;
	struct particleidentification pid;
};

/* following the declarations, this is the actual construction
    of the parameter tree structure which is done in a1900.h*/
extern struct fragmentseparator a1900;
#endif
