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

static void InitDict(CTCLInterpreter& interp, CTCLObject& objv);
static void AddKey(CTCLObject& dict, const char* key, int value);
static void AddKey(CTCLObject& dict, const char* key, const char* value);
static void AddKey(CTCLObject& dict, const char* key, double value);
static void AddKey(CTCLObject& dict, const char* key, CTCLObject& value);


namespace SpecTcl {
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
    
    SpecTcl::CDatabase::create(filename.c_str());
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
    SpecTcl::CDatabase* pDb = new SpecTcl::CDatabase(fname.c_str());
    
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
int  TclSaveSet::m_spectrumSerial(0);
int  TclSaveSet::m_gateSerial(0);
int  TclSaveSet::m_applicationSerial(0);
int  TclSaveSet::m_treeVariableSerial(0);

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
    
    AddKey(obj, "id", info.s_base.s_id);
    AddKey(obj, "name", info.s_base.s_name.c_str());
    AddKey(obj, "type", info.s_base.s_type.c_str());
    
    CTCLObject parameterNames;
    parameterNames.Bind(*obj.getInterpreter());
    stringVectorToList(*obj.getInterpreter(), parameterNames, pNames);
    AddKey(obj, "parameters", parameterNames);
    
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
//////

}                          // SpecTcl namespace.

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