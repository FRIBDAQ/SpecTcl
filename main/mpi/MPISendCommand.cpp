/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2014.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Giordano Cerizza
             Simon Giraud
             Aaron Chester
             Jin Hee Chang
	     FRIB
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

#include "MPISendCommand.h"
#include "TclPump.h"
#include "TCLInterpreter.h"
#include "TCLObject.h"
#include "Exception.h"
#include "Globals.h"
#include <stdexcept>
#include <string>
#include <sstream>    // For formatting error messages.
/**
 *  constructor
 *    @param interp - interpreter on which to registesr mpi::send.
 */
CMPISendCommand::CMPISendCommand(CTCLInterpreter& interp) :
    CTCLObjectProcessor(interp, "mpi::send", TCLPLUS::kfTRUE) {}

/** 
 * destructor
 */
CMPISendCommand::~CMPISendCommand() {}

/** 
 * operator() 
 *    Gets control when the command runs.  This is pretty simple:
 *   - Require three total parameters (command, rank-list and script).
 *   - If executeScript returns true...uhm.. execute the script. Catching exceptions ->
 *     TCL_ERROR/result.
 * 
 * @param interp - interpreter executing the command/script.
 * @param objv   - The command words.
 */
int
CMPISendCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv) {
    bindAll(interp, objv);
    try {
        requireExactly(objv, 3, "Incorrect number of command line parameters.");
    } 
    catch (std::string msg) {
        std::string resultString = msg;
        resultString += "\n";
        resultString += Usage();

        CTCLObject result;
        result.Bind(interp);
        result = resultString;
        interp.setResult(result);
        return TCL_ERROR;
    }

    if (executeScript(objv[1])) {
        std::string script = objv[2];
        try {
            interp.GlobalEval(script);
        }
        catch (CException& e) {   // TODO - add in usage.
            interp.setResult(e.ReasonText());
        }
        catch (std::string s) {
            interp.setResult(s);
        }
        catch (std::exception& e) {
            interp.setResult(e.what());
        }
        catch (...) {
            interp.setResult("mpi::send - unanticipated exception type");
        }

        return TCL_ERROR;
    }
    return TCL_OK;
}

/////////////////////////////// Private methods.

/**
 * executeScript 
 *    @param roleList - list of roles/ranks.
 *    @return bool    - True if we should execute the script here.
 * 
 */
bool
CMPISendCommand::executeScript(CTCLObject& roleList) {
    // If not mpi then by all means run it:

    if (!isMpiApp()) return true;

    auto ranks = computeRanks(roleList);
    auto me    = myRank();

    return ranks.count(me) > 0;
}
/**
 *  computeRanks
 *    Determine the set of ranks covered by the rolses in an TCL Object that
 * is a list.  Elements of the list can be:
 *   - unsigned integers - rank numbers.
 *   - role nams ('root', 'event-sink', 'worker').
 * 
 * If an element is not one of those a std::invalid_argument exception is thrown which is ultimately
 * caught by operator() and turned into a TCL_ERROR.
 * 
 * @param roleList - the object with the role list.
 * @param std::set<unsigned> - Contains all of the ranks mentioned.
 * 
 */
std::set<unsigned>
CMPISendCommand::computeRanks(CTCLObject& roleList) {
    auto interp = roleList.getInterpreter();
    unsigned me = myRank();
    std::set<unsigned> result;

    // Process role list elements:

    for (int i =0; i < roleList.llength(); i++) {
        CTCLObject item = roleList.lindex(i);
        item.Bind(interp);

        // Try it as an unsigned int:

        try {
            int rank = item;
            if (rank < 0) {
                throw std::invalid_argument("Rank numbers must not be negative");
            }
            result.insert(static_cast<unsigned>(rank));
            continue;
        }
        catch (std::exception& e) {
            throw;                  // Unsigned.
        }
        catch (...) {
            // Could be a textual role:

            std::string role = item;
            if (roleMyRank(item)) {
                result.insert(me);             // I'm in the list.
                break;                         // Really don't need to go any further.
            }
        }
    }

    return result;
}
/**
 *  roleMyRank
 *     Given a string:
 * 
 * @return bool
 * @retval true if the process rank is in that role.
 * @retval false if the process rank is not in that role.
 * @throw std::invalid_argument - if the role name is not valid.
 * 
 */
bool
CMPISendCommand::roleMyRank(const std::string& role) {
    // Validate the role string:

    if (role != "root" && role != "event-sink" && role != "worker") {
        std::stringstream msgStream;
        msgStream << role << " is not a valid role must be one of 'root', 'event-sink' or 'worker'";
        std::string msg = msgStream.str();
        throw std::invalid_argument(msg);
    }
    return myRole() == role;
}
/**
 *  myRole
 *    @return std::string - the textual role name of the process.
 */
std::string
CMPISendCommand::myRole() {
    int role = myRank();
    if (role == MPI_ROOT_RANK) return "root";
    if (role == MPI_EVENT_SINK_RANK) return "event-sink";
    return "worker";
}

/**
 * Usage
 *    Output a usage string.
 * 
 * @return std::string
 */
std::string
CMPISendCommand::Usage() {
    std::string usage;
    usage += "Usage\n";
    usage += "   mpi::send target-list script\n";
    usage += "Where:\n";
    usage += "   target-list is a Tcl list of who should run the command. This can be\n";
    usage += "         a mix of MPI ranks numbers and role names where the roles are\n";
    usage += "         'root', 'event-sink' and 'worker'\n";
    usage += "    script - is a script that will be executed in the target-list.\n";
    usage += "       The script status will be the worst of the statuses and the result\n";
    usage += "       string will be the longest of them.\n";

    return usage;
}
