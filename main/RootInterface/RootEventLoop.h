/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2024.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerizza
             Jin-Hee Chang
             Simon Giraud
             Aaron Chester
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/
#ifndef ROOTEVENTLOOP_H
#define ROOTEVENTLOOP_H
#include <TCLTimer.h>


class CRootEventLoop : public CTCLTimer
{
private:
  bool m_exit;
public:
  CRootEventLoop(CTCLInterpreter* pInterp);
  ~CRootEventLoop();
  
  virtual void operator()();
  void stop();
};

#endif