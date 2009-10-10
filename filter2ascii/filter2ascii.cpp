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

/*
   Program to convert a set of filter files to 
   ascii.  The output files are written as a table of numbers.
   The first line of the table is the list of parameter names in the
   order in which they appear on table lines.
   Each subsequent line of the table is an event.
   If parameters are not present in an event they will show up as ---
   otherwise they show up as floating point numbers.
*/


#include "CXdrInputStream.h"
#include <stdlib.h>
#include <sysexits.h>
#include <strings.h>


#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

// Globally scoped storage..

static int nParameters = 0;


/*
   Print usage to stderr.
*/

void Usage()
{
  cerr << "Usage\n";
  cerr << "   filter2ascii filterfiles...\n";
}

/*
   Create the output filename fron an input filename.
   This is done by remove the filetype from inputName
   and pasting on asc instead.
   If the inputName does not have a file type,
   .asc is just appended.
*/
string createOutputName(string inputName)
{
  string result;
  char* pPeriod = rindex(inputName.c_str(), '.');
  if (!pPeriod) {
    result = inputName + ".asc";
  }
  else {
    const char* src = inputName.c_str();
    while (src < pPeriod) {
      result += *src++;
    }
    result += ".asc";
  }
  return result;
}
/* translate a single event...
 */
void
translateEvent(CXdrInputStream& in, ostream& out)
{
  // assume 32 bit ints:

  int nBitmasks = (nParameters + 31)/32;
  vector<int> bitMasks;
  for (int i = 0; i < nBitmasks; i++) {
    int  b;
    in >> b;
    bitMasks.push_back(b);
  }
  // Now the parameters.
  // If the bit is not set output -- otherwise
  // output the value.

  int parno =0; 
  for (int i =0; i < bitMasks.size(); i++) {
    for(int j =0; (j < 32) && (parno < nParameters) ; j++) {
      if (bitMasks[i] & (1 << j)) {
	double value;
	in >> value;
	out << value << " ";
      } 
      else {
	out << "--- ";
      }
      parno++;
    }
  }
  out << "\n";

}
/*
   Translate a single record from the input file -> output file.
   Header records print he names across the line.
   event records print the event data.
*/
void translateRecord(CXdrInputStream& in, ostream& out)
{
  string type;
  
  in >> type;
  
  if (type == "header") {
    in >> nParameters;
    for (int i =0; i < nParameters; i++) {
      string name;
      in >> name;
      out << name << " ";
    } 
    out << endl;
  }
  else {
    if (nParameters == 0) {	// NO header!!!
      cerr << " bad input file.. missing header ";
      in.Disconnect();
      return;
    }
    if (type != "event") {
      cerr << " bad input file format... unrecognized record " << type;
      in.Disconnect();
      return;
    }
    translateEvent(in, out);
  }

}

/*
  Translate a single filter file.
*/
void translateFilter(string inputName, string outputName)
{
  // Open the input and output files:

  cerr << "Translating " << inputName << " to " << outputName << " ...";
  cerr.flush();

  nParameters = 0;

  try {
    ofstream         out(outputName.c_str());
    CXdrInputStream  in(inputName.c_str());
    in.Connect();
    
    while(in.isOpen()) {
      translateRecord(in, out);
    }
  }
  catch (string msg) {
    cerr << " Input file format error: " << msg;
  }
    cerr << "done...\n";
}

/*  Entry point.. for each file on the input line produce an output file:
 */

int 
main(int argc, char** argv)
{
  argc--; argv++;

  if (!argc) {
    Usage();
    exit(EX_USAGE);
  }

  for (int i =0; i < argc; i++) {
    string inputName = argv[i];
    string outputName = createOutputName(inputName);

    translateFilter(inputName, outputName);
  }
}
