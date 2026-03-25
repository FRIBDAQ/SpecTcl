#ifndef CTREEPARAMETERVECTOR_H
#define CTREEPARAMETERVECTOR_H
/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2026.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Author:
        Ron Fox
	    NSCL
	    Michigan State University
	    East Lansing, MI 48824-1321
*/
/**
 * @file CTreeParameterVector.h
 * @brief Define the expandable Tree Paramter vector class
 */

 #include <vector>
 #include <map>
 #include <string>
 #include <histotypes.h>

 class CTreeParameter;
 
 static const UInt_t DEFAULT_LOW(0);
 static const UInt_t DEFAULT_HIGH(100);

 /**
  * this struct is shared across all CTreeParmeterVectors with the same basename.
  */
typedef struct _TreeVectorInfo {
    UInt_t s_low;                         // Low limit.
    UInt_t s_high;                        // High limit.
    std::vector<CTreeParameter*>  s_createdParameters;   // Parameter's we've created.
    std::vector<CTreeParameter*>  s_event;               // Parameters in this event.
} TreeVectorInfo, *pTreeVectorInfo;



 /**
  * @class CTreeParameterVector
  * This class ia an expandable vector of tree parameters.
  * The idea is that, as in e.g. FDSI code,  you just push_back
  * values into the array and parameters are created as needed to hold
  * them.  The vector supports subscripting just as CTreeParamterArrays doe.
  * Since we can't pre-calculate the size of the index strings, the
  * parameter names will look like Tcl array elements e.g.:
  * 
  * basename(0), basename(1)...
  * 
  * This may make parameter-list look a bit funny but tough.
  * 
  */

 class CTreeParameterVector {
private:
    static std::map<std::string, pTreeVectorInfo> m_baseNameMap; // Map of all infos.

    pTreeVectorInfo m_pInfo;       // My info.
    std::string     m_baseName;
public:
    // Canonicals:

    CTreeParameterVector(const char* basename);
    CTreeParameterVector(const char* basename, UInt_t low, UInt_t high);
    CTreeParameterVector(const CTreeParameterVector& rhs);    // Can copy construct.
    CTreeParameterVector operator=(const CTreeParameterVector& rhs); // can assign.
    int operator==(const CTreeParameterVector& rhs) const;  // equal names.
    int operator!=(const CTreeParameterVector& rhs) const;  // not equal names.


    ~CTreeParameterVector();                   // destructor.

    // accessor

    CTreeParameter& operator[](size_t index);   //!< Indexing uses at so throws if out of bounds.
    CTreeParameter& push_back(double value);    //!< Appends to the vector and returns reference to the parameter.
    size_t size() const;                        //!< Number of elements this event.
    size_t allocation() const;                  //!< number of defined tree parameters.
    
    void Reset();                               //!< Resets the event vector -> empty.

    static void BeginEvent();                   //!< Resets all event vectors -> empty.
 };

#endif