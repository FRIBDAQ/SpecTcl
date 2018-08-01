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
  nsclbinerror.h
  by Chase Bolen
  2-25-00
  These classes are error containers for nsclbin.  
  nsclfileerror and nscloperror are derived from nsclbinerror.
   
  $Log$
  Revision 5.2  2005/06/03 15:19:29  ron-fox
  Part of breaking off /merging branch to start 3.1 development

  Revision 5.1.2.1  2004/12/21 17:51:27  ron-fox
  Port to gcc 3.x compilers.

  Revision 5.1  2004/11/29 16:56:13  ron-fox
  Begin port to 3.x compilers calling this 3.0

  Revision 4.2  2003/01/02 17:11:33  venema
  Major version upgrade to 2.0 includes support for arbitrary user coordinate mapping and sticky print options.

  Revision 4.1  2002/09/13 17:30:11  ron-fox
  Add return values to functions that don't have but need

*/

#ifndef NSCLBINERROR_H
#define NSCLBINERROR_H
class nsclbinerror {
 public:
    virtual bool operator==(nsclbinerror& rhs) {return (reasoncode()==rhs.reasoncode());} 
    virtual bool operator<(nsclbinerror& rhs) {return (whatswrong<rhs.reasoncode());}
    virtual int reasoncode() {return whatswrong;}
    virtual const char* reasontext() {return (const char*)0;}
 private:
    int whatswrong;
};


class nsclfileerror :public nsclbinerror {
 public:  
  typedef enum _errorreason {
    EmptyStream,
    HeaderReadError,
    ReadErrorN,
    ReadErrorS,
    BufferWriteError,
    DataWriteError,
    BitmaskWriteError
  } errorreason;
  nsclfileerror(errorreason);
  nsclfileerror& operator=(errorreason);
  bool operator==(errorreason& rhs) {return (whatswrong==rhs);}
  errorreason geterror() {return whatswrong;}
  const char* reasontext();
  int reasoncode() {return (int)whatswrong;}
 private:
  errorreason whatswrong;
};


class nscloperror : public nsclbinerror {
 public:
  typedef enum _errorreason {
    InvalidFormat,
    InvalidHeader,
    WriteWOSetup,
    BadDataIndex,
    BadIndexGet,
    OutOfMemory
  } errorreason;
  nscloperror(errorreason);
  nscloperror& operator=(errorreason);
  bool operator==(errorreason& rhs) {return (whatswrong==rhs);}
  errorreason geterror() {return whatswrong;}

  const char* reasontext();
  int reasoncode() {return (int)whatswrong;}
 private:
  errorreason whatswrong;
};
#endif
