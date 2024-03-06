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

/** @file:  DBTcl.cpp
 *  @brief: Provides the DBTcl and its instance command ensembles
 */
#include "DBTcl.h"

#include "SpecTclDatabase.h"
#include "SaveSet.h"
#include "DBParameter.h"
#include "DBSpectrum.h" 
#include "DBGate.h"
#include "DBApplications.h"
#include "DBTreeVariable.h"


#include <TCLInterpreter.h>
#include <TCLObject.h>

#include <stdexcept>
#include <sstream>
#include <string>

//
//  A little dictionary API:
namespace SpecTclDB {
static void InitDict(CTCLInterpreter& interp, CTCLObject& objv);
static void AddKey(CTCLObject& dict, const char* key, int value);
static void AddKey(CTCLObject& dict, const char* key, const char* value);
static void AddKey(CTCLObject& dict, const char* key, double value);
static void AddKey(CTCLObject& dict, const char* key, CTCLObject& value);

// other unbound static utiltities:

static std::string
    gateClassificationToString(DBGate::BasicGateType c);
static void gatePointsToDictList(
    CTCLInterpreter& interp, CTCLObject& obj,
    const DBGate::Points& pts
);


///////////////////////////////////////////////////////////
// DBTcl class:

int DBTcl::m_serialNumber(0);

/**
 * constructor:
 *   Constructs the DBTcl command.  This is the base
 *   command of a command ensembel with the following
 *   subcommands:
 *
 *   -  create - creates a new database file.
 *   -  connect- Creates a unique command ensemble
 *               that allows the user to manipulate an
 *               existing database.  See the DBInstance
 *               class for more information about that
 *               command, which will be of the form
 *               DBTclInstance_n where n is a unique number.
 *  @param interp - interpreter on which the DBTcl command is
 *               registered.
 */
DBTcl::DBTcl(CTCLInterpreter& interp) :
    CTCLObjectProcessor(interp, "DBTcl", true)
{}

/**
 * operator()
 *    Command processor for the DBTcl command
 *    - Ensure there's a subcommand.
 *    - Tease the subcommand out of command parameters.
 *    - Dispatch to the subcommand processor, returning an
 *      error indication if the subcommand does not match.
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 * @return int  - TCL_OK on success else TCL_ERROR
 * @note excepion procesing is used to make error processing
 *       simpler.
 */
int
DBTcl::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    int status = TCL_OK;
    try {
        bindAll(interp, objv);
        requireAtLeast(objv, 2, "Need at least a subcommand");
        
        std::string sub = objv[1];
        if (sub == "create") {
            create(interp, objv);
        } else if (sub == "connect") {
            connect(interp, objv);
        } else {
            std::stringstream msg;
            msg << sub << " Is an invalid subcommand. "
                << "Must be 'create' or 'connect'";
            throw std::invalid_argument(msg.str());
        }
    }
    catch (std::string s) {
        interp.setResult(s);
        status = TCL_ERROR;
    }
    catch (std::exception& e) {
        interp.setResult(e.what());
        status = TCL_ERROR;
    }
    
    return status;
}
/**
 * create (protected)
 *    Create a new, properly formatted data base file:
 *
 *  <pre>
 *      DBTcl create filename
 *  </pre>
 *
 *  where filelname is the name of the database file.
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 */
void
DBTcl::create(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 3, "The create sub command only takes a filename");
    std::string filename = objv[2];
    
    CDatabase::create(filename.c_str());
}
/**
 * connect (protected)
 *    Connect to an existing database:
 *
 *  <pre>
 *     DBTcl connect filename.
 *  </pre>
 *
 *  where filename  is the name of an existing database file.
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 * @note on success a new DBTclInstance is created with a unique command
 *       name and the command name is passed back as the result so typical
 *       usage is:
 *  <pre>
 *    set cmd [DBTcl connect filename]
 *    $cmd ....
 *  </pre>
 */
void
DBTcl::connect(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 3, "The connect subcommand takes only a filename");
    
    // Open the database
    
    std::string fname = objv[2];
    CDatabase* pDb = new CDatabase(fname.c_str());
    
    // Create and return the new command processor:
    
    std::stringstream cmdname;
    cmdname << "DBTclInstance_" << m_serialNumber++;
    std::string command = cmdname.str();
    new DBTclInstance(interp, command.c_str(), pDb);
    interp.setResult(command);
}
//////////////////////////////////////////////////////////////////
// DBTclInstance implementation.

int DBTclInstance::m_serialNumber(0);

/**
 * constructor:
 *    @param interp - interpreter running the command.
 *    @param command - name of our command.
 *    @param pDatabase - database we operate on.
 *    @note pDatabase's ownership is transferred to this.
 */
DBTclInstance::DBTclInstance(
    CTCLInterpreter& interp, const char* command,
    CDatabase* pDatabase
) : CTCLObjectProcessor(interp, command, true), m_pDatabase(pDatabase)
{}

/**
 * destructor
 *    deletes the database object.
 */
DBTclInstance::~DBTclInstance()
{
    delete m_pDatabase;
}

/**
 * operator()
 *    - Ensure we have a subcommand word.
 *    - Dispatch based on the subcommand and error if there's
 *      not a valid subcommand.
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 * @return int   - TCL_Ok if all worked.
 * @note each subcommand processor will document the form of its subcommand.
 * 
 */
int DBTclInstance::operator()(
    CTCLInterpreter& interp, std::vector<CTCLObject>& objv
)
{
    int status = TCL_OK;
    try {
        bindAll(interp, objv);
        requireAtLeast(objv, 2, "Database Command instances require a subcommand");
        std::string  command = objv[1];
        
        if (command == "destroy") {
            destroy();
        } else if (command == "createSaveset") {
            createSaveset(interp, objv);
        } else if (command == "getSaveset") {
            getSaveset(interp, objv);
        } else if (command == "listSavesets") {
            listSavesets(interp, objv);
        } else if (command == "listRuns") {
            listRuns(interp, objv);
        } else {
            std::stringstream msg;
            msg << command << " Is not a valid database instance subcommand";
            throw std::invalid_argument(msg.str());
        }
    }
    catch (std::exception& e) {
        interp.setResult(e.what());
        status = TCL_ERROR;
    }
    catch (std::string msg) {
        interp.setResult(msg);
        status = TCL_ERROR;
    }
    return status;
}

/**
 * destroy
 *   called to delete this object.
 */
void DBTclInstance::destroy()
{
    delete this;
}
/**
 * listRuns
 *    Provides information about every run in the database.
 *    Each run stored in the database returns a dict of the form:
 *
 *    config - name of the configuration saveset that holds the run.
 *    number - Run number.
 *    title  - title of the run.
 *    start_time - time the run started (clock time).
 *    stop_time  - time the run stopped (clock time).
 *  
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 * @return int   - TCL_Ok if all worked.
 * @note each subcommand processor will document the form of its subcommand.
 * 
 */
void DBTclInstance::listRuns(
    CTCLInterpreter& interp,   std::vector<CTCLObject>& objv
)
{
    requireExactly(objv, 2, "listRuns does not require any additional parameters");
    CTCLObject result;
    result.Bind(interp);
    
    // Get all the savesets in this database:
    
    auto savesets = m_pDatabase->getAllSaveSets();
    for (int s = 0; s < savesets.size(); s++) {
        auto runNumbers = savesets[s]->listRuns();
        std::string configName = savesets[s]->getInfo().s_name;
        for (int r = 0; r < runNumbers.size(); r++) {
            int id = savesets[s]->openRun(runNumbers[r]);
            auto info = savesets[s]->getRunInfo(id);
            
            CTCLObject dict; dict.Bind(interp);
            InitDict(interp, dict);
            AddKey(dict, "config", configName.c_str());
            AddKey(dict, "number", info.s_runNumber);
            AddKey(dict, "title", info.s_title.c_str());
            AddKey(dict, "start_time", int(info.s_startTime));
            if (info.s_stopTime != 0) {   // 0 is null.
                AddKey(dict, "stop_time", int(info.s_stopTime));
            }
            
            result += dict;
        }
        
        delete savesets[s];       // Dynamically allocated.
    }
    interp.setResult(result);
}
/**
 * createSaveSet
 *    The form of this command is:
 *
 *  instancecmd createSaveset save-set-name
 *
 *  Creates a new saveset in the database.  The result, on success is
 *  a new command ensemble that represents the saveset.
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 */
void DBTclInstance::createSaveset(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 3, "createSaveset subcommand only needs a saveset name");
    
    std::string saveName = objv[2];
    SaveSet* svset = m_pDatabase->createSaveSet(saveName.c_str());
    interp.setResult(wrapSaveSet(svset));
    
}
/**
 * getSaveset
 *   The form of this command is:
 *
 *   instancecmd getSaveset existing-save-set-name
 *
 *   Retuns a command that wraps a previously existing saveset.
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 */
void DBTclInstance::getSaveset(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 3, "getSaveset subcommand only need a save set");
    
    std::string saveName  = objv[2];
    SaveSet* pSet = m_pDatabase->getSaveSet(saveName.c_str());
    interp.setResult(wrapSaveSet(pSet));
}
/**
 * listSavesets
 *   Provide a list of the names of the savests in the database:
 *   Format:
 *
 *   instance-command listSavesets
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 *  The result value is a Tcl list that has, as elements the names of
 *  the savesets stored in the database.
 */
void
DBTclInstance::listSavesets(
    CTCLInterpreter& interp, std::vector<CTCLObject>& objv
)
{
    requireExactly(objv, 2, "listSavesets has no additional parameters");
    
    auto l = m_pDatabase->getAllSaveSets();
    
    CTCLObject result;
    result.Bind(interp);
    
    for (int i = 0; i < l.size(); i++) {
        CTCLObject name;
        name.Bind(interp);
        
        name = l[i]->getInfo().s_name;
        delete l[i];
        
        result += name;
    }
    
    
    interp.setResult(result);
}
 
 

//// Private utilities for DBTclInstance.

/**
 * wrapSaveSet
 *   Wraps a saveset object inside a TclSaveSet.
 *
 * @param pSet - pointer to the saveset.
 * @return std::string - name of the command.
 */
std::string
DBTclInstance::wrapSaveSet(SaveSet* pSet)
{
    std::stringstream nameStream;
    nameStream << "saveset_" << m_serialNumber++;
    std::string name = nameStream.str();
    
    new TclSaveSet(*getInterpreter(), name.c_str(), pSet);
    
    return name;
}

//////////////////////////////////////////////////////////////
// TclSaveSet implementation.

int  TclSaveSet::m_parameterSerial(0);
int  TclSaveSet::m_spectrumSerial(0);          // below are
int  TclSaveSet::m_gateSerial(0);              // reserved for
int  TclSaveSet::m_applicationSerial(0);       // later if
int  TclSaveSet::m_treeVariableSerial(0);      // needed

/**
 * constructor
 *   @param interp - interpreter on which this command is being registered.
 *   @param cmd    - actual command name to use.
 *   @param pSaveset- The saveset this command encapsulates.
 */
TclSaveSet::TclSaveSet(
    CTCLInterpreter& interp, const char* cmd,
    SaveSet*  pSaveset
) : CTCLObjectProcessor(interp, cmd, true), m_pSaveSet(pSaveset)
{}

/**
 * destructor
 */
TclSaveSet::~TclSaveSet()
{
    delete m_pSaveSet;
}

/**
 * operator()
 *    Process the command:
 *    
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 * @return int   - TCL_Ok if all worked.
 * @note each subcommand processor will document the form of its subcommand.
 */
int
TclSaveSet::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    int status =TCL_OK;
    try {
        bindAll(interp, objv);
        requireAtLeast(objv, 2, "Save set commands need a subcommand");
        std::string command = objv[1];
        
        if (command == "destroy") {
            destroy();
        } else if (command == "info") {
            getInfo(interp, objv);
        } else  if (command == "createParameter") {
            createParameter(interp, objv);
        } else if (command == "listParameters") {
            listParameters(interp, objv);
        } else if (command == "findParameter") {
            findParameter(interp, objv);
        } else if (command == "createSpectrum") {
            createSpectrum(interp, objv);
        } else if (command == "spectrumExists") {
            spectrumExists(interp, objv);
        } else if (command == "findSpectrum") {
            findSpectrum(interp, objv);
        } else if (command == "listSpectra") {
            listSpectra(interp, objv);
        } else if (command == "create1dGate") {
            create1dGate(interp, objv);
        } else if (command == "create2dGate") {
            create2dGate(interp, objv);
        } else if (command == "createCompoundGate") {
            createCompoundGate(interp, objv);
        } else if (command == "createMaskGate") {
            createMaskGate(interp, objv);
        } else if (command == "gateExists") {
            gateExists(interp, objv);
        } else if (command == "findGate") {
            findGate(interp, objv);
        } else if (command == "listGates") {
            listGates(interp, objv);
        } else if (command == "applyGate") {
            applyGate(interp, objv);
        } else if (command == "findApplication") {
            findApplication(interp, objv);
        } else if (command == "listApplications") {
            listApplications(interp, objv);
        } else if (command == "createVariable") {
            createVariable(interp, objv);
        } else if (command == "variableExists") {
            variableExists(interp, objv);
        } else if (command == "findVariable") {
            findVariable(interp, objv);
        } else if (command == "listVariables") {
            listVariables(interp, objv);
        } else if (command == "hasChannels") {
            hasChannels(interp, objv);
        } else if (command == "storeChannels") {
            storeChannels(interp, objv);
        } else if (command == "getChannels") {
            getChannels(interp, objv);
        } else if (command == "listRuns") {
            listRuns(interp, objv);
        } else if (command == "getRunInfo") {
            getRunInfo(interp, objv);
        } else if (command == "getScalers") {
            getScalers(interp, objv);
        } else {
            std::stringstream msg;
            msg << command << " is not a legal save set subcommand";
            throw std::invalid_argument(msg.str());
        }
    }
    catch (std::string msg) {
        interp.setResult(msg);
        status = TCL_ERROR;
    }
    catch (std::exception& e) {
        interp.setResult(e.what());
        status = TCL_ERROR;
    }
    
    return status;
}
/**
 * destroy
 *    Called to destroy the saveset object/command.
 */
void
TclSaveSet::destroy()
{
    delete this;
}
/**
 * getInfo
 *   Format:
 *
 *   saveset-command info
 *
 *   Returns the saveset info as the command result.
 *   This is a dict with the following keys:
 *   -  id - the id of the saveset in the save_sets table.
 *   - name - the saveset name.
 *   - timestamp - the saveset creation time for e.g. clock -format.
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 */
void
TclSaveSet::getInfo(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 2, "Saveset's info subcommand reqires no parameters");
    auto info = m_pSaveSet->getInfo();
    
    CTCLObject result;
    InitDict(interp, result);
    AddKey(result, "id", info.s_id);
    AddKey(result, "name", info.s_name.c_str());
    AddKey(result, "timestamp", int((info.s_stamp)));
    
    interp.setResult(result);
}

/**
 * createParameter
 *    Creates a new parameter definition in the saveset.
 *
 *   Form 1:
 *
 *    saveset-command createParameter name number
 *
 *  Form 2 (treeparameter):
 *
 *    saveset-command createParameter name number low high bins ?units?
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
*/
void
TclSaveSet::createParameter(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    // If there are 4 paramters it's a simple parmameter:
    
    if (objv.size() == 4) {
        std::string name   = objv[2];
        int         number = objv[3];
        delete m_pSaveSet->createParameter(name.c_str(), number);
        
    } else if ((objv.size() == 7) || (objv.size() == 8)) {  //treeparameter
        std::string name   = objv[2];
        int         number = objv[3];
        double      low    = objv[4];
        double      high   = objv[5];
        int         bins   = objv[6];
        std::string units;
        if (objv.size() == 8) units = std::string(objv[7]);
        m_pSaveSet->createParameter(
            name.c_str(), number, low, high, bins, units.c_str()
        );
    } else {
        throw std::string("The saveset createParameter command must have 4, 7 or 8 command line words");
    }
}

/**
 *  listParameters
 *
 *  Form:
 *
 *     instance-command listParameters
 *
 *  Sets the result with a list of parameters.  Each parameter is dict
 *  that contains the following keys:
 *  - id     - data base table id of the parameter record.
 *  - name   - parameter name
 *  - number - Number of the parameter.
 *  - low    - (optionally)  low limit on parameter.
 *  - high   - (optionally)  high limit on parameter.
 *  - bins   - (optionally)  suggested binning for parameter.
 *  - units  - (optionally)  units of measure for the parameter.
 *
 *  @note The optional dictionary keys are only present on parameters with
 *        metadata stored.
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
*/
void
TclSaveSet::listParameters(
    CTCLInterpreter& interp, std::vector<CTCLObject>& objv
)
{
    requireExactly(objv, 2, "The listParmeters subcommand takes no additional parameters");
    auto listing = m_pSaveSet->listParameters();
    
    CTCLObject result;
    result.Bind(interp);
    for (int i = 0; i < listing.size(); i++) {
        CTCLObject dict;
        paramDefToObj(interp, dict, listing[i]);
        result += dict;
        delete listing[i];
    }
    interp.setResult(result);
}
/**
 * findParameter
 *    Lookup the information about a parameter.
 *    See listParameters for the structure of the dict that is
 *    returned.  Note that in this case only a single dict
 *    is returned, not a list.
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
*/
void
TclSaveSet::findParameter(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 3, "findParameter needs a parameter name");
    std::string paramName = objv[2];
    auto param = m_pSaveSet->findParameter(paramName.c_str());
    CTCLObject result;
    paramDefToObj(interp, result, param);
    delete param;
    interp.setResult(result);
}

/**
 * createSpectrum:
 *    Format:
 *
 *    instance-cmd createSpectrum name type parameters axes ?datatype?
 *
 * Where
 *    - parameters is a list of parameter names.
 *    - axes is a list of axis definitions of the form [list low high bins]
 *    - datatype is one of 'byte', 'word' or 'long'
 *
 *  @note that we're helped along by the fact that all the error
 *        checking gets done by DBSpectrum's methods, and that it throws
 *        an std::exception derived object on error.
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
*/
void
TclSaveSet::createSpectrum(
    CTCLInterpreter& interp, std::vector<CTCLObject>& objv
)
{
    requireAtLeast(
        objv, 6,
        "createSpectrum needs at least name, type, parameters and axes"
    );
    requireAtMost(
        objv, 7,
        "createSpectrum needs at most name, type, parameters, axes, and datatype"
    );
    
    std::string name = objv[2];
    std::string type = objv[3];
    std::vector<const char*> params = listObjToConstCharVec(objv[4]);
    auto axes = listObjToAxes(objv[5]);
    std::string dtype = "long";
    if (objv.size() == 7) dtype = std::string(objv[6]);
    
    delete m_pSaveSet->createSpectrum(
        name.c_str(), type.c_str(), params, axes, dtype.c_str()
    );
}

/**
 * spectrumExists
 *    Sets the result with true of false depending on whether or not
 *    a spectrum exists:
 *    Format:
 *
 *    instance-cmd spectrumExists specname
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
*/
void
TclSaveSet::spectrumExists(
    CTCLInterpreter& interp, std::vector<CTCLObject>& objv
)
{
    requireExactly(objv, 3, "spectrumExists takes a spectrum name only");
    
    std::string name = objv[2];
    bool result = m_pSaveSet->spectrumExists(name.c_str());
    
    interp.setResult(result ? "1" : "0");
}
/**
 * findSpectrum
 *    Returns the attributes of an existing spectrum.  This is a dict
 *    with the following keys:
 *    -  id     - spectrum_defs table id of root records
 *    -  name   - Name of spectrum.
 *    -  type   - spectrum type code.
 *    -  parameters - list of parameter names.
 *    -  axes   - list of axis definitions.  Each axis is a dict
 *                with the keys low, high and bins.
 *    - datatype - data type string.s
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
*/
void
TclSaveSet::findSpectrum(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 3, "findSpectrum needs only a spectrum name");
    std::string specname = objv[2];
    auto pSpec = m_pSaveSet->lookupSpectrum(specname.c_str());
    
    CTCLObject result;
    result.Bind(interp);
    makeSpectrumDict(result, pSpec);
    
    
    interp.setResult(result);
}
/**
 * listSpectra
 *    List the characteristics of all spectra in the saveset.
 *    The result is set to a (possibly empty).  Each list element
 *    is a dict of the form descibed in findSpectrum.
 *
 *    Format:
 *
 *    instance-cmd listSpectra
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
*/
void
TclSaveSet::listSpectra(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 2, "listSpectra does not take any parameters");
    auto spectra = m_pSaveSet->listSpectra();
    
    CTCLObject result;
    result.Bind(interp);
    for (int i =0; i < spectra.size(); i++) {
        CTCLObject element;
        element.Bind(interp);
        makeSpectrumDict(element, spectra[i]);
        result += element;
        delete spectra[i];
    }
    
    interp.setResult(result);
    
}

/**
 * create1dGate
 *    Create a 1d gate (slice or gamma slice e.g.).
 *
 *
 *    Format:
 *
 *    instance-cmd create1dGate name type parameter-list low high
 *
 *    Where parameter-list is a Tcl list of parameter names.
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
*/
void
TclSaveSet::create1dGate(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(
        objv, 7, "create1dGate needs name, type, parameter-list, low, high"
    );
    std::string name = objv[2];
    std::string type = objv[3];
    std::vector<const char*> parameterNames =
        listObjToConstCharVec(objv[4]);
    double low = objv[5];
    double high= objv[6];
    
    delete m_pSaveSet->create1dGate(
        name.c_str(), type.c_str(), parameterNames, low, high
    );
    
}
/**
 * create2dGate
 *    Create a 2d gate (e.g. gc).
 *
 *    Format:
 *
 *    instance-cmd create2dGate name type parameter-list point-list
 *
 *    Where point-list is a list of xy pairs e.g.
 *    [list {1.0 5.0} {5.0 70.)...]
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
*/
void
TclSaveSet::create2dGate(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(
        objv, 6, "create2dGate needs name, type, parameters and points"
    );
    std::string name = objv[2];
    std::string type = objv[3];
    std::vector<const char*> parameterNames =
        listObjToConstCharVec(objv[4]);
    std::vector<std::pair<double, double>> pts = pointsFromObj(objv[5]);
    
    delete m_pSaveSet->create2dGate(
        name.c_str(), type.c_str(), parameterNames, pts
    );
}
/**
 * createCompoundGate
 *    Creates a gate that depends on other gates (e.g. a * gate).
 *
 *    Format:
 *
 *    instance-cmd createCompoundGate name type gate-list
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
*/
void
TclSaveSet::createCompoundGate(
    CTCLInterpreter& interp, std::vector<CTCLObject>& objv
)
{
    requireExactly(objv, 5, "createCompoundGate needs name, type, gate-list");
    std::string name = objv[2];
    std::string type = objv[3];
    std::vector<const char*> gateNames = listObjToConstCharVec(objv[4]);
    
    delete m_pSaveSet->createCompoundGate(name.c_str(), type.c_str(), gateNames);
}
/**
 * createMaskGate
 *    Create a bitmask (e.g. em) gate.
 *    Format:
 *
 *    instance-cmd createMaskGate name type parameter mask
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
*/
void
TclSaveSet::createMaskGate(
    CTCLInterpreter& interp, std::vector<CTCLObject>& objv
)
{
    requireExactly(
        objv, 6, "createMaskGate needs name, type, parameter and mask-value"   
    );
    std::string name = objv[2];
    std::string type = objv[3];
    std::string param = objv[4];
    int         value = objv[5];
    
    delete m_pSaveSet->createMaskGate(
        name.c_str(), type.c_str(), param.c_str(), value
    );
}
/**
 * gateExists
 *   Sets the result to a boolean true or false depending on whether
 *   or not a gate exists:
 *
 *    Format:
 *
 *    instance-cmd gateExists gate-name.
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
*/
void
TclSaveSet::gateExists(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 3, "gateExists needs a gate name");
    std::string name = objv[2];
    
    interp.setResult(m_pSaveSet->gateExists(name.c_str()) ? "1" : "0");
}

/**
 *  findGate
 *    Returns information about an existing gate as a dict.
 *    Command format:
 *
 *    instance-cmd findGate gate-name
 *
 *   The result is set with  a dict that has the following keys:
 *
 *   -   id   - gate id.
 *   -   name - gate name.
 *   -   type - gate type.
 *   -   classification - gate classification: point, compound, mask.
 *   -   parameters (optional) List of names of parameters the gate needs.
 *   -   gates (optional) list of names of gates the gate depends on.
 *   -   points (optional) list of dicts containing x, y point coordinates.
 *   -   mask   (optional)  contains the mask value for bit masks.
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
*/
void
TclSaveSet::findGate(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 3, "findGate needs a gate name");
    std::string name = objv[2];
    DBGate* pGate    = m_pSaveSet->lookupGate(name.c_str());
    
    CTCLObject result;
    makeGateDict(interp, result, pGate);
    delete pGate;
    
    interp.setResult(result);
}
/**
 *  listGates
 *     List the gates in the save set. The result is set with a list
 *     of dicts.  Each dict has the form described by the comments
 *     in findGate.
 *     Command format:
 *
 *     instance-cmd listGates
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
*/
void
TclSaveSet::listGates(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 2, "listGates requires no additional parametes");
    
    auto gates = m_pSaveSet->listGates();
    CTCLObject result;
    result.Bind(interp);
    for (int i =0; i < gates.size(); i++) {
        CTCLObject gate;
        gate.Bind(interp);
        makeGateDict(interp, gate, gates[i]);
        delete gates[i];
        
        result += gate;
    }
    
    interp.setResult(result);
}
/**
 * applyGate
 *    Store a gate application to a spectrum.
 *    Command Format:
 *
 *    instance-cmd applyGate gate-name spectrum-name
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
*/
void
TclSaveSet::applyGate(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(
        objv, 4, "applyGate requires onliy a gate name and a spectrum name"
    );
    std::string gateName     = objv[2];
    std::string spectrumName = objv[3];
    
    delete m_pSaveSet->applyGate(gateName.c_str(), spectrumName.c_str());
}

/**
 * findApplication
 *   Returns information about a gate application.
 *   Command Format:
 *
 *   instance_cmd findApplication gate-name spectrum-name
 *
 * sets the result to be a dict that describes the application.
 * Keys:
 *    - id   id of the application in the table.
 *    - gate  name of the gate.
 *    - spectrum name of the spectrum the gate is applied to.
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 * @note if there is no application an exception is thrown.
*/
void
TclSaveSet::findApplication(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 4, "findApplication needs gate name and spectrum name only");
    
    std::string gate = objv[2];
    std::string spec = objv[3];
    
    auto pApp = m_pSaveSet->lookupApplication(gate.c_str(), spec.c_str());
    
    // Didn't throw so we've got one.
    
    CTCLObject result;
    
    makeApplicationDict(interp, result, pApp);
    
    
    interp.setResult(result);
}
/**
 * listApplications
 *    Makes a list of dicts that describe all of the gate applications
 *    currently made.
 *    The result is a list of dicts where each dict is described in
 *    findApplication's comments above.
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 * @note if there is no application an exception is thrown.
 */
void
TclSaveSet::listApplications(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 2, "listApplications takes no additional parameters");
    auto applications = m_pSaveSet->listApplications();
    CTCLObject result;
    result.Bind(interp);
    
    for (int i = 0; i < applications.size(); i++) {
        CTCLObject app;
        makeApplicationDict(interp, app, applications[i]);
        result += app;
        delete applications[i];
    }
    
    interp.setResult(result);
}
/**
 * createVariable
 *    Creats a tree variable definition in the saveset.
 *    Command format:
 *
 *    instance-cmd createVariable name value ?units?
 *
 *    Where the units field will be empty if no units are proviced.
 *    The value must be able to be coerced to a valid double.
 *    
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 * @note if there is no application an exception is thrown.
*/
void
TclSaveSet::createVariable(
    CTCLInterpreter& interp, std::vector<CTCLObject>& objv
)
{
        requireAtLeast(
            objv, 4, "createVariable requires at least a name and value"
        );
        requireAtMost(
            objv, 5, "createVariable requires at most a name, value and units"
        );
        
        std::string name = objv[2];
        double      value= objv[3];
        std::string units;
        if (objv.size() == 5) units = std::string(objv[4]);
        
        m_pSaveSet->createVariable(name.c_str(), value, units.c_str());
        
}
/**
 * variableExists
 *   Format:
 *
 *   instance-cmd variableExists varname
 *
 *   sets the result to boolean true if there's a tree variable with that
 *   name. otherwise sets it to false.
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 * @note if there is no application an exception is thrown.
*/
void
TclSaveSet::variableExists(
    CTCLInterpreter& interp, std::vector<CTCLObject>& objv
)
{
    requireExactly(objv, 3, "variableExists requires a variable name only");
    std::string varname = objv[2];
    
    bool result = m_pSaveSet->variableExists(varname.c_str());
    
    interp.setResult(result? "1" : "0");
}
/**
 * findVariable
 *   Format:
 *
 *   instance-variable findVariable varname
 *
 *   Sets the result with a dict that describes the variable.  The dict
 *   has the following keys:
 *
 *   -   id      - id of the variable in the treevariables table.
 *   -   name    - name of the variable
 *   -   value   - value of the variable.
 *   -   units   - units of the variable.  This is an empty string
 *                 if there are no units.
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 * @note if there is no application an exception is thrown.
*/
void
TclSaveSet::findVariable(
    CTCLInterpreter& interp, std::vector<CTCLObject>& objv
)
{
    requireExactly(objv, 3, "findVariable requires only a variable name");
    std::string name = objv[2];
    
    DBTreeVariable* pVar = m_pSaveSet->lookupVariable(name.c_str());
    
    CTCLObject result;
    makeVarDict(interp, result, pVar);
    
    interp.setResult(result);
    
}
/*
 * listVariables
 *    Format:
 *
 *    instance-cmd listVariables
 *
 *    Sets the result to a list of dicts that describe all variables
 *    defined in the save set. See findVariables for the contents of each
 *    dict.
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 * @note if there is no application an exception is thrown.
*/
void
TclSaveSet::listVariables(
    CTCLInterpreter& interp, std::vector<CTCLObject>& objv
)
{
    requireExactly(objv, 2, "listVariables does not require any other parameters");
    
    auto vars = m_pSaveSet->listVariables();
    CTCLObject result;
    result.Bind(interp);
    
    for (int i =0; i < vars.size(); i++) {
        CTCLObject dict;
        dict.Bind(interp);
        makeVarDict(interp, dict, vars[i]);
        delete vars[i];
        result += dict;
    }
    
    interp.setResult(result);
}
/**
 * hasChannels
 *   Format:
 *
 *   instance-cmd hasChannels spectrum-name
 *
 *   Sets the result with a boolean. True if spectrum-name
 *   has channel data stored, false otherwise.  Naturally an
 *   error is throw if the spectrum does not exist.
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 */
void
TclSaveSet::hasChannels(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 3, "hasChannels requires only a spectrum name parameter");
    std::string name = objv[2];
    auto spec = m_pSaveSet->lookupSpectrum(name.c_str());
    bool result = spec->hasStoredChannels();
    delete spec;
    
    interp.setResult(result ? "1": "0");
}
/**
 * storeChannels:
 *    Format:
 *
 *    instance-cmd  storeChannels spectrum-name channel-data
 *
 *  Where channel data is of the form {{xbin ?ybin? value}...}
 *  For 1d spectra, ybin is omitted.  Note that this is the
 *  form of the data from the SpecTcl scontents command.
 *  The channel data provided is stored for the named spectrum.
 * 
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 */
void
TclSaveSet::storeChannels(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(
        objv, 4, "storeChannels needs a spectrum name and channel data"
    );
    std::string name = objv[2];
    auto pSpec = m_pSaveSet->lookupSpectrum(name.c_str());
    CTCLObject& data(objv[3]);
    
    std::vector<DBSpectrum::ChannelSpec> chans;
    for (int i =0; i < data.llength(); i++) {
        CTCLObject channel = data.lindex(i);
        channel.Bind(interp);
        if ((channel.llength() != 3) && (channel.llength() != 2)) {
            throw std::invalid_argument("Bad channel data");
        }
        DBSpectrum::ChannelSpec c;
        c.s_y = 0;                  // If 1d.
        
        CTCLObject x = channel.lindex(0);
        x.Bind(interp);
        c.s_x = x;
        
        if (channel.llength() == 2) {
            CTCLObject value = channel.lindex(1);
            value.Bind(interp);
            c.s_value = value;
        } else {
            CTCLObject y = channel.lindex(1);
            CTCLObject value = channel.lindex(2);
            y.Bind(interp);
            c.s_y = y;
            
            value.Bind(interp);
            c.s_value = value;
        }
        
        chans.push_back(c);
    }
    
    
    pSpec->storeValues(chans);
    delete pSpec;
    
}
/**
 * getChannels
 *    Returns the contents of a stored spectrum.
 *    Format:
 *
 *    instance-cmd getChannels spectrum-name
 *
 *    The return value is a list of three element lists.
 *    Each three element list has the form:
 *    {xbin ybin value}  where the database makes no
 *    distinction between 1-d and 2d spectra.
 *    For reloading 1d spectra, just ignore the ybin.
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 *  @note it is an error to ask for channels from a spectrum
 *      that has none.  Use hasChannels first to ensure it does.
*/
void TclSaveSet::getChannels(
    CTCLInterpreter& interp, std::vector<CTCLObject>& objv
)
{
    requireExactly(objv, 3,"getChannels needs only a spectrum name");
    std::string name = objv[2];
    auto pSpec = m_pSaveSet->lookupSpectrum(name.c_str());
    
    auto chans = pSpec->getValues();
    CTCLObject result;
    result.Bind(interp);
    for (int i =0; i < chans.size(); i++) {
        CTCLObject point; point.Bind(interp);
        CTCLObject x; x.Bind(interp);
        CTCLObject y; y.Bind(interp);
        CTCLObject value; value.Bind(interp);
        
        x = chans[i].s_x;
        y = chans[i].s_y;
        value = chans[i].s_value;
        
        point += x;
        point += y;
        point += value;
        
        result += point;
    }
    
    interp.setResult(result);
}
/**
 * listRuns
 *   Set the result to the (possibly empty) list of run numbers
 *   of event data that were stored in this save-set.
 *
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 */
void
TclSaveSet::listRuns(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 2, "listRuns does not accept additional parameters");
    CTCLObject result;
    result.Bind(interp);
    auto runs = m_pSaveSet->listRuns();
    for (int i =0; i < runs.size(); i++) {
        result += runs[i];
    }
    interp.setResult(result);
}
/**
 * getRunInfo
 *   Returns a list of dicts that contain information about runs
 *   stored in this save set.
 *   Keys in this dict are:
 *   - config - the name of the save set that holds this run.
 *   - number - the runnumber.
 *   - title  - the run title string.
 *   - start_time - the timestamp of the start of the run.
 *                  [clock format] can make this human readable.
 *   - stop_time  - the timestamp for the end of the run.  This may not
 *                  be present if recording the run failed in any way.
 *   
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 *                 the command name.
 */
void
TclSaveSet::getRunInfo(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 2, "getRunInfo does not accept any additional parameters");
    CTCLObject result;
    result.Bind(interp);
    
    auto runs = m_pSaveSet->listRuns();
    for (int r = 0; r < runs.size(); r++) {
        int id = m_pSaveSet->openRun(runs[r]);
        auto info = m_pSaveSet->getRunInfo(id);
        CTCLObject dict;
        dict.Bind(interp);
        InitDict(interp, dict);
        AddKey(dict, "config", m_pSaveSet->getInfo().s_name.c_str());
        AddKey(dict, "number", info.s_runNumber);
        AddKey(dict, "title", info.s_title.c_str());
        AddKey(dict, "start_time", int(info.s_startTime));
        if (info.s_stopTime) {
            AddKey(dict, "stop_time", int(info.s_stopTime));
        }
        result += dict;
    }
    interp.setResult(result);
}
/**
 * getScalers
 *    Sets the result to a list of dicts that capture all of the
 *    scaler data stored in a run in a saveset.
 *    The result is a list of dicts.  Each dict represents a scaler
 *    readout and has the following keys:
 *    - sourceid - Id of the data source producing this scaler readout.
 *    -  start - sstart time offset into the run for this counting period.
 *    -  stop  - Stop time offset into the run for this counting interval
 *    - divisor - Value to divide stop/start by to get seconds.
 *    - timestamp- Time of day of the readout.
 *    - channels - List of channel values
 * @param interp - interpreter executing the command.
 * @param objv   - vector command paranmeters - including
 */
void
TclSaveSet::getScalers(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
    requireExactly(objv, 3, "getScalers needs a run number only");
    int run = objv[2];
    int id = m_pSaveSet->openRun(run);
    void* ctx = m_pSaveSet->openScalers(id);
    CTCLObject result;
    result.Bind(interp);
    SpecTclDB::SaveSet::ScalerReadout scalers;
    while(m_pSaveSet->readScaler(ctx, scalers)) {
        CTCLObject dict;
        dict.Bind(interp);
        InitDict(interp, dict);
        AddKey(dict, "sourceid", scalers.s_sourceId);
        AddKey(dict, "start", scalers.s_startOffset);
        AddKey(dict, "stop", scalers.s_stopOffset);
        AddKey(dict, "divisor", scalers.s_divisor);
        AddKey(dict, "timestamp", int(scalers.s_time));
        CTCLObject chans;
        chans.Bind(interp);
        for (int i =0; i < scalers.s_values.size(); i++) {
            chans += scalers.s_values[i];
        }
        AddKey(dict, "channels", chans);
        
        result += dict;
    }
    
    m_pSaveSet->closeScalers(ctx);
    interp.setResult(result);
}
////
// TclSaveSet private utilities:
//
/**
*  paramDefToObj
*     Create a parameter definition dict obj from a parameter
*     object pointer.
*  @param interp - interpreter reference
*  @param[out] obj - The result object
*  @param param - pointer to the parameter definition.
*/
void
TclSaveSet::paramDefToObj(
    CTCLInterpreter& interp, CTCLObject& obj, DBParameter* param
)
{
    InitDict(interp, obj);
    auto& info = param->getInfo();
    AddKey(obj, "id", info.s_id);
    AddKey(obj, "name", info.s_name.c_str());
    AddKey(obj, "number", info.s_number);
    
    if (info.s_haveMetadata) {
        AddKey(obj, "low", info.s_low);
        AddKey(obj, "high", info.s_high);
        AddKey(obj, "bins", info.s_bins);
        AddKey(obj, "units", info.s_units.c_str());
    }    
}

/**
 * listObjToConstCharVec
 *  Takes a CTCLObject reference and produces a vector of const char*
 *  that point to their string representations.
 *
 *  -  The CTCLObject must remain in scope for the duration of the
 *     use of that vector.
 *  -  Object shimmering is not a factor since all CTCLObject's
 *     underlying Tcl_Obj always has a string representation that's stored
 *     as a char*
 *
 * @note if the object is not a valid list there will be an exception.
 * @note the object must be bound to an interpreter.
 * @note we make use of the fact that Tcl_Obj is a copy on write beast.
 *
 * @param obj  - Reference to the object to decode.
 * @return std::vector<const char*>
 */
std::vector<const char*>
TclSaveSet::listObjToConstCharVec(CTCLObject& obj)
{
    std::vector<const char*> result;
    std::vector<CTCLObject> list = obj.getListElements(); // shimmers obj ->list
    
    // The const char*'s below only stay in scope because unless I
    // write to elmeents of list, they share data with obj, specifically
    // the string representation of each element of the list representation
    // of obj.
    
    for (int i = 0; i < list.size(); i++) {
        Tcl_Obj* el = list[i].getObject();
        result.push_back(Tcl_GetString(el));
    }
    
    return result;
    
}
/**
 * listObjToAxes
 *    Converts a  list object to an axis specification.
 *    This requires that each element of the list is a
 *    3 element list containing two objects with double representation
 *    and one with integer representation.
 *
 *  @param obj  - the object containing axis specifications.
 *  @return std::vector<SpectrumAxis>
 */
std::vector<SaveSet::SpectrumAxis>
TclSaveSet::listObjToAxes(CTCLObject& obj)
{
    std::vector<SaveSet::SpectrumAxis> result;
    CTCLInterpreter* pInterp = obj.getInterpreter();
    
    // Get the vector of axis objects:
    
    auto axes = obj.getListElements();
    for (int i =0; i < axes.size(); i++) {  // not sure this is needed but..
        axes[i].Bind(*pInterp);
    }
    // Each of those objects, in turn is a list that contains
    // in order low, high, bins.
    
    for (int  i = 0; i < axes.size(); i++) {
        auto axis = axes[i].getListElements();
        for (int j = 0; j < axis.size(); j++) {
            axis[i].Bind(*pInterp);
        }
        if (axis.size() != 3) {
            std::stringstream msg;
            msg << std::string(axis[i]) << " Is not a valid axis specification";
            throw std::invalid_argument(msg.str());
        }
        
        SaveSet::SpectrumAxis def;
        def.s_low =  double(axis[0]);
        def.s_high = double(axis[1]);
        def.s_bins = int(axis[2]);
        
        result.push_back(def);
    }
    
    return result;
}

/**
 * makeSpectrumDict
 *    Given a spectrum object pointer, creates a dict that
 *    describes the spectrum.  See findSpectrum for the structure
 *    of that dict.
 * @param[out] obj - the object into which the dict is built.  Must be
 *                   bound into an inteprreter.
 * @param pSpec    - Pointer to the spectrum object to describe.
 */
void
TclSaveSet::makeSpectrumDict(CTCLObject& obj, DBSpectrum* spec)
{
    InitDict(*obj.getInterpreter(), obj);
    
    // WE need the info object and the parameter names
    
    auto info   = spec->getInfo();
    auto pNames = spec->getParameterNames();
    auto xpNames = spec->getXParameterNames();
    auto ypNames = spec->getYParameterNames();
    
    AddKey(obj, "id", info.s_base.s_id);
    AddKey(obj, "name", info.s_base.s_name.c_str());
    AddKey(obj, "type", info.s_base.s_type.c_str());
    
    CTCLObject parameterNames;
    parameterNames.Bind(*obj.getInterpreter());
    stringVectorToList(*obj.getInterpreter(), parameterNames, pNames);
    AddKey(obj, "parameters", parameterNames);

    CTCLObject xpnames;
    xpnames.Bind(*obj.getInterpreter());
    stringVectorToList(*obj.getInterpreter(), xpnames, xpNames);
    AddKey(obj, "xparameters", xpnames);

    CTCLObject ypnames;
    ypnames.Bind(*obj.getInterpreter());
    stringVectorToList(*obj.getInterpreter(), ypnames, ypNames);
    AddKey(obj, "yparameters", ypnames);

    
    CTCLObject axes;
    axes.Bind(*obj.getInterpreter());
    for (int i =0; i < info.s_axes.size(); i++) {
        SaveSet::SpectrumAxis spaxis;
        spaxis.s_low = info.s_axes[i].s_low;
        spaxis.s_high= info.s_axes[i].s_high;
        spaxis.s_bins= info.s_axes[i].s_bins;
        CTCLObject axis;
        axis.Bind(*obj.getInterpreter());
        makeAxisDict(*obj.getInterpreter(), axis, spaxis);
        axes += axis;
    }
    AddKey(obj, "axes", axes);
    
    AddKey(obj, "datatype", info.s_base.s_dataType.c_str());
}
/**
 * stringVectorToList
 *    Takes a vector of strings and makes a list object out of them.
 * @param[out] obj   - Object the list will be lappended to must be bound.
 * @param strings    - Vector of strigns to append.
 */
void
TclSaveSet::stringVectorToList(
    CTCLInterpreter& interp, CTCLObject& obj,
    const std::vector<std::string>& strings
)
{
    for (int i =0; i < strings.size(); i++) {
        CTCLObject element;
        element.Bind(*obj.getInterpreter());
        element = strings[i];
        obj += element;
        
    }
}
/**
 * makeAxisDict
 *    Given a spectrum axis struct, produces a spectrum axis dict.
 *
 *  @param interp   - intepreter object to use to create the dict.
 *  @param[out] obj - object to turn inot the dict.
 *  @param  axes    - Vector of axis struct.
 */
void
TclSaveSet::makeAxisDict(
    CTCLInterpreter& interp, CTCLObject& obj,
    const SaveSet::SpectrumAxis& axis
)
{
    InitDict(interp, obj);
    AddKey(obj, "low", axis.s_low);
    AddKey(obj, "high", axis.s_high);
    AddKey(obj, "bins", axis.s_bins);
}

/**
 * pointsFromObj
 *    Given a Tcl object that's a list of 2 element sublists,
 *    each pair representing a coordinate point, decodes
 *    all of this into a vector of points.
 *
 *
 * @param obj   - object to decode
 * @return std::vector<std::pair<double, double> >- points
 */
std::vector<std::pair<double,double>>
TclSaveSet::pointsFromObj(CTCLObject& obj)
{
    std::vector<std::pair<double, double>> result;
    for (int i=0; i < obj.llength(); i++) {
        CTCLObject elobj = obj.lindex(i);
        elobj.Bind(*obj.getInterpreter());
        if (elobj.llength() != 2) {
            std::stringstream msg;
            msg << std::string(elobj)
                << " is not a 2 element list and therefore can't be a point";
            throw std::invalid_argument(msg.str());
        }
        CTCLObject xpt = elobj.lindex(0);
        CTCLObject ypt = elobj.lindex(1);
        xpt.Bind(*obj.getInterpreter());
        ypt.Bind(*obj.getInterpreter());
        
        std::pair<double, double> pt = {double(xpt), double(ypt)};
        result.push_back(pt);
    }
    return result;
}
/**
 * makeGateDict
 *   Given a reference to an object and the pointer to a gate description,
 *   fills the object with a dict that describes the gate:
 *
 *   The result is set with  a dict that has the following keys:
 *
 *   -   id   - gate id.
 *   -   name - gate name.
 *   -   type - gate type.
 *   -   classification - gate classification: point, compound, mask.
 *   -   parameters (optional) List of names of parameters the gate needs.
 *   -   gates (optional) list of names of gates the gate depends on.
 *   -   points (optional) list of dicts containing x, y point coordinates.
 *   -   mask   (optional)  contains the mask value for bit masks.
 * @param interp  the interpreter to which objects will be bound.
 * @param obj[out] the object that will get the gate description.
 * @param pGate   pointer to the gate object.
 */
void
TclSaveSet::makeGateDict(CTCLInterpreter& interp, CTCLObject& obj, DBGate* pGate)
{
    auto info = pGate->getInfo();
    obj.Bind(interp);
    InitDict(interp, obj);
    AddKey(obj, "id", info.s_info.s_id);
    AddKey(obj, "name", info.s_info.s_name.c_str());
    AddKey(obj, "type", info.s_info.s_type.c_str());
    std::string classification =
        gateClassificationToString(info.s_info.s_basictype);
    AddKey(obj, "classification", classification.c_str());
    
    // Only point and mask gates have parameters:
    
    if ((info.s_info.s_basictype == DBGate::point)   ||
        (info.s_info.s_basictype == DBGate::mask)
        ) {
        auto parameterNames = pGate->getParameters();
        CTCLObject params;
        params.Bind(interp);
        
        stringVectorToList(interp, params, parameterNames);
        AddKey(obj, "parameters", params);
        
    }
    // Only compound gates have gate membes:
    
    if (info.s_info.s_basictype == DBGate::compound) {
        auto gateNames = pGate->getGates();
        CTCLObject gates;
        gates.Bind(interp);
        stringVectorToList(interp, gates, gateNames);
        AddKey(obj, "gates", gates);
    }
    // only point gates have points:
    
    if (info.s_info.s_basictype == DBGate::point) {
        CTCLObject pts;
        gatePointsToDictList(interp, pts, info.s_points);
        AddKey(obj, "points", pts);
    }
    // Mask gates have a mask:
    
    if (info.s_info.s_basictype == DBGate::mask) {
        AddKey(obj, "mask", info.s_mask);
    }
}
/**
 * makeApplicationDict
 *    Fill an object in with the dictionary that describes an application.
 *
 * @param interp - interpreter used to bind objects.
 * @param[out] obj - the object to fill in.
 * @param pApp     - pointer to the application object.
 */
void
TclSaveSet::makeApplicationDict(
    CTCLInterpreter& interp, CTCLObject& obj, DBApplication* pApp
)
{
    obj.Bind(interp);
    std::string gate = pApp->getGateName();
    std::string spec = pApp->getSpectrumName();
    InitDict(interp, obj);
    
    AddKey(obj, "id", pApp->getInfo().s_id);
    AddKey(obj, "gate", gate.c_str());
    AddKey(obj, "spectrum", spec.c_str());

}

/**
 * makeVarDict
 *    Make a dict describing a single treevariable.
 *    See findVariable's comment string for information about
 *    what's in the dict.
 * @param interp - TCL Interpreter that will be used to bind
 *                 objects.
 * @param[out] obj - object that will have the dict written into it.
 * @parm pVar      - Pointer to the variable description to be described
 *                   in the dict.
 */
void
TclSaveSet::makeVarDict(
    CTCLInterpreter& interp, CTCLObject& obj, DBTreeVariable* pVar
)
{
    obj.Bind(interp);
    InitDict(interp, obj);
    
    auto info = pVar->getInfo();
    AddKey(obj, "id", info.s_id);
    AddKey(obj, "name", info.s_name.c_str());
    AddKey(obj, "value", info.s_value);
    AddKey(obj, "units", info.s_units.c_str());
} 
//////


////////////////////////////////////////////////////////////////
// Unbound static functions

/// Dictionary mainpulation:

/**
 * InitDict
 *    Given a CTCLObject sets it to an initialized empty dict.
 * @param interp - intepreter.
 * @param obj    - object to so initialize.
 */

static void InitDict(CTCLInterpreter& interp, CTCLObject& obj)
{
    Tcl_Obj* o = Tcl_NewDictObj();
    
    obj   = o;
    obj.Bind(interp);
}
/**
 * AddKey
 *   overloads to add key/value pairs to the dictionary.
 * @param dict - dict object from e.g. InitDIct.
 * @param key  - key to add.
 * @param value - value to associated with the key.
 */

static void AddKey(CTCLObject& dict, const char* key, int value)
{
    
    
    CTCLObject oValue;
    oValue.Bind(*dict.getInterpreter());
    oValue = value;
    
    AddKey(dict, key, oValue);
    
}
static void AddKey(CTCLObject& dict, const char* key, const char* value)
{
    CTCLObject oValue;
    oValue.Bind(*dict.getInterpreter());
    oValue = value;
    
    AddKey(dict, key, oValue);
}
static void AddKey(CTCLObject& dict, const char* key, double value)
{
    CTCLObject oValue;
    oValue.Bind(*dict.getInterpreter());
    oValue = value;
    
    AddKey(dict, key, oValue);
}

static void AddKey(CTCLObject& dict, const char* key, CTCLObject& value)
{
    CTCLObject okey;
    okey.Bind(*dict.getInterpreter());
    okey = key;
    
    Tcl_DictObjPut(
        dict.getInterpreter()->getInterpreter(),
        dict.getObject(), okey.getObject(), value.getObject()
    );
}

/**
 * gateClassificationToString
 *    Convert a gate clasification to a string.
 *  @param c - classification from the BaseInfo s_basictype field.
 *  @return std::string
 */
std::string
gateClassificationToString(DBGate::BasicGateType c)
{
    std::string result;
    
    switch (c) {
    case DBGate::point:
        result = "point";
        break;
    case DBGate::compound:
        result = "compound";
        break;
    case DBGate::mask:
        result = "mask";
        break;
    default:
        throw std::logic_error(
            "Invalid gate classification in gateClassificationToString"
        );
    }
    
    return result;
}
/**
 * Turns a vector of gate points  to a list of dicts with x,y keys.
 *
 *  @param interp      - interpreter that will be used to bind objects.
 *  @param[out] obj    - Object that will get the dict written into it.
 *  @param pts         - Gate points.
 */
void gatePointsToDictList(
    CTCLInterpreter& interp, CTCLObject& obj,
    const DBGate::Points& pts
)
{
    obj.Bind(interp);
    for (int i =0; i < pts.size(); i++) {
        CTCLObject point;
        point.Bind(interp);
        InitDict(interp, point);
        AddKey(point, "x", pts[i].s_x);
        AddKey(point, "y", pts[i].s_y);
        obj += point;
    }
    
}


}                // SpecTclDB Namespace