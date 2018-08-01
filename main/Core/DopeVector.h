/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2008

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
             Ron Fox
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/


#ifndef DOPEVECTOR_H
#define DOPEVECTOR_H
#include <vector>



/*!
  This class encapsulates a dope vector.
  Dope vectors are used in SpecTcl to keep track of the set of parameters
  that are present in an event.  Dope vectors are typically associated
  with an event.
*/
class DopeVector {
  // local data:
private:
  unsigned int               m_dopeSize;
  std::vector<unsigned int>  m_indices;

  // Canonicals
public:
  DopeVector();
  DopeVector(const DopeVector& rhs);
  virtual ~DopeVector();
  DopeVector& operator=(const DopeVector& rhs);


  int operator==(const DopeVector& rhs) const {
    return (m_dopeSize == rhs.m_dopeSize) &&
      (m_indices == rhs.m_indices);
  }
  int operator!=(const DopeVector& rhs) const {
    return !(*this == rhs);
  }



  // Operations.
public:
  void reset();			         // Clear the dope vector to zero length.
  void append(unsigned int index);   // Append element.
  unsigned int size() const;         // How many entries in the vector:
  unsigned int& operator[](int index); // Get an element.

};

#endif
