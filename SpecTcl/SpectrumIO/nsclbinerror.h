/*
  nsclbinerror.h
  by Chase Bolen
  2-25-00
  These classes are error containers for nsclbin.  
  nsclfileerror and nscloperror are derived from nsclbinerror.
   
  $Log$
  Revision 4.1  2002/09/13 17:30:11  ron-fox
  Add return values to functions that don't have but need

*/

#ifndef NSCLBINERROR_
#define NSCLBINERROR_
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
  typedef enum errorreason {
    EmptyStream,
    HeaderReadError,
    ReadErrorN,
    ReadErrorS,
    BufferWriteError,
    DataWriteError,
    BitmaskWriteError
  };
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
  typedef enum errorreason {
    InvalidFormat,
    InvalidHeader,
    WriteWOSetup,
    BadDataIndex,
    BadIndexGet,
    OutOfMemory
  };
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
