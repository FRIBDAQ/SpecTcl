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


/*!
   This program splits filter files into fixed sized chunks.
   It should be used as follows:

   filtsplit --size=blocks_per_file --output=stem_name [-V] input_file
   Where:
     blocks_per_file - Is the number of blocks per output file.
     stem_name       - Is the output file name stem, a 3 digit number and .flt
                       will be appended to the stem_name to form the actual
		       output filename.
     -V              - Is an optional parameter that, if present, shows the
                       splitting process on the stdout.

*/

#include <config.h>
#include "cmdline.h"
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>

#include <CXdrInputStream.h>
#include <CXdrOutputStream.h>

#ifdef HAVE_STD_NAMESPACE
using namespace std;
#endif



// global data:

bool            verbose(false);
long            blocksPerFile;
string          filenameStem;
string          inputFile;

/*!
  Prints the program usage:
*/
void Usage()
{
  cmdline_parser_print_help();
}

/*!
  Process the command line parameters into our globals,
  exiting with an error message if they don't match
  \param argc - Count of command line arguments.
  \param argv - Array of pointers to the arguments.

*/
void processArgs(int argc, char** argv)
{
  struct gengetopt_args_info  arginfo;

  // First just try to parse the options:

  if (cmdline_parser(argc, argv, &arginfo) == EXIT_FAILURE) {
    Usage();
    fflush(stdout);
    exit(EXIT_FAILURE);
  }
  
  // Now let's see what we have

  // Only allowed one input file:

  if (arginfo.inputs_num != 1) {
    fprintf(stderr, "There must be exactly one input file\n");
    Usage();
    exit(EXIT_FAILURE);
  } 
  else {
    inputFile = arginfo.inputs[0];
  }

  // We must have --size and --output switches, their values
  // fill in the blocksPerFile and filenameStem globals:

  if (arginfo.size_given) {
    blocksPerFile = arginfo.size_arg;
  }
  else {
    fprintf(stderr, "The --size flag is required, but missing\n");
    Usage();
    exit(EXIT_FAILURE);
  }
  if (arginfo.output_given) {
    filenameStem = arginfo.output_arg;
  }
  else {
    fprintf(stderr, "The --output flag is required but missing\n");
    Usage();
    exit(EXIT_FAILURE);
  }

  // If verbose is supplied, we can change the value of the
  // verbose flag:

  verbose = arginfo.verbose_flag;

}
// Read a header in to vector of parameter name strings:
//
vector<string>
readHeader(CXdrInputStream& input)
{
  int nParams;
  vector<string> result;
  input >> nParams;

  for (int i=0; i < nParams; i++) {
    string parameter;
    input >> parameter;
    result.push_back(parameter);
  }
  if (verbose) {
    fprintf(stderr, "Encountered a header record with the following parameters\n");
    for (int i =0; i < result.size(); i++) {
      fprintf(stderr, "   %s\n", result[i].c_str());
    }
  }

  return result;

}

// Create a filename from a stem and index number:
//
string
createFilename(int index)
{
  char indexString[100];
  sprintf(indexString, "%03d", index);
  string name = filenameStem;
  name += indexString;
  return name;
}

// write the header to the output file:
// the header is a string "header" followed by the number of parameters
// followed by the parameters themselves:
//
void
writeHeader(CXdrOutputStream& out, vector<string> parameters)
{
  out << "header";
  out << parameters.size();
  for (int i =0; i < parameters.size(); i++) {
    out << parameters[i];
  }
}

// Return a count of the number of bits set in a mask:

int
countBits(int mask)
{
  int bits = 0;
  int bit = 1;

  while (bit != 0) {		// Mask will spin off the top end...
    if (mask & bit) bits++;
    bit = bit << 1;
  }
  return bits;
}
// copy an event from the input to the output stream:

void
copyEvent(CXdrInputStream& in, CXdrOutputStream& out, size_t numParams)
{
  size_t numBitRegisters = (numParams + 31)/32;	// Number of bit registers needed.
  
  // Need to see how many bits are set so that we know how many parameters to copy
  // so copy the bit registers seperatly tallying bits:
  //
  int numParameters = 0;
  for (int i= 0; i < numBitRegisters; i++) {
    int mask;
    in >> mask;
    numParameters += countBits(mask);
    out << mask;
  }
  // Now copy the parameters:

  for (int i =0; i < numParameters; i++) {
    float parameter;

    in >>  parameter;
    out << parameter;
  }
}


// Create an output file:

vector<string>
writeNextFile(CXdrInputStream& input, vector<string> parameters, int index)
{
  string filename;
  try {
    filename = createFilename(index);
    CXdrOutputStream out(filename);

    if (verbose) {
      fprintf(stderr, "Opened output file: %s\n", filename.c_str());
    }
    writeHeader(out, parameters);
    while(input.isOpen() && (out.getBlocks() < blocksPerFile)) {
      string label;
      input >> label;
      if (label == string("header")) {
	parameters = readHeader(input);
      }
      else if (label == string("event"))  {
	copyEvent(input, out, parameters.size());
      }
      else {
	fprintf(stderr, "Invalid label string in record: %s\n", label.c_str());
	exit(EXIT_FAILURE);
      }
    }
    out.Close();
    if (verbose) {
      fprintf(stderr, "Closed %s\n", filename.c_str());
    }
      
  }
  catch (string msg) {
    fprintf(stderr, "Unable to write output file %s : %s \n",
	    filename.c_str(), msg.c_str());
    exit(EXIT_FAILURE);
  }

  return parameters;
  
}

// Create the output files:

void 
writeOutputFiles(CXdrInputStream& input, vector<string> parameters)
{
  int index = 0;
  while (input.isOpen()) {
    parameters = writeNextFile(input, parameters, index);
    index++;
  }
}

// Entry point:

int main(int argc, char** argv)
{
  processArgs(argc, argv);

  // If verbose, print the input params:

  if (verbose) {
    fprintf(stderr, "Splitting: %s\n", inputFile.c_str());
    fprintf(stderr, "Into %d block chunks\n", blocksPerFile);
    fprintf(stderr, "Ouptut filenames of the form %snnn.flt\n", filenameStem.c_str());
  }


  // Try to open the input file as an Xdr stream:

  CXdrInputStream* pInputStream;
  try {
    pInputStream = new CXdrInputStream(inputFile);
    pInputStream->Connect();
  }
  catch (string msg) {
    fprintf(stderr, "Unable to open %s : %s\n", 
	    inputFile.c_str(), msg.c_str());
    exit(EXIT_FAILURE);
  }
  // We need to read the first header.. we also need to be sure it is a header:

  string label;
  (*pInputStream) >> label;
  if (label != string("header")) {
    fprintf(stderr, "Input file does not start with a header record.\n");
    fprintf(stderr, "Most likely not a filter file\n");
    exit(EXIT_FAILURE);		     				 
  }
  if (verbose) {
    fprintf(stderr, "Opened input filter file: %s\n", inputFile.c_str());

  }
  // Process the header record so that we can reproduce it into other files:

  vector<string> parameters = readHeader(*pInputStream);

 
  writeOutputFiles(*pInputStream, parameters);
  


  return EXIT_SUCCESS;
}
