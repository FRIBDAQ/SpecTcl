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

#include <TCLInterpreter.h>
#include <TCLObject.h>

#include <stdexcept>
#include <sstream>
#include <string>

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
        requireAtLeast(objv, 2, "Save set commands need a subcommand");
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
}                          // SpecTcl namespace.