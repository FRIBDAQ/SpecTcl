#include <iomanip.h>
#include <iostream.h>
#include <errno.h>
#include <nsclbinerror.h>
/* 
      nsclbin.h
      by Chase Bolen
      2-25-00

   The classes in this file deal with the actual data of a 
   musort or smaug formatted spectrum.
   spcbin is the base class from which spcin and spcout are derived
   from spcin are mushort, muint, smaugbyte, smaugshort, and smaugint.
   These are for input of musort word, musort longword, smaug byte, smaug word, and
       smaug longword respectivlely.
   From spcout: smaugwritebyte, smaugwriteshort, and smaugwritelong
   For writing smaug byte, smaug word, and smaug longword spectra.
   There are no utilities for writing musort spectra.
   Both mushort and muint use a template class called muintemplate<T>
   Mushort creates a muintemplate<short> object, and muint creates muintemplate<int>
   The other classes work similarly with smaugintemplate<T> and writetemplate<T>

*/
#ifndef SPCBIN_
#define SPCBIN_
static const int maskbytes = 1024;
typedef char bitmask[maskbytes];
class spcbin {
 public:
  virtual ~spcbin() {};
  virtual long get(int, int) = 0;        //R: gets the value of one channel
  virtual void put(int, int)= 0;         //W: puts a value into a channel
  virtual void put(int, int, int)= 0;    //W: same as previous but using two index
  virtual int readin(istream&) = 0;      //R: reads in data channels into an array
  virtual bool writeout(ostream&) = 0;   //W: writes out the data array into the ostream
  int sizeofbitmask(int x, int y = 1) {  //RW: returns the number of bitmask records
    if (y==0) y=1;                       //    needed for the spectrum size
    return (x*y-1)/8192+1;
  }  
  void reporterror(nsclfileerror*);
  void reporterror(nscloperror*);
  
 private:
  bool failset, goodset;
  
};

class spcin : public spcbin{ 
 public:
  virtual ~spcin() {};
  virtual int readin(istream&) = 0;
  virtual long get(int, int = 0) = 0;
 private:
  void put(int a, int b) {}
  void put(int a, int b, int c) {}
  bool writeout(ostream& a) {}
  
};


template <class T>
class muintemplate : public spcin {
 public:
  muintemplate(int, int);
 ~muintemplate();
  int readin(istream&);
  long get(int, int=0);
 private: 
  
  int xlength, ylength;
  T *data; 
//  istream Binary;
  
};






class mushort : public spcin {
 public:
  mushort(int, int);
  ~mushort();
  int readin(istream&);
  long get(int, int=0);
  
 private: 
  muintemplate<short> *templateobj;
  
};

class muint : public spcin {
 public:
  muint(int, int);
  ~muint();
  int readin(istream&);
  long get(int, int=0);
 private:
  muintemplate<int> *templateobj;
};




template <class T>
class smaugintemplate : public spcin {
 public:
  smaugintemplate(int, int);
  ~smaugintemplate();
  int readin(istream&);
  long get(int, int=0);
  
 private: 
  int xlength, ylength;
  T *data; 
//  istream Binary;
  
};





class smaugshort : public spcin {
 public:
  smaugshort(int, int);
  ~smaugshort();
  int readin(istream&);
  long get(int, int=0);
  
 private: 
  smaugintemplate<short> *templateobj;
  
};


class smaugint : public spcin {
 public:
  smaugint(int, int);
  ~smaugint();
  int readin(istream&);
  long get(int, int=0);
  
 private: 
  smaugintemplate<int> *templateobj;
};


class smaugbyte : public spcin {
 public:
  smaugbyte(int, int);
  ~smaugbyte();
  int readin(istream&);
  long get(int, int=0);
  
 private: 
  smaugintemplate<char> *templateobj;
};


class spcout : public spcbin{ 
 public:
  virtual ~spcout() {};
  virtual bool writeout(ostream&) = 0;
  virtual void put(int, int = 0) = 0;
  virtual void put(int, int, int) = 0;
  void reporterror(char er[]) {
    cerr <<er<<endl;
  }
  int sizeofbitmask(int x, int y) {
    if (y==0) y=1;
    return (x*y-1)/8192+1;
  }
 private:
  long get(int a, int b) {}
  int readin(istream& a) {}
  
  
};

template <class T>
class writetemplate : public spcout {
 public:
  writetemplate(int, int = 1);
  ~writetemplate();
  bool writeout(ostream&);
  void put(int, int= 0);
  void put(int, int, int);
 private:
  int xlength, ylength;
  T temp, *data;
  
};


class smaugwriteint : public spcout {
 public:
  smaugwriteint(int, int);
  ~smaugwriteint();
  void put(int, int);
  void put(int, int, int);
  bool writeout(ostream&);
 private:
  writetemplate<int> *spectout;
};

class smaugwriteshort : public spcout {
 public:
  smaugwriteshort(int, int);
  ~smaugwriteshort();
  void put(int, int);
  void put(int, int, int);
  bool writeout(ostream&);
 private:
  writetemplate<short> *spectout;
};

class smaugwritebyte : public spcout {
 public:
  smaugwritebyte(int, int);
  ~smaugwritebyte();
  void put(int, int);
  void put(int, int, int);
  bool writeout(ostream&);
 private:
  writetemplate<char> *spectout;
};


#endif
