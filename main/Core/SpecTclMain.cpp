/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerriza
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  SpecTclMain.cpp
 *  @brief: Entry point for  tailored SpecTcl 
 */

/**
 * This is in a separate file so it can be removed from the SpecTcl library
 * so it won't cause dueling main problems.
 */
#include "TclGrammerApp.h"
#include <iostream>

using namespace std;

extern int SpecTclArgc;
extern char** SpecTclArgv;
/*!
 * \brief main
 *
 * Here is our hidden/controlled main function. Derived instances of
 * SpecTcl must use our main and only have control over the methods they
 * override.
 *
 * \param argc
 * \param argv
 * \return
 */
int main(int argc, char* argv[]) {
    SpecTclArgc = argc;
    SpecTclArgv = argv;
    try {
        CTclGrammerApp::m_argc = argc;
        CTclGrammerApp::m_pArgV = argv;
        CTclGrammerApp::getInstance()->run();

        return 0;
    } catch (std::string msg) {
        cerr << "An unhandled string exception was caught: " << msg << endl;
        return -1;
    }
    catch (const char* msg) {
        cerr << "An unhandled char* exception was caught: " << msg << endl;
        return -1;
    }
    catch (std::exception& e) {
      cerr << "std::exception: " << e.what() << endl;
      return -1;
    }
    catch(...) {
        cerr << "Unhandled exception\n";
        return -1;
    }

    return 0;
}
