#include <nsclbinerror.h>
/*
  nsclbinerror.cc
  by Chase Bolen
  2-25-00
  
See nsclbinerror.h for an explanation of the classes.
   
*/




static char* fileerrorstrings[] = {
  "File error: Empty stream.",
  "File error: error reading header.",
  "File error: read error from data object.",
  "File error: error reading data.",
  "File error: error writing buffer.",
  "File error: error writing data.",
  "File error: error writing bitmask."
};

nsclfileerror::nsclfileerror(errorreason what):whatswrong(what) {}
  
nsclfileerror& nsclfileerror::operator=(errorreason what) {
  whatswrong=what;
  return *this;
}
const char* nsclfileerror::reasontext() {
  return fileerrorstrings[(int)whatswrong];
}


static char* operrorstrings[] = {
  "Header error: invalid format.",
  "Header error: invalid header.",
  "Cannot write without setup.",
  "Data object init error: Bad data array initialization.",
  "Error from get: Bad data array index.",
  "Cannot create data object.  Out of memory."
};

nscloperror::nscloperror(errorreason what):whatswrong(what) {}
  
nscloperror& nscloperror::operator=(errorreason what) {
  whatswrong=what;
  return *this;
}
const char* nscloperror::reasontext() {
  return operrorstrings[(int)whatswrong];
}
