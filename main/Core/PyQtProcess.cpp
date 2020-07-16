//    This software is Copyright by the Board of Trustees of Michigan
//    State University (c) Copyright 2020.
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Authors:
//    Giordano Cerizza
//    NSCL
//    Michigan State University
//    East Lansing, MI 48824-1321

#include "PyQtProcess.h"

#include <ErrnoException.h>

#include <string>
#include <cstdlib>
#include <iostream>

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include<sys/wait.h> 
#include<sys/types.h> 

CPyQtProcess::CPyQtProcess() : m_pid(0)
{
}

CPyQtProcess::~CPyQtProcess() {
    if (m_pid != 0) {
        kill();
    }
}
void
CPyQtProcess::exec()
{
    std::cout << "Inside CPyQtProcess::exec" << std::endl;

    int inflg, errflg, outflg;

    inflg  = fcntl(0, F_GETFD, 0);
    outflg = fcntl(1, F_GETFD, 0);
    errflg = fcntl(2, F_GETFD, 0);
    fcntl(0, F_SETFD, 0);
    fcntl(1, F_SETFD, 0);
    fcntl(2, F_SETFD, 0);
    
    m_pid = fork();

    if(m_pid < 0) {
      m_pid = 0;
      throw CErrnoException("CPyQtProcess::exec() - Unable to fork()");
    } else if (m_pid == 0){

      close(STDOUT_FILENO);
      close(STDERR_FILENO);
      close(STDIN_FILENO);
      
      /*
      // print environ
      int i = 1;
      char *s = *environ;
      
      for (; s; i++) {
      printf("%s\n", s);
      s = *(environ+i);
      }
      */

      std::string path = generatePath();
      char* const argv[] = {const_cast<char*>(path.c_str()), nullptr};

      // child process
      execvpe(path.c_str(), argv, environ);
      return;

    } else {
      fcntl(0, F_SETFD, inflg);
      fcntl(1, F_SETFD, outflg);
      fcntl(2, F_SETFD, errflg);
    }

}

std::string
CPyQtProcess::generatePath() const {

  // Makefile rule sets INSTALLED_IN to @prefix@
  std::string defaultPyQtPath(INSTALLED_IN);

  defaultPyQtPath += "/bin/PyQtGUI";

  // environment variable overrides
  std::string PyQtPath;
  const char* envPath = std::getenv("PYQTGUI_EXECUTABLE_PATH");
  if (envPath) {
    PyQtPath = envPath;
  }
  
  if (PyQtPath.empty()) {
    PyQtPath = defaultPyQtPath;
  }

  std::cout << "Path generated for PyQtGUI: " << PyQtPath << std::endl;
  return PyQtPath;
}

void
CPyQtProcess::kill()
{
    // don't try to kill a process that doesn't exist
    if (m_pid == 0) {
        return;
    }

    int status = ::kill(m_pid, SIGTERM);
    if (status < 0) {
        throw CErrnoException("CPyQtProcess::kill()");
    }

    m_pid = 0;

}

int
CPyQtProcess::getPid() const
{
    return m_pid;
}

bool
CPyQtProcess::isRunning() const
{
    return (m_pid != 0);
}

