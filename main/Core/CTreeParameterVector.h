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
 #include <stdexcept>

 class CTreeParameter;
 
 

 

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
public:
   /**
     * this struct is shared across all CTreeParmeterVectors with the same basename.
     */
    typedef struct _TreeVectorInfo {
        double s_low;                         // Low limit.
        double s_high;                        // High limit.
        std::string s_units;                    // units of measure.
        std::vector<CTreeParameter*>  s_createdParameters;   // Parameter's we've created.
        std::vector<CTreeParameter*>  s_event;               // Parameters in this event.
        _TreeVectorInfo();
    } TreeVectorInfo, *pTreeVectorInfo;

    // Thrown from find.
    class NoSuchVectorException : std::exception {
    private:
        std::string m_message;
        NoSuchVectorException() {}          // For copy constuction.
    public:
        NoSuchVectorException(const std::string& name);
        NoSuchVectorException(const char* name);
        NoSuchVectorException(const NoSuchVectorException& other);
        NoSuchVectorException& operator=(const NoSuchVectorException& rhs);
        virtual ~NoSuchVectorException();

        virtual const char* what() const noexcept;
    };
private:
    static std::map<std::string, pTreeVectorInfo> m_baseNameMap; // Map of all infos.

    pTreeVectorInfo m_pInfo;       // My info.
    std::string     m_baseName;
public:
    // Canonicals:

    CTreeParameterVector(const char* basename);
    CTreeParameterVector(const char* basename, double low, double high, const char* units = "");
    CTreeParameterVector(const CTreeParameterVector& rhs);    // Can copy construct.
    CTreeParameterVector& operator=(const CTreeParameterVector& rhs); // can assign.
    int operator==(const CTreeParameterVector& rhs) const;  // equal names.
    int operator!=(const CTreeParameterVector& rhs) const;  // not equal names.


    ~CTreeParameterVector();                   // destructor.

    // accessor

    CTreeParameter& operator[](size_t index);   //!< Indexing uses at so throws if out of bounds.
    CTreeParameter& push_back(double value);    //!< Appends to the vector and returns reference to the parameter.
    size_t size() const;                        //!< Number of elements this event.
    size_t allocation() const;                  //!< number of defined tree parameters.
    
    void reset();                               //!< Resets the event vector -> empty.

    // characteriestics

    double low() const;
    double high() const;
    std::string units() const;
    void setLow(double low);
    void setHigh(double high);
    void setUnits(const char* units);
    std::string name() const;


    static void BeginEvent();                   //!< Resets all event vectors -> empty.
    static std::map<std::string, pTreeVectorInfo>::iterator begin();
    static std::map<std::string, pTreeVectorInfo>::iterator end();
    static size_t numVectors();
    static CTreeParameterVector find(const char* name);

    // Uitility methods.
private:
    // You might wonder why not just default high/low in the first method.  The answer
    // is that I want to allow finding the parameter block and _not_ overriding
    // the limits if it already exists.
    pTreeVectorInfo getInfoBlock(const char* name);
    pTreeVectorInfo getInfoBlock(const char* name, double low, double high, const char* units);

    CTreeParameter*  createEventParameter();
    CTreeParameter*  createParameter();
    std::string      nextName() const;
 };

#endif