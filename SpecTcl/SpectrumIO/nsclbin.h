
#include <string.h>
#include <errno.h>
#include <nscldata.h>
#include <nsclbinerror.h>
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
  nsclbin(istream*);
  nsclbin(ostream*);
  ~nsclbin();
  void getname(char[]);
  void getdate(char[]);
  void gettime(char[]);
  void getdata(istream&);
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
  ostream& writeout(ostream&);
  
 private:
  void testdata();
  short calcformat(short, short);
  void error(nscloperror*);
  void error(nsclfileerror*);
  void error();			// Put out error message from errno.
  void muset();
  void smaugset();
  void rtrim(char[],char);
  void padstream(ostream&);
  void readpad(istream&);
  //  data members ****************
  //  iostream Binary;

  char head[121], time[9], date[11], formatinfo[131];
  short dimension, muformat, smaugformat, xlength, ylength, truey, format,buffer[512];
  static const short LONGWORD = 4, WORD=2, BYTE=1, ASCSPACE=32;
  bool setupdone, cleanupdata;
  int elements, totcount;
  spcbin *data;
  nsclbinerror *errorholder;
  const static double recordsize = 1024;  //1k record sizes
   
};





#endif



