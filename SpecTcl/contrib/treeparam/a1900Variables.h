/* 
	Variable definitions for the A1900
	The root variable name is a1900v
	Author: D. Bazin
	Date: July 2001
*/

#ifndef __TREEVARIABLE_H
#include "TreeVariable.h"
#endif

#ifndef __A1900VARIABLES_H
#define __A1900VARIABLES_H

struct pincalibrator
{
	CTreeVariable offset;
	CTreeVariable gain;
	CTreeVariable thickness;
};

struct tofcalibrator
{
	CTreeVariable offset;
	CTreeVariable slope;
};

struct pidcalibrator
{
	CTreeVariable brho1;
	CTreeVariable length1;
	CTreeVariable brho2;
	CTreeVariable length2;
};

/* now the whole fragment separator */
struct fragmentseparatorcalibrator
{
	struct pincalibrator pin;
	struct tofcalibrator rftof;
	struct pidcalibrator pid;
};

/* following the declarations, this is the actual construction
    of the variable tree structure defined in a1900v.h */
extern struct fragmentseparatorcalibrator a1900v;
#endif
