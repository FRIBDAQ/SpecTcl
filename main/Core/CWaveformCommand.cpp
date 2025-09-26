/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2005.

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
 *  @file CWaveformCommand.cpp
 *  @brief implementation of the waveform command.
 */

#include "CWaveformCommand.h"
#include "CWaveForm.h"
#include "SpecTcl.h"

#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <TclDict.h>
#include <Exception.h>
#include <stdexcept>
#include <sstream>    // for message formatting.
#include <tcl.h>
#include <TclPump.h>

 /**
  * constructor
  *    We call the command spectcl::serial::waveform.  The MPI relay that will actually be constructed
  * when all is done will be called waveform.
  * 
  * @param interp - interpreter on which the command will be registered.
  */

CWaveformCommand::CWaveformCommand(CTCLInterpreter& interp) :
    CTCLObjectProcessor(interp, "spectcl::serial::waveform", kfTRUE) {}
/**
 *  destructor 
 *    null for now as the anticipated lifetime is program.
 */

CWaveformCommand::~CWaveformCommand() {}

/**
 *  operator()
 *     Gains control when the waveform command is encountered in a script or in immediate execution.
 * This method simply:
 *    * Establishes the ability for subcommand handlers to signal errors via exception throws.
 *    * Determines if there is a sub command and, if so, dispatches to the appropriate subcommand handler.
 *    * If there is not a recognized subcommand, treats the remainder of the command as if it were a 
 *      create operation.  The details of each subcommand's parameters and return value are described
 *      in the comment headers for the subcommand handlers.
 */
int
CWaveformCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv) {
    try {
        bindAll(interp, objv);       // So subcommands don't have to.
        requireAtLeast(
            objv, 2, 
            "Insufficient number of command parameters");  // Even implied create has a parameter.
        std::string sub = objv[1];
        if(sub == "create") {
            create(interp, objv, 2);              // Name is after create subcommand.
        } else if (sub == "list") {
            list(interp, objv);
        } else if (sub == "get") {
            get(interp, objv);
        } else if (sub == "metadata") {
            metadata(interp, objv);
        } else if (sub == "resize") {
            resize(interp, objv);
        } else {
            create(interp, objv, 1);              // no subcommand, assume create.
        }

    }
    catch (std::string msg) {
        interp.setResult(msg);
        return TCL_ERROR;
    }
    catch (std::exception& e) {
        interp.setResult(e.what());
        return TCL_ERROR;
    }
    catch (CException& e) {
        interp.setResult(e.ReasonText());
        return TCL_ERROR;
    }
    catch (...) {
        interp.setResult("Unexpected exception type executing waveform command");
        return TCL_ERROR;
    }

    return TCL_OK;
}
///////////////////////// protected methods are sub-command handlers. //////////////////////////////////

/**
 * create:
 * 
 * \verbatim
 *      waveform create name samples
 * \endverbatim
 * 
 * Where:
 *   - name is a unique name to give to the waveform object.
 *   - samples is the number of samples the waveform holds.
 * 
 * Result is the name of the waveform if successful.
 * 
 * e.g.
 * \verbatim
 *  puts [waveform create george 1234]
 * \endverbatim
 * 
 * will output "george".
 * 
 * @param interp - the interpreter executing the command and in which the result will be left.
 * @param objv   - the complete vector of command words.
 * @param nameIndex - The index within objv where the name can be expected to live.
 * @note objv.size() - nameIndex must be exactly 1 as the only other command word allowed 
 *                     is the sample count.
 * @throw CTCLException for conversion errors
 * @throw std::string incorrect number of command parameters.
 * @throw std::invalid_argument - duplicate name or nsamples <= 0.
 */
void
CWaveformCommand::create(CTCLInterpreter& interp, std::vector<CTCLObject>& objv, unsigned nameIndex) {
    if ((objv.size() - nameIndex) != 2) {
        throw std::string("waveform ?create? incorrect number of command parameters");
    }
    std::string name = objv[nameIndex];
    int         samples = objv[nameIndex+1];   // can throw CTCLException.

    // The name must be unique:

    if (find(name.c_str())) {
        std::stringstream msg;
        msg << "A waveform object called: " << name << " already exists.";
        std::string smsg = msg.str();
        throw std::invalid_argument(smsg);
    }
    // Samples must be > 0

    if (samples <= 0) {
        std::stringstream msg;
        msg << "Waveform objects must have at least one sample you tried to make one with "
            << samples;
        std::string smsg(msg.str());
        throw std::invalid_argument(smsg);
    }

    // Should work by now:

    save(CWaveform(name.c_str(), samples));

    interp.setResult(name);
}
/**
 * list
 *    lists waveform properties that match a pattern. Command format:
 * 
 * \verbatim
 *    waveform list ?pattern?
 * \endverbatim
 * 
 * If not supplied pattern defaults to "*" which matches all patterns.
 * 
 * The result is a list where elements of the list are a waveform.  each element is a
 * dict which has key/values :
 * 
 * - name - name of the waveform.
 * - samples - number of samples in the waveform.
 * - metadata - a dict containing the metadata with keys metadata item names and values those values.
 * 
 * Example for such a dict might be:
 * 
 * \verbatim
 *  {name cc samples 100 metadata {description {Central contact} frequency 250MHz}}
 * \endverbatim
 * 
 * @param interp - interpreter executing the command.
 * @param objv   - The complete vector of command words.  There can be at most 3.
 * @throw std::string - there are too many command line parameters.
 * This command should always be successful, if no waveforms have names matching the pattern,
 * the result is just an empty  list.
 */
void
CWaveformCommand::list(CTCLInterpreter& interp, std::vector<CTCLObject>& objv) {
    requireAtMost(objv, 3, "Too many command line parameters"); // throws std::string maybe.

    std::string pattern = "*";            // default pattern.
    if (objv.size() == 3) {
        pattern = std::string(objv[2]);                // User supplied pattern.
    }
    // set up the result list:

    CTCLObject result;
    result.Bind(interp);

    // We use the SpecTcl API rather than direct for iteration to be consistent>

    auto pApi = SpecTcl::getInstance();
    auto p    = pApi->waveformBegin();
    auto end  = pApi->waveformEnd();

    
    for (; p != end; ++p) {
        if (Tcl_StringMatch(p->first.c_str(), pattern.c_str())) {
            CTCLObject description;
            description.Bind(interp);
            listWaveform(description, p->second);
            result += description;
        }
    }
    interp.setResult(result);
}
/** 
 *  get
 *     Get the waveform data fromt he specified waveform objects.
 * 
 * \verbatim
 *    waveform get name...
 * \endverbatim
 * 
 * On success, the result is a list of lists of waveforms:
 * Each waveform object can, in the MPI case, produce a list of current waveforms, one for each
 * worker process. If not We do that wrapping here. for example an MPI waveform for 2 workers might look like:
 * The waveform is a list containng the waveform name followed by a list of available trace data
 * For example in two workers:
 * 
 * 
 * \verbatim
 *  {name { waveform from worker one} { waveform from second worker} }
 * \endverbatim
 * 
 * If more than one waveform is asked for then you might  get:
 * \verbatim
 *  {name1 { waveform from worker one} { waveform from second worker}} {name2 { waveform from worker one} { waveform from second worker} }
 * \endverbatim
 * 
 * In serial SpecTcl, there's naturally only one waveform.
 *  
 * @param interp - interpreter running the command.
 * @param objv   - the complete vector of command words.
 * @throw string - if invalid # of command words.
 * @throw std::invalid_argument - if a waveform name does not exist.
 */
void
CWaveformCommand::get(CTCLInterpreter& interp, std::vector<CTCLObject>& objv) {
    requireAtLeast(objv, 3, "waveform get requires at least one waveform name");

    // Marshall the waveforms up into a vector, We throw std::invalid_argumeent if
    // we one is not defined.


    std::vector<CWaveform*> waveforms;
    for (int i =2; i < objv.size(); i++) {   //  Iteration over names.
        std::string name = objv[i];
        CWaveform* pWf = find(name.c_str());
        if (!pWf) {
            // No such:

            std::stringstream smsg;
            smsg << "waveform get - there is no waveform named " << name;
            std::string s = smsg.str();
            throw std::invalid_argument(s);
        }
    }

    // Now loop over the waveforms, getting a decription for each and appending it to a result:

    CTCLObject result;
    result.Bind(interp);
    for (auto p : waveforms) {
        CTCLObject item;
        item.Bind(interp);
        getWaveform(item, *p);             // Does the work for one wf.
        result += item;
    }
    // result is ready to set.

    interp.setResult(result);
}
/**
 * metadata
 *    This is itself a subensemble:
 * 
 * \verbatim
 *   waveform metadata get|set ...
 * \endverbatim
 * 
 * We just ensure there is a sub-subcommand and dispatch it. If get or set is not provided,
 * this is an invalid_argument exception.
 * 
 * @param interp - interpreter executing the command.
 * @param objv   - The full vector of command words.
 * @throw std::string - no sub-subcommand.
 * @throw std::invalid_argument - sub-subcommand given but it's invalid
 * 
 * The result depends on the sub-subcommand and is documented in it handlers.
 */
void
CWaveformCommand::metadata(CTCLInterpreter& interp, std::vector<CTCLObject>& objv) {
    requireAtLeast(objv, 3, "waveform metdata requires set | get sub-subcommand");
    std::string sub = objv[2];   // subsubcommand:

    if (sub == "set") {
        metadataSet(interp, objv);
    } else if (sub == "get") {
        metadataGet(interp, objv);
    } else {
        // Illegal:

        std::stringstream smsg;
        smsg << "waveform metadata needs set or get you supplied: " << sub;
        std::string msg(smsg.str());
        throw std::invalid_argument(msg);
    }

}
/**
 * metadataSet
 *    Set metadata for a waveform.
 * 
 * \verbatim
 *    waveform metadata set wfname name1 value1 ...
 * \verbatim
 * 
 * No result is set for the command.
 * 
 * Sets one or more metadata items for the wave form wfname.  Requires at least
 * 6 parameters and an even number of parameters.   
 * 
 *   @param interp interpreter executing the command.
 *   @param objv   complete vector of command words.
 *   @throw std::string - too few parameters.
 *   @throw std::invalid_argument - no such waveform.
 *   @throw std::string not an even number of parametrers.
 *   @note all checks are done before any metadata are modified.
 */
void
CWaveformCommand::metadataSet(CTCLInterpreter& interp, std::vector<CTCLObject>& objv) {
    requireAtLeast(
        objv, 6, 
        "waveform metadata set needs at least a waveform and a name value pair.");
    if (objv.size() % 2) {
        throw std::string("waveform metadata set needs an even number of command words.");
    }

    std::string wfname = objv[3];
    auto wf = find(wfname.c_str());
    if (!wf) {
        std::stringstream smsg;
        smsg << "waveform metadata set No such waveform: " << wfname;
        std::string msg(smsg.str());
        throw std::invalid_argument(msg);
    }

    // Everyting checks out, now set the metadata items:

    for (int i = 7; i < objv.size(); i+=2) {
        std::string name  = objv[i];
        std::string value = objv[i+1];

        wf->setMetadata(name.c_str(), value.c_str());
    }

}
/**
 * metadataGet
 * 
 *    Retrieves metadata from a waveform:
 * 
 * \verbatim
 *  waveform metadata get wfname ?mdname?
 * \endverbatim
 * 
 * If mdname is provided that is the metadata that is fetched.  If not all metadata is fetched.
 * 
 * The result, on success will be  a dict of metadata name keys with the values as their values.
 * Note this has a list representation of {name value name value...}  which the user can shimmer into a dict.
 * 
 * @param interp - interpreter executing the command.
 * @param objv   - full vector of command words.
 * @throw std::string - extra parameters provided.
 * @throw std::invalid_argument no such waveform.
 * @throw CNoSuchObjectException if there's no metadata with this name.
 * 
 */
void
CWaveformCommand::metadataGet(CTCLInterpreter& interp, std::vector<CTCLObject>& objv) {
    requireAtMost(objv, 5, "waveform metadata get too many command line parameters");
    
    std::string wfName = objv[3];
    CWaveform*  pWf    = find(wfName.c_str());
    if (!pWf) {
        std::stringstream smsg;
        smsg << "waveform metadata get no such waveform " << wfName;
        std::string msg(smsg.str());
        throw std::invalid_argument(msg);
    }
    CTCLObject result;
    result.Bind(interp);
    

    if (objv.size() == 5) { // A metadata name is supplied:
        std::string mdName = objv[4];
        std::string mdValue = pWf->getMetadata(mdName.c_str());  // can throw CNoSuchObjectException.

        result += mdName.c_str();
        result += mdValue.c_str();
    } else {              // Dump all metadata.
        auto& md = pWf->getMetadata();
        for (auto& item : md) {
            std::string name = item.first;
            std::string value = item.second;

            result += name.c_str();
            result += value.c_str();
        }
    }
    // set the result:

    interp.setResult(result);
}

/**
 * resize
 *    Resize the waveform, that is set a new number of samples.
 * 
 * \verbatim
 *    waveform resize name new-samples
 * \endverbatim
 * 
 * No result is set.
 * 
 * @param interp - interpreter running the command.
 * @param objv   - the complete vector of command words.
 * 
 * @throw std::string - incorrect number of command words.
 * @throw std::invalid_argument - no such waveform.
 * @throw CTCLException - samples are not convertible to integers.
 * @throw std::invalid_argument samples are not > 0 (must be at least one sample).
 * 
 */
void
CWaveformCommand::resize(CTCLInterpreter& interp, std::vector<CTCLObject>& objv) {
    requireExactly(objv, 4, "Incorrect number of command line parameters for waveform resize");
    std::string wfName = objv[2];

    // Waveform must exist:

    CWaveform* wf = find(wfName.c_str());
    if (!wf) {
        std::stringstream smsg;
        smsg << "waveform resize - no such waveform: " << wfName;
        std::string msg(smsg.str());
        throw std::invalid_argument(msg);
    }

    // validate the new sample count:

    int newSamples = objv[3];              // Can throw CTCLException.
    if (newSamples <= 0) {
        std::stringstream smsg;
        smsg << "waveform resize - new sample count must be greater than 0 not : " 
            << newSamples;
        std::string msg(smsg.str());
        throw std::invalid_argument(msg);
    }

    // Now it must work:

    wf->resize(newSamples);
}

//////////////////////////////// private utilities ///////////////////////////////////////////////

/**
 * find
 *   Use the SpecTcl API to locate a waveform object.
 * 
 * @param name - name of the waveformto find.
 * @return CWaveform*  -  pointer to the waveform.
 * @retval nullptr if no match.
 */
CWaveform*
CWaveformCommand::find(const char* name) {
    SpecTcl* pApi = SpecTcl::getInstance();
    return pApi->findWaveform(name);
}
/**
 * save
 *     Savfe a waveform in the dictionary via the SpecTcl API.
 * 
 * @param wf - waveform that is _copied_ into the dict.
 * @throw CDuplicateSingleton if a waveform by that name already exists.
 */
void
CWaveformCommand::save(const CWaveform& wf) {
    SpecTcl* pApi = SpecTcl::getInstance();
    pApi->addWaveform(wf);
}
/** 
 * listWaveform
 *     Constructs the dict that describes a waveform. \
 * 
 * @param result - references the bound result into which the dict is created.
 * @param wf     - references the waveform to describe.
 * 
 * @note see the comment header to CWaveformCommand::list for a description of the dict we crate.
 * 
 */
void
CWaveformCommand::listWaveform(CTCLObject& result, const CWaveform& wf) {
    using  Tcl::DictPut;
    CTCLInterpreter* pInterp = result.getInterpreter();    // we'll need this.


    // First the name and the samples:

    DictPut(*pInterp, result, "name", wf.getName().c_str());
    DictPut(*pInterp, result, "samples", (int)wf.size());

    //  The subdict for the metadata:

    CTCLObject metadata;
    metadata.Bind(*pInterp);
    for (auto& item: wf.getMetadata()) {
        std::string name = item.first;
        std::string value = item.second;
        DictPut(*pInterp, metadata, name.c_str(), value.c_str());
    }

    DictPut(*pInterp, result, "metadata", metadata);
}



/**
 * getWaveform
 * 
 *    Returns a list for the waveform  values. This is of the form:
 * 
 * \verbatim 
 *  {name {pts}}
 * \endverbatim
 * 
 * @param result - bound object to hold the result.
 * @param wf     - references the waveform.
 * 
 */
void
CWaveformCommand::getWaveform(CTCLObject& result, const CWaveform& wf) {
    CTCLInterpreter* pInterp = result.getInterpreter();

    result += wf.getName();
    
    CTCLObject points;
    points.Bind(*pInterp);

    auto& trace = wf.trace();
    for (int p : trace) {
        points += p;
    }
    result += points;

}