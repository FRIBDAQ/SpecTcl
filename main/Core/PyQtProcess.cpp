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
#include <sys/wait.h> 
#include <sys/types.h> 
#include <sys/resource.h>

// Environment variables:
// EXE_PATH_ENV - points to the executable itself.
// ROOT_DIR - Points to the top level directory (/bin/_CutiePie is appended).
// defaultPyQtPath is the full path to the built in one.

static const char* EXE_PATH_ENV="PYQTGUI_EXECUTABLE_PATH";
static const char* ROOT_DIR="PYQTGUI_ROOT";
static const std::string DEFAULT_QTPY_PATH(INSTALLED_IN);

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

    

    // Ensure the stdio fds to be inherited by the child:

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

      // Child process:

      // Close all file descriptors that SpecTcl might have
      // already opened - e.g. the server sockets.
      // Unfortunately there's no easy way to know which they
      // are so we just close them all and ignore the errors:

      struct rlimit maxes;
      if (!getrlimit(RLIMIT_NOFILE, &maxes))  {
        // It's not fatal for this to fail...we just can't do it that's all.

        // Iterate over the possible files, closing all but stdout and stderr
        for (int i = 0; i < maxes.rlim_cur; i++) {
          if ( (i != STDOUT_FILENO) && (i != STDERR_FILENO)) {
            close(i);               // Ignore errors as fd might already be closed.
          }
        }
      }
      
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
      // Parent process.
      fcntl(0, F_SETFD, inflg);
      fcntl(1, F_SETFD, outflg);
      fcntl(2, F_SETFD, errflg);
    }

}

std::string
CPyQtProcess::generatePath() const {

  // Makefile rule sets INSTALLED_IN to @prefix@
  

  std::string defaultPyQtPath = DEFAULT_QTPY_PATH + "/bin/_CutiePie";

  // environment variable overrides
  std::string PyQtPath;
  const char* envPath = std::getenv(EXE_PATH_ENV);
  if (envPath) {
    PyQtPath = envPath;
  }
  // If empty, try the root directory env var:

  if (PyQtPath.empty()) {
    const char* rootpath = std::getenv(ROOT_DIR);
    if (rootpath) {
      PyQtPath = rootpath;
      PyQtPath += "/bin/_CutiePie";
    }
  }
  
  // If still empty 

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

