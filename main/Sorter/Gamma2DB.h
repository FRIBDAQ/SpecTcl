/*
		    GNU GENERAL PUBLIC LICENSE
		       Version 2, June 1991

 Copyright (C) 1989, 1991 Free Software Foundation, Inc.
                       59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.

			    Preamble

  The licenses for most software are designed to take away your
freedom to share and change it.  By contrast, the GNU General Public
License is intended to guarantee your freedom to share and change free
software--to make sure the software is free for all its users.  This
General Public License applies to most of the Free Software
Foundation's software and to any other program whose authors commit to
using it.  (Some other Free Software Foundation software is covered by
the GNU Library General Public License instead.)  You can apply it to
your programs, too.

  When we speak of free software, we are referring to freedom, not
price.  Our General Public Licenses are designed to make sure that you
have the freedom to distribute copies of free software (and charge for
this service if you wish), that you receive source code or can get it
if you want it, that you can change the software or use pieces of it
in new free programs; and that you know you can do these things.

  To protect your rights, we need to make restrictions that forbid
anyone to deny you these rights or to ask you to surrender the rights.
These restrictions translate to certain responsibilities for you if you
distribute copies of the software, or if you modify it.

  For example, if you distribute copies of such a program, whether
gratis or for a fee, you must give the recipients all the rights that
you have.  You must make sure that they, too, receive or can get the
source code.  And you must show them these terms so they know their
rights.

  We protect your rights with two steps: (1) copyright the software, and
 (2) offer you this license which gives you legal permission to copy,
distribute and/or modify the software.

  Also, for each author's protection and ours, we want to make certain
that everyone understands that there is no warranty for this free
software.  If the software is modified by someone else and passed on, we
want its recipients to know that what they have is not the original, so
that any problems introduced by others will not reflect on the original
authors' reputations.

  Finally, any free program is threatened constantly by software
patents.  We wish to avoid the danger that redistributors of a free
program will individually obtain patent licenses, in effect making the
program proprietary.  To prevent this, we have made it clear that any
patent must be licensed for everyone's free use or not licensed at all.

  The precise terms and conditions for copying, distribution and
modification follow.

		    GNU GENERAL PUBLIC LICENSE
   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

  0. This License applies to any program or other work which contains
a notice placed by the copyright holder saying it may be distributed
under the terms of this General Public License.  The "Program", below,
refers to any such program or work, and a "work based on the Program"
means either the Program or any derivative work under copyright law:
that is to say, a work containing the Program or a portion of it,
either verbatim or with modifications and/or translated into another
language.  (Hereinafter, translation is included without limitation in
the term "modification".)  Each licensee is addressed as "you".

Activities other than copying, distribution and modification are not
covered by this License; they are outside its scope.  The act of
running the Program is not restricted, and the output from the Program
is covered only if its contents constitute a work based on the
Program (independent of having been made by running the Program).
Whether that is true depends on what the Program does.

  1. You may copy and distribute verbatim copies of the Program's
source code as you receive it, in any medium, provided that you
conspicuously and appropriately publish on each copy an appropriate
copyright notice and disclaimer of warranty; keep intact all the
notices that refer to this License and to the absence of any warranty;
and give any other recipients of the Program a copy of this License
along with the Program.

You may charge a fee for the physical act of transferring a copy, and
you may at your option offer warranty protection in exchange for a fee.

  2. You may modify your copy or copies of the Program or any portion
of it, thus forming a work based on the Program, and copy and
distribute such modifications or work under the terms of Section 1
above, provided that you also meet all of these conditions:

    a) You must cause the modified files to carry prominent notices
    stating that you changed the files and the date of any change.

    b) You must cause any work that you distribute or publish, that in
    whole or in part contains or is derived from the Program or any
    part thereof, to be licensed as a whole at no charge to all third
    parties under the terms of this License.

    c) If the modified program normally reads commands interactively
    when run, you must cause it, when started running for such
    interactive use in the most ordinary way, to print or display an
    announcement including an appropriate copyright notice and a
    notice that there is no warranty (or else, saying that you provide
    a warranty) and that users may redistribute the program under
    these conditions, and telling the user how to view a copy of this
    License.  (Exception: if the Program itself is interactive but
    does not normally print such an announcement, your work based on
    the Program is not required to print an announcement.)

These requirements apply to the modified work as a whole.  If
identifiable sections of that work are not derived from the Program,
and can be reasonably considered independent and separate works in
themselves, then this License, and its terms, do not apply to those
sections when you distribute them as separate works.  But when you
distribute the same sections as part of a whole which is a work based
on the Program, the distribution of the whole must be on the terms of
this License, whose permissions for other licensees extend to the
entire whole, and thus to each and every part regardless of who wrote it.

Thus, it is not the intent of this section to claim rights or contest
your rights to work written entirely by you; rather, the intent is to
exercise the right to control the distribution of derivative or
collective works based on the Program.

In addition, mere aggregation of another work not based on the Program
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


//  CGamma2DB.h:
//
//    This file defines the CGamma2DB class.
//
// Author:
//    Jason Venema
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321
//    mailto:venemaja@msu.edu
//
//  Copyright 2001 NSCL, All Rights Reserved.
//
/////////////////////////////////////////////////////////////

/*
  Change log:
  $Log$
  Revision 5.2  2005/06/03 15:19:22  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.2  2005/05/27 17:47:37  ron-fox
  Re-do of Gamma gates also merged with Tim's prior changes with respect to
  glob patterns.  Gamma gates:
  - Now have true/false values and can therefore be applied to spectra or
    take part in compound gates.
  - Folds are added (fold command); and these perform the prior function
      of gamma gates.

  Revision 5.1.2.1  2004/12/21 17:51:24  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:56:06  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.2  2003/04/01 19:53:12  ron-fox
  Support for Real valued parameters and spectra with arbitrary binnings.

*/


#ifndef __GAMMA2DB_H  //Required for current class
#define __GAMMA2DB_H

#ifndef __CGAMMASPECTRUM_H
#include <CGammaSpectrum.h>
#endif

                               //Required for base classes
#ifndef __CAXIS_H
#include <CAxis.h>
#endif

#ifndef __SPECTRUM_H
#include "Spectrum.h"
#endif                               

#ifndef __STL_STRING
#include <string>
#ifndef __STL_STRING
#define __STL_STRING
#endif
#endif

#ifndef __STL_VECTOR
#include <vector>
#ifndef __STL_VECTOR
#define __STL_VECTOR
#endif
#endif

#ifndef __HISTOTYPES_H
#include <histotypes.h>
#endif

//  Forward class definition:

class CParameter;


class CGamma2DB : public CGammaSpectrum
{
  UInt_t m_nXScale;		//!< X channel count.
  UInt_t m_nYScale;		//!< Y Channel count.
  
public:

			//Constructor(s) with arguments

  CGamma2DB(const STD(string)& rName, UInt_t nId,
	       STD(vector)<CParameter>& rParameters,
	       UInt_t nXScale, UInt_t nYScale);

  CGamma2DB(const STD(string)& rName, UInt_t nId,
	    STD(vector)<CParameter>& rParameters,
	    UInt_t nXScale, UInt_t nYScale,
	    Float_t xLow, Float_t xHigh,
	    Float_t yLow, Float_t yHigh);


  // Constuctor for use by derived classes
  //  CGamma2DB(const STD(string)& rName, UInt_t nId,
  //    STD(vector)<CParameter>& rParameter);

  virtual  ~ CGamma2DB( ) { }       //Destructor	
private:
			//Copy constructor [illegal]

  CGamma2DB(const CGamma2DB& acspectrum1dl); 

			//Operator= Assignment Operator [illegal] 

  CGamma2DB operator= (const CGamma2DB& aCGamma1D);

			//Operator== Equality Operator [Not too useful but:]
public:
  int operator== (const CGamma2DB& aCGamma)
  { return (
	    (CGamma2DB::operator== (aCGamma)) &&
	    (m_nXScale == aCGamma.m_nXScale) &&
	    (m_Parameters == aCGamma.m_Parameters) &&
	    (m_nYScale == aCGamma.m_nYScale)
	    );
  }
  // Selectors 

public:
  UInt_t getXScale() const
  {
    return m_nXScale;
  }
  UInt_t getYScale() const
  {
    return m_nYScale;
  }

  virtual SpectrumType_t getSpectrumType() {
    return keG2D;
  }
  // Mutators (available to derived classes:

protected:
  void setXScale (UInt_t am_nScale)
  { 
    m_nXScale = am_nScale;
  }
  void setYScale(UInt_t nScale)
  {
    m_nYScale = nScale;
  }

  //
  //  Operations:
  //   
public:                 
  virtual   ULong_t operator[](const UInt_t* pIndices) const;
  virtual   void    set(const UInt_t* pIndices, ULong_t nValue);
  
  virtual void GetResolutions(STD(vector)<UInt_t>& rvResolutions);
  virtual   UInt_t Dimension (UInt_t n) const;  
  virtual   UInt_t Dimensionality () const {
    return 2;
  }

  virtual void Increment(STD(vector)<STD(pair)<UInt_t, Float_t> >& rParameters);
private:
  static Axes CreateAxisVector(STD(vector)<CParameter>& rParams,
			       UInt_t nXchan, UInt_t nYchan,
			       Float_t xLow, Float_t xHigh,
			       Float_t yLow, Float_t yHigh);
  void CreateStorage();
  void SetParameterVector(STD(vector)<CParameter>& rParameters);
};

#endif
