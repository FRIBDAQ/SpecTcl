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


static const char* Copyright = "(C) Copyright Michigan State University 2009, All rights reserved";
#include <config.h>
#include <iomanip>
#include <iostream>

#include <errno.h>
#include <string.h>
#include <nscldata.h>
#include <nsclbinerror.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif


/*
      nscldata.cc
      by Chase Bolen
      2-25-00

      see nscldata.h for an explanation of these classes



 */

void spcbin::reporterror(nsclfileerror *err) {
  throw (err);
}
void spcbin::reporterror(nscloperror *err) {
  throw (err);
}

template <class T>
muintemplate<T>::muintemplate(int x, int y = 1): xlength(x), ylength(y) {
  if (ylength == 0) ylength = 1;
  if ((xlength>0) && (ylength >0)) {
    data = new T[xlength * ylength];
  } else {
    reporterror((nscloperror*)new nscloperror(nscloperror::BadIndexGet ) );
  }
  if (data==0) {
    spcbin::reporterror((nscloperror*)new nscloperror(nscloperror::OutOfMemory ) );
  }
}

template <class T>
muintemplate<T>::~muintemplate() {
  delete []data;
}

template <class T>
int muintemplate<T>::readin(istream& Binary) {
  T temp;
  int ReadOk;
  Binary.read((char*)data,sizeof(temp)*xlength*ylength);
  if (Binary.fail()) {
    reporterror((nsclfileerror*)new nsclfileerror(nsclfileerror::ReadErrorS ) );
  }
  ReadOk = Binary.fail();
  return ReadOk;
}

template <class T>
long muintemplate<T>::get(int xloc, int yloc) {
  
  if ((xloc >=0) && (yloc >=0)) {

    return data[yloc * xlength + xloc];
  } else {
    reporterror((nscloperror*)new nscloperror(nscloperror::BadIndexGet ) );
  }
  return 0;
}

mushort::mushort(int x, int y){
  templateobj = new muintemplate<short>(x,y);
}

mushort::~mushort() {
  delete templateobj;
}

int mushort::readin(istream& Binary) {
  return templateobj->readin(Binary);
}

long mushort::get(int xloc, int yloc) {
  return templateobj->get(xloc,yloc);
}

muint::muint(int x, int y = 1) {
   templateobj = new muintemplate<int>(x,y);
}

muint::~muint() {
  delete templateobj;
}

int muint::readin(istream& Binary) {
  return templateobj->readin(Binary);
}

long muint::get(int xloc, int yloc) {
  return templateobj->get(xloc,yloc);
}

template <class T>
smaugintemplate<T>::smaugintemplate(int x, int y): xlength(x), ylength(y) {
  if (ylength == 0) ylength = 1;
  if ((xlength>0) && (ylength >0)) {
    data = new T[xlength * ylength];
  } else {
    reporterror((nscloperror*)new nscloperror(nscloperror::BadDataIndex ) );
  }
  if (data==0) {
   spcbin::reporterror((nscloperror*)new nscloperror(nscloperror::OutOfMemory ) );
  }
}

template <class T>
smaugintemplate<T>::~smaugintemplate() {
  delete []data;
}

template <class T>
int smaugintemplate<T>::readin(istream& Binary) {
  T temp;
  int ReadOk, num, maskrecords,current = 0;
  maskrecords = sizeofbitmask(xlength, ylength);
  bitmask* mask = new bitmask[maskrecords];
  Binary.read((char*)mask, sizeof(bitmask)* maskrecords);
  if (ylength>0) {
    num=xlength*ylength;
  } else {
    num=xlength;
  }
  memset(data,0,sizeof(temp)*num);
  for (int rec = 0; rec<maskrecords; rec++) {
    for (int byte = 0; byte<maskbytes; byte++) {
      for (int bit = 0; bit<8; bit++) {
	if (mask[rec][byte] & (1<<bit)) {
	  Binary.read((char*)&temp, sizeof(temp));
	  data[current]=temp;
	}
	current++;
	if (current==num) break;
      }
      if (current==num) break;
    }   
    if (current==num) break;
  }
  if (Binary.fail()) {
    delete []mask;
    reporterror((nsclfileerror*)new nsclfileerror(nsclfileerror::ReadErrorS ) );
  }
  ReadOk = Binary.fail();
  delete []mask;
  return ReadOk;
}

template <class T>
long smaugintemplate<T>::get(int xloc, int yloc) {
  if ((xloc >=0) && (yloc >=0)) {
    return data[yloc * xlength + xloc];
  } else {
    reporterror((nscloperror*)new nscloperror(nscloperror::BadIndexGet ) );
  }
  return 0;
}

smaugshort::smaugshort(int x, int y) {
  templateobj=new smaugintemplate<short>(x, y);
}

smaugshort::~smaugshort() {
  delete templateobj;
}

int smaugshort::readin(istream& Binary) {
  return templateobj->readin(Binary);
}

long smaugshort::get(int xloc, int yloc) {
  return templateobj->get(xloc, yloc);
}

smaugint::smaugint(int x, int y = 1) {
  templateobj=new smaugintemplate<int>(x,y);
}
smaugint::~smaugint() {
  delete templateobj;
}

int smaugint::readin(istream& Binary) {
  return templateobj->readin(Binary);
}

long smaugint::get(int xloc, int yloc) {
  return templateobj->get(xloc,yloc);
}

smaugbyte::smaugbyte(int x, int y) {
  templateobj=new smaugintemplate<char>(x,y);
}

smaugbyte::~smaugbyte() {
  delete templateobj;
}

int smaugbyte::readin(istream& Binary) {
  return templateobj->readin(Binary);
}

long smaugbyte::get(int xloc, int yloc) {
  return templateobj->get(xloc, yloc);
}

template <class T>
writetemplate<T>::writetemplate(int x, int y): xlength(x), ylength(y) {
  if (ylength == 0) {
    ylength = 1;
  }
  if ((xlength>0) && (ylength >0)) {
    data = new T[xlength * ylength];
    memset(data,0,sizeof(temp)*xlength*ylength);
  } else {
   spcbin::reporterror((nscloperror*)new nscloperror(nscloperror::BadDataIndex ) );
  }
  if (data==0) {
   spcbin::reporterror((nscloperror*)new nscloperror(nscloperror::OutOfMemory ) );
  }
}  

template <class T>
writetemplate<T>::~writetemplate() {
    delete []data;
}

template <class T>
void writetemplate<T>::put(int index, int val) {
  data[index]=val;
}

template <class T>
void writetemplate<T>::put(int xindex, int yindex, int val) {
  data[yindex *xlength+xindex]=val; 
}

template <class T>
bool writetemplate<T>::writeout(ostream& Binary) {
  int current = 0, num=xlength;
  if (ylength > 0) {
    num*=ylength;
  }
  int maskrecords = sizeofbitmask(xlength, ylength);
  bitmask* mask = new bitmask[maskrecords];
  memset(mask, 0, sizeof(bitmask) * maskrecords);
  
  for (int rec = 0; rec<maskrecords; rec++) {
    for (int byte = 0; byte<maskbytes; byte++) {
      for (int bit = 0; bit<8; bit++) {
	if (data[current] !=0) { 
	  mask[rec][byte] = mask[rec][byte] | (1<<bit);
	} else {
	}
	current++;
	if (current==num) break;
      }
      if (current==num) break;
    }   
    if (current==num) break;
  }
  Binary.write(&(mask[0][0]), sizeof(bitmask)*maskrecords);  
  if (Binary.fail()) {
    delete []mask;
    spcbin::reporterror((nsclfileerror*)new nsclfileerror(nsclfileerror::BitmaskWriteError ) );
  }
  current = 0;
  for (int rec = 0; rec<maskrecords; rec++) {
    for (int byte = 0; byte<maskbytes; byte++) {
      for (int bit = 0; bit<8; bit++) {
	if (mask[rec][byte] & (1<<bit)) {
	  Binary.write((char*)&(data[current]), sizeof(T));
	}
	current++;
	if (current==num) break;
      }
	if (current==num) break;
    }   
     	if (current==num) break;
  }  
  if (Binary.fail()) {
    delete []mask;
    spcbin::reporterror((nsclfileerror*)new nsclfileerror( nsclfileerror::DataWriteError) );
  }
  delete []mask;
  return true;
}

smaugwriteint::smaugwriteint(int x, int y) {
  spectout = new writetemplate<int>(x,y);
}

smaugwriteint::~smaugwriteint() {
  delete spectout;
}

void smaugwriteint::put(int index, int val) {
  spectout->put(index, val);
}

void smaugwriteint::put(int xindex, int yindex, int val) {
  spectout->put(xindex, yindex,val);
}

bool smaugwriteint::writeout(ostream& Binary) {
  return spectout->writeout(Binary);
}

smaugwriteshort::smaugwriteshort(int x, int y) {
  spectout = new writetemplate<short>(x,y);
}

smaugwriteshort::~smaugwriteshort() {
  delete spectout;
}

void smaugwriteshort::put(int index, int val) {
  spectout->put(index, val);
}

void smaugwriteshort::put(int xindex, int yindex, int val) {
  spectout->put(xindex, yindex,val);
}

bool smaugwriteshort::writeout(ostream& Binary) {
  return spectout->writeout(Binary);
}

smaugwritebyte::smaugwritebyte(int x, int y) {
  spectout = new writetemplate<char>(x,y);
}

smaugwritebyte::~smaugwritebyte() {
  delete spectout;
}

void smaugwritebyte::put(int index, int val) {

  spectout->put(index, val);
}

void smaugwritebyte::put(int xindex, int yindex, int val) {
 
  spectout->put(xindex, yindex,val);
}

bool smaugwritebyte::writeout(ostream& Binary) {
  return spectout->writeout(Binary);
}
