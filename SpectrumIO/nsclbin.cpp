#include <string.h>
#include <errno.h>
#include <nscldata.h>
#include <nscltime.h>
#include <nsclbin.h>
#include <nsclbinerror.h>
#include <math.h>
/*
      nsclbin.cc
      Chase Bolen
      2-25-00

  see nsclbin.h for an explanation of these classes


*/

nsclbin::nsclbin(istream& Binary) {
  
  dimension = xlength = ylength = format = totcount = -1;
  memset(head, 0,sizeof(head));
  memset(time, 0, sizeof(time));
  memset(date, 0, sizeof(date));
  memset(formatinfo, 0, sizeof(formatinfo));
  memset(buffer,0,sizeof(buffer));
  cleanupdata=0;
}
nsclbin::~nsclbin() {
  
  if (cleanupdata) delete data;
}

void nsclbin::error() {
  cerr << strerror(errno) << endl;
  //exit(1);
}

void nsclbin::error(nscloperror *err) {  
  throw(err);
}
void nsclbin::error(nsclfileerror *err) {
  throw(err);
}

void nsclbin::getdata(istream& Binary) {
  
  if (Binary.fail()) {
    error((nsclfileerror*)new nsclfileerror(nsclfileerror::EmptyStream));
  }
  Binary.read(buffer, sizeof(buffer));

  if (Binary.fail()) {
    error((nsclfileerror*)new nsclfileerror(nsclfileerror::HeaderReadError));
  }
  memcpy(head,&(buffer[0]), sizeof(head)-1);
  rtrim(head,' ');
  memcpy(time, &(buffer[61]), sizeof(time)-1);
  memcpy(date, &(buffer[65]), sizeof(date)-1);
  dimension = buffer[71];
  xlength = buffer[73];   ylength = buffer[75];
  muformat= buffer[77];
  smaugformat = buffer[81];
  memcpy(&elements, &(buffer[82]), sizeof(elements));
  memcpy(formatinfo, &(buffer[112]), sizeof(formatinfo)-1);
  totcount = ((int)buffer[449] & 0xffff) + (((int)buffer[448]) << 16);
  if (((dimension != 1) && (dimension !=2) ||
      (xlength <0) || (ylength <0))) {
    error((nscloperror*)new nscloperror(nscloperror::InvalidHeader));
  }
  format = calcformat(muformat, smaugformat);
  if (smaugformat == 0) {
    muset();
  } else {
    smaugset();
  }
  
  data->readin(Binary);
  readpad(Binary);
  int readok = Binary.fail();
  if (readok) {
    error((nsclfileerror*)new nsclfileerror(nsclfileerror::ReadErrorN) );
  }
  //testdata();
}

long nsclbin::get(int x, int y = 0) {
  
  long dat = data->get(x,y); 
  return dat;
}


void nsclbin::getname(char name[120]) {
  strcpy(name, head);
}

void nsclbin::gettime(char ptrtime[9]) {
  strcpy(ptrtime, time);
}

void nsclbin::getdate(char ptrdate[10]) {
  strcpy(ptrdate, date);
}

int nsclbin::getylength() {
    return ylength;
}
int nsclbin::getxlength() {
    return xlength;
}
int nsclbin::getdimension() {
    return dimension;
}

int nsclbin::getformat() {
    return format;
}

int nsclbin::getcount() {
  return totcount;
}
int nsclbin::getelements() {
  return elements;
}

void nsclbin::testdata () {
  cout << "data is "<<dimension<<"D"<<endl;
  cout <<endl<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
  cout << "header: "<< head<<endl;
  cout << "time: "<< time<<endl;
  cout << "date: "<< date<<endl;
  cout << "dimension:" <<dimension<<endl;
  cout << "xlength: "<< xlength<<endl;
  cout << "ylength: "<< ylength<<endl;
  cout << "musort format: "<< muformat<<endl;
  cout << "smaug format: " <<smaugformat<<endl;
  cout << "overall format: "<<format<<endl;
  cout << "file format info: "<<formatinfo<<endl;
  cout << "elements: "<<elements<<endl;
  cout << "total count: "<<totcount<<endl;
  cout <<endl<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
}
short nsclbin::calcformat(short musort, short smaug) {
  if (smaug == 0) {
    musort = 6-(musort*2);
    if ((musort != 2) &&(musort !=4)) {
    error((nscloperror*)new nscloperror(nscloperror::InvalidFormat) );
    }
    return musort;
  } else {
    if ((smaug !=BYTE)&&(smaug !=WORD)&&(smaug !=LONGWORD)) {
      error((nscloperror*)new nscloperror(nscloperror::InvalidFormat) );
    }
    return smaug;
  }
  
}

void nsclbin::muset() {
  
  if (format == LONGWORD) {
    data = new muint(xlength, ylength);
    cleanupdata=1;
  } else if (format == WORD) {
    data = new mushort(xlength, ylength);
    cleanupdata=1;
  } else {    
    error((nscloperror*)new nscloperror(nscloperror::InvalidFormat) );
  }
}

void nsclbin::smaugset() {
  
  if (format == LONGWORD) {
    data = new smaugint(xlength, ylength);
    cleanupdata=1;
  } else if (format == WORD) {
    data = new smaugshort(xlength, ylength);
    cleanupdata=1;
  } else if (format == BYTE) {
    data = new smaugbyte(xlength, ylength);
    cleanupdata=1;
  } else {
    error((nscloperror*)new nscloperror(nscloperror::InvalidFormat) );
} 
}







nsclbin::nsclbin(ostream& Binary) {

  dimension = xlength = ylength = format = totcount = elements-1;
  memset(head, 0,sizeof(head));
  memset(head, ASCSPACE,sizeof(char)*80);
  head[sizeof(head)-1]=ASCSPACE;
  memset(time, 0, sizeof(time));
  memset(date, 0, sizeof(date));
  memset(formatinfo, 0, sizeof(formatinfo));
  memset(buffer,0,sizeof(buffer));
  setupdone=cleanupdata=0;

} 

void nsclbin::setname(char namein[]) { 
  strcpy(head, namein);
  head[strlen(namein)]=ASCSPACE;
  memcpy(&(buffer[0]),head, sizeof(head)-1);
}
void nsclbin::setname(const char namein[]) {
  strcpy(head, namein); 
  head[strlen(namein)]=0;
  memcpy(&(buffer[0]),head, sizeof(head)-1);
}
void nsclbin::setdate(char datein[]) {
  strcpy(date, datein);
  memcpy(&(buffer[65]),date,sizeof(date)-1);
}
void nsclbin::setdate(const char datein[]) {
  strcpy(date, datein);
  memcpy(&(buffer[65]),date,sizeof(date)-1);
}
void nsclbin::settime(char timein[]) {
  strcpy(time, timein);
  memcpy(&(buffer[61]), time, sizeof(time)-1); 
}
void nsclbin::settime(const char timein[]) {
  strcpy(time, timein);
  memcpy(&(buffer[61]), time, sizeof(time)-1); 
}
void nsclbin::setdimension(int dimin) {
  dimension=dimin;
  buffer[71]=dimension;
}
void nsclbin::setxlength(int xin) {
  xlength=xin;
  buffer[73]=xlength;
}
void nsclbin::setylength(int yin) {
  ylength=yin;
  buffer[75]=ylength;
}
void nsclbin::setformat(int formin) {

  smaugformat=format= formin;
  muformat = (6-smaugformat)/2;
  if (smaugformat>0) muformat = 4;  //this is just to be consistant with older smaug files
  buffer[77]=muformat;      
  buffer[81]=smaugformat;

}
void nsclbin::setcount(int countin) {
  totcount=countin;
  short l=(short)(totcount<<16),u=(short)((totcount-l)&0xffff);
  buffer[448]=l;
  buffer[449]=u;
}
void nsclbin::setelements(int elin) {
  elements = elin;
  memcpy(&(buffer[82]),&(elements),sizeof(elements));
}
bool nsclbin::testinit() {
  if ((dimension !=2)&&(dimension!=1)) return 0;
  if ((xlength<=0)||(ylength<0)||
      ((ylength!=0)&&(dimension==1))||
      ((ylength==0)&&(dimension==2))) return 0;
  if ((format!=BYTE)&&(format!=WORD)&&(format!=LONGWORD)) return 0;
  return 1;
}
bool nsclbin::setup() {
  char finfo[131]= "CANPS FORMAT 1.0.1986 NSCL-MSU; SPCLIB; VAX/VMN; MAX REC LENGTH=1KB  FOLLOWING RECORDS ARE OF TYPE";
  int sizeofbitmask(int x, int y);
  if (testinit()) {
    truey = (ylength==0);
    if (ylength > truey) truey=ylength;
    if (time[0]==0) chartime(time);
    if (date[0]==0) chardate(date);
    int s = sizeofbitmask(xlength,ylength);
    char ustr[9*s];
    memset(ustr,0,9*s);
    for(int i=0; i<s;i++) {
      strcat(ustr, " U1");
    }
     switch (format) {
    case LONGWORD:
      strcat(ustr, " H4");
      break;
    case WORD:
      strcat(ustr, " H2");
      break;
    case BYTE:
      strcat(ustr, " H1");
      break;
    default:
      error((nscloperror*)new nscloperror(nscloperror::InvalidFormat) );
    }
    strcat(finfo,ustr);
    
    memset(formatinfo,ASCSPACE,sizeof(formatinfo)-1);
    strcpy(formatinfo, finfo);
    formatinfo[strlen(finfo)]=ASCSPACE;
    memcpy(&(buffer[61]), time, sizeof(time)-1);
    memcpy(&(buffer[65]),date,sizeof(date)-1);
    memcpy(&(buffer[112]),formatinfo, sizeof(formatinfo)-1);
    if (setupdone) delete data;
    switch (format) {
    case LONGWORD:
      data = new smaugwriteint(xlength, ylength);
      cleanupdata=1;
      break;
    case WORD:
      data = new smaugwriteshort(xlength, ylength);
      cleanupdata=1;
      break;
    case BYTE:
      data = new smaugwritebyte(xlength, ylength);
      cleanupdata=1;
      break;
    default:
      error((nscloperror*)new nscloperror(nscloperror::InvalidFormat) );
    }
    setupdone=1;
    return 1;
  }
  return 0;
}
bool nsclbin::put(int xindex, int yindex, int val) {
  if ((setupdone)&&(xindex<xlength)&&(xindex>=0)&&(yindex<truey)&&(yindex>=0)) {
  
    data->put(xindex, yindex, val);
    return 1;
  }
  return 0;
}
bool nsclbin::put(int index, int val) {
  if ((setupdone)&&(index<xlength*truey)&&(index>=0)) {
  
    data->put(index,val);
    return 1;
  }
  return 0;
}


ostream& nsclbin::writeout(ostream& Binary) {
  if (setupdone) {
    Binary.write((char*)buffer, (long)sizeof(buffer));
    if (Binary.fail()) {
      error((nsclfileerror*)new nsclfileerror(nsclfileerror::BufferWriteError) );
    }
    data->writeout(Binary);
    padstream(Binary);
    return Binary;
  }
  error((nscloperror*)new nscloperror(nscloperror::WriteWOSetup) );
}
void nsclbin::rtrim(char str[120],char trimchar=' ') {
  for(int i = strlen(str); i>=0;i--) {
    if ((str[i]!=trimchar)&&(str[i]!='\0')) break;
    str[i]=0;
  }
}
inline int sizeofbitmask(int x, int y = 1) {
  if (y==0) y=1;
  return (x*y-1)/8192+1;
}
//void padstream(ostream& Binary)
//
//adds a bit of null padding on the end of the file to bring the file size to a 
//  multiple of 1k.  this is done to be backwards compatible with the older nsclbinary
//  file readers which expect files written with record sizes of 1k.  
//
void nsclbin::padstream(ostream& Binary) {
  double current = Binary.tellp(), padpos = (ceil(current/recordsize))*recordsize;
  char temp[(unsigned)(padpos-current)];
  memset(temp,0,sizeof(char)*(unsigned)(padpos-current));
  Binary.write(temp, (long)sizeof(char)*(unsigned)(padpos-current));
}
void nsclbin::readpad(istream& Binary) {
  double current = Binary.tellg(), padpos = (ceil(current/recordsize))*recordsize;
  char temp[(unsigned)(padpos-current)];
  Binary.read(temp, (long)sizeof(char)*(unsigned)(padpos-current));
}
