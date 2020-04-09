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

/** @file:  DBApplication.h
 *  @brief: Encapsulates/creates gate applications to spectra.
 */
#ifndef DBAPPLICATIONS_H
#define DBAPPLICATIONS_H

namespace SpecTcl {
class CSqlite;    
/**
 * @class DBApplication
 *    Gates by themselves are simply conditions that return a boolean
 *    for each event.  They are only useful when they are used to conditionalize
 *    something.  In SpecTcl, that something, is normally the incrementing
 *    of a spectrum.  Each SpecTcl spectrum has exactly one gate that is
 *    conditionlizes its increments. That gate is said to be "applied" to that
 *    spectrum.  Spectra in SpecTcl come into being with a predefined "T"
 *    gate applied to them.  The SpecTcl database gate_applications table
 *    keeps track of any gates other than the pre-defined T gate that are
 *    applied to spectra.  It's not much more than a JOIN table between the
 *    spectrum_defs and gate_defs tables.
 */
class DBApplication {
public:
    struct Info {
        unsigned s_id;
        unsigned s_gateid;
        unsigned s_spectrumid;
    };
private:
    CSqlite& m_connection;
    Info     m_Info;
public:
    // constructors
    
    // Constructors used internally:
private:
    DBApplication(CSqlite& conn, const Info& info);
    DBApplication(CSqlite& conn, int saveid, const char* gate, const char* spectrum);
    // Forbidden constructors:
private:
    DBApplication(const DBApplication&);
    DBAppliction& operator=(const DBApplication&);
    int operator==(const DBApplication&);
    int operator!=(const DBApplication&);
    
    // Object methods:
    
public:
    const Info& getInfo() const { return m_Info; }
    
    // Static methods:

public:
    static DBApplication* applyGate(
        CSqlite& conn, int saveid, const char* gate, const char* spectrum
    );
    static std::vector<DBApplication*> listApplications(CSqlite& conn, int saveid);
    
    // private utilities:
private:
    
};
}                         // SpecTcl namespace.

#endif