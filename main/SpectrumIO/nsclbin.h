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

#include <string.h>
#include <errno.h>
#include <nscldata.h>
#include <nsclbinerror.h>

#ifndef __ISTREAM_DAQH
#include <istream>
#endif

#ifndef __OSTREAM_DAQH
#include <ostream>
#endif

/*    
      nsclbin.h
      Chase Bolen
      2-25-00

  the nsclbin class is for reading smaug and musort binary spectrum files
  it deals with the header, and uses the spcbin class to do the work of getting data.
  

*/
#ifndef NSCLBIN_
#define NSCLBIN_
class nsclbin {
 public:
  nsclbin(std::istream*);
  nsclbin(std::ostream*);
  ~nsclbin();
  void getname(char[]);
  void getdate(char[]);
  void gettime(char[]);
  void getdata(std::istream&);
  int getylength();
  int getxlength();
  int getelements();
  int getformat();
  int getdimension();
  int getcount();
  long get(int, int = 0);
  void setname(char[]);
  void setname(const char[]);
  void setdate(char[]);
  void setdate(const char[]);  
  void settime(char[]);
  void settime(const char[]);
  void setdimension(int);
  void setxlength(int);
  void setylength(int);
  void setformat(int);
  void setcount(int);
  void setelements(int);
  bool testinit();
  bool setup();
  bool put(int,int);
  bool put(int, int, int);
  std::ostream& writeout(std::ostream&);
  
 private:
  void testdata();
  short calcformat(short, short);
  void error(nscloperror*);
  void error(nsclfileerror*);
  void error();			// Put out error message from errno.
  void muset();
  void smaugset();
  void rtrim(char[],char);
  void padstream(std::ostream&);
  void readpad(std::istream&);
  //  data members ****************
  //  istd::ostream Binary;

  char head[121], time[9], date[11], formatinfo[131];
  short dimension, muformat, smaugformat, xlength, ylength, truey, format,buffer[512];
  static const short LONGWORD = 4, WORD=2, BYTE=1, ASCSPACE=32;
  bool setupdone, cleanupdata;
  int elements, totcount;
  spcbin *data;
  nsclbinerror *errorholder;
  const static double recordsize;  //1k record sizes
   
};





#endif



