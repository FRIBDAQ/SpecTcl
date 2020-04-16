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

/** @file:  DBTcl.h
 *  @brief: Binding of SpecTcl::CDatabase for Tcl.
 */
#ifndef DBTCL
#define DBTCL

#include <TCLObjectProcessor.h>
#include <vector>
#include "SaveSet.h"



namespace SpecTcl {

class CDatabase;
class SaveSet;
class DBParameter;
class DBSpectrum;
/**
 * @class DBTcl
 *    This command ensemble defines commands that
 *    mimic the static methods of the SpecTcl::CDatabase class
 *    In addition the DBTcl connect command
 *    returns a new command processor that encapsualtes
 *    a database providing access to method classes:
 *
 *    Ensemble commands:
 *
 *    -  create - initializes a new database file.
 *    -  connect- creates a new command processor that
 *                encapsulates a database object providing the
 *                commands:
 *           *  createSaveSet
 *           *  getSaveSet
 *           *  getSaveSetById
 *           *  listSaveSets
 *           *  destroy 
 *   More about these in the documentation to those command
 *   processors.
 */

class DBTcl : public CTCLObjectProcessor
{
private:
    static int m_serialNumber;
public:
    DBTcl(CTCLInterpreter& interp);
    
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
protected:
    void create(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void connect(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    
};
/**
 * @class DBTclInstance
 *    This is the command processor that's created  by
 *    DBTcl connect. See the documentation for operator()
 *    for a full documentation of the command ensemble
 *    sub-commands.
 */
class DBTclInstance : public CTCLObjectProcessor
{
private:
    static int m_serialNumber;
    CDatabase* m_pDatabase;
public:
    DBTclInstance(
        CTCLInterpreter& interp, const char* command,
        CDatabase* pDatabase
    );
    virtual ~DBTclInstance();
    
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
protected:
    void createSaveset(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void getSaveset(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void listSavesets(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void destroy();
private:
    std::string wrapSaveSet(SaveSet* pSaveSet);
};

/**
 * @class TclSaveSet
 *     This class provides the services of a save set.
 *     It is a command ensemble processor that comes into being
 *     via either the createSaveset or one of the getSaveSet(xx)
 *     operations in a database instance.
 *     The command ensemble, in turn provides the API
 *     of a save set to create/manipulate other
 *     items.  See the documenation for operator() for the
 *     full command ensemble subcommand set.
 */
class TclSaveSet : public CTCLObjectProcessor
{
    // The statics below are used to generate unique
    // commands for the entities we can produce.
private:
    static int  m_parameterSerial;
    static int  m_spectrumSerial;
    static int  m_gateSerial;
    static int  m_applicationSerial;
    static int  m_treeVariableSerial;

    // The object processor is performing its operations
    // on the attached saveset
private:
    SaveSet*  m_pSaveSet;
    
    // canonicals:
    
public:
    TclSaveSet(
        CTCLInterpreter& interp, const char* cmd,
        SaveSet*  pSaveset
    );
    virtual ~TclSaveSet();
    
    int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
protected:
    void getInfo(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    
    void listParameters(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void createParameter(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void findParameter(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    
    void spectrumExists(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void createSpectrum(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void listSpectra(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void findSpectrum(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    
    void gateExists(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void create1dGate(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void create2dGate(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void createCompoundGate(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void createMaskGate(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void findGate(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void listGates(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    
    void applyGate(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void findApplication(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void listApplications(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    
    void createVariable(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void findVariable(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void variableExists(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void listVariables(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void destroy();
    
    // Utility methods.
private:
    void paramDefToObj(CTCLInterpreter& interp, CTCLObject& obj, DBParameter* param);
    std::vector<const char*> listObjToConstCharVec(CTCLObject& obj);
    std::vector<SaveSet::SpectrumAxis> listObjToAxes(CTCLObject& obj);
    
    void makeSpectrumDict(CTCLObject& obj, DBSpectrum* spec);
    void stringVectorToList(
        CTCLInterpreter& interp, CTCLObject& obj,
        const std::vector<std::string> & strings
    );
    void makeAxisDict(
        CTCLInterpreter& interp, CTCLObject& obj,
        const SaveSet::SpectrumAxis& axis
    );
    std::vector<std::pair<double, double>> pointsFromObj(CTCLObject& obj);
    
    void makeGateDict(CTCLInterpreter& interp, CTCLObject& obj, DBGate* pGate);
};  




}                                       // SpecTcl namespace.

#endif