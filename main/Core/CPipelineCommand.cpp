#include "CPipelineCommand.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <sstream>
#include <stdexcept>
#include <Exception.h>
#include <tcl.h>
#include <SpecTcl.h>
#include <CPipelineManager.h>
#include "SpecTcl.h"

CPipelineCommandActual::CPipelineCommandActual(CTCLInterpreter& interp) :
  CTCLObjectProcessor(interp, "::spectcl::serial::pman" , true),
  m_manager(CPipelineManager::getInstance())
{

}

int
CPipelineCommandActual::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  // Only run if there is an analyzer whose pipeline can be manipulated:

  if (!SpecTcl::getInstance()->GetAnalyzer()) {
    return TCL_OK;
  }
  bindAll(interp, objv);

  int nArgs = objv.size();
  
 
    try{
      if (nArgs < 2) {
       showCommands(interp, objv);            
      }  else {
        std::string subcommand = objv[1];    
        if (subcommand == "mk"){                // tested
          createPipeline(interp, objv);      
        } else if (subcommand == "ls"){         // tested
          listPipelines(interp, objv);      
        } else if (subcommand == "current"){    // tested
          listCurrentPipeline(interp, objv);
        } else if (subcommand == "ls-all"){     // tested
          listAll(interp, objv);
        } else if (subcommand == "ls-evp") {
          listEvp(interp,objv);
        } else if (subcommand == "use"){        // tested
          usePipeline(interp, objv);      	
        } else if (subcommand == "add"){        // tested     
          addProcessor(interp, objv);
        } else if (subcommand == "rm"){         // tested
          removeProcessor(interp, objv);
        } else if (subcommand == "clear"){      // Tested
          clearPipeline(interp, objv);            
        } else if (subcommand == "clone"){      // Tested.
          clonePipeline(interp, objv);
        } else if (subcommand == "rmevp") {
          rmEvp(interp, objv);
        } else {
          std::string msg = "Invalid subcommand: " ;
          msg += subcommand;
          throw std::string(msg);
        }
      }}
    catch (const std::string& msg) {
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
      interp.setResult("pman - unanticipated exception type"); 
      return TCL_ERROR;
    }
  return TCL_OK;
}

void
CPipelineCommandActual::showCommands(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  std::stringstream sCmds;
  sCmds << "pman mk pipe-name       : Create a new pipeline\n";
  sCmds << "pman ls ?pattern?       : Lists the pipeline names matching pattern (default is *)\n";
  sCmds << "pman current            : Lists name of and event processors in the current pipeline\n";
  sCmds << "pman ls-all ?pattern?   : List names and event processors in all pipeilnes\n";
  sCmds << "pman ls-evp ?pattern?   : Lists the names of registered event processors\n";
  sCmds << "pman use name           : Use the pipeline 'name' as the current pipeline\n";
  sCmds << "pman add pipename epname : Add the event processor 'evpname' to the end of \n";
  sCmds << "                        :  Pipeline 'pipename' \n";
  sCmds << "pman rm pipename epname : Remove the event processor 'evpname'  from the\n";
  sCmds << "                        : pipeline named 'pipename'\n";
  sCmds << "pman clear pipename     : Removes all event processors from 'pipename'\n";
  sCmds << "pman clone old new      : Clones the pipline named 'old' creating one \n";
  sCmds << "pman rmevp evpname      : Unregisters an event processor\n";
  sCmds << "                          named 'new'\n";
  
  
  throw sCmds.str();
}
/**
 * createPipeline
 *    Create a new pipeline
 *
 */
void
CPipelineCommandActual::createPipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 3, "Missing Pipeline Name");
  std::string pipeName = objv[2];
  
  SpecTcl::getInstance()->makePipeline(pipeName.c_str());
}

/**
 * listPipeline
 *    List the names of the pipelines that match the specified pattern
 *    '*' is used when no pattern is supplied.
 */
void
CPipelineCommandActual::listPipelines(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  std::string pattern = "*";            // Default.
  requireAtMost(objv, 3, "Too many command parameters");
  if (objv.size() == 3) {
    pattern = std::string(objv[2]);
  }
  std::vector<std::string> pipeNames =
    SpecTcl::getInstance()->listProcessingPipelines();
  
  // Transfer the ones that match the pattern to rersult:
  
  CTCLObject result;
  result.Bind(interp);
  
  for (int i = 0; i < pipeNames.size(); i++) {
    if (Tcl_StringMatch(pipeNames[i].c_str(), pattern.c_str())) {
      result += pipeNames[i];
    }
  }
  
  
  interp.setResult(result);
  
}
/**
 * listCurrentPipeline
 *    Lists the names of the event processors in the current pipeline.
 */
void
CPipelineCommandActual::listCurrentPipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 2, "Too many command parameters");
  CTCLObject result;
  CTCLObject evpList;
  result.Bind(interp);
  evpList.Bind(interp);
  std::string pipe = CPipelineManager::getInstance()->getCurrentPipelineName();
  listPipeline(evpList, pipe);
  result += pipe;
  result += evpList;
  interp.setResult(result);
}
/**
 * listAll
 *    Returns a list of the pipelines and their contents for the pipelines
 *    that match the current pipeline name.  The result is a list of two
 *    element sublists.  The first element of each sublist is the name of the
 *    pipeline. The second a list of the event processors in that list.
 */
void
CPipelineCommandActual::listAll(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  std::string pattern = "*";
  requireAtMost(objv, 3, "Too many command parameters");
  if (objv.size() == 3) {
    pattern = std::string(objv[2]);
  }
  
  SpecTcl* pApi = SpecTcl::getInstance();
  std::vector<std::string> pipeNames = pApi->listProcessingPipelines();
  
  CTCLObject result;
  result.Bind(interp);
  
  for (int i = 0; i < pipeNames.size(); i++) {
    if (Tcl_StringMatch(pipeNames[i].c_str(), pattern.c_str())) {
      CTCLObject name;
      CTCLObject processors;
      name.Bind(interp);
      processors.Bind(interp);
      
      name = pipeNames[i];
      listPipeline(processors, pipeNames[i]);
      
      CTCLObject item;
      item.Bind(interp);
      item += name;
      item += processors;
      result += item;
    }
  }
  interp.setResult(result);
}
/**
 * usePipeline
 *    Specify the current pipeline name.
 */
void
CPipelineCommandActual::usePipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 3, "Missing pipeline name");
  std::string pipeName = objv[2];
  
  SpecTcl::getInstance()->SetCurrentPipeline(pipeName);
}
/**
 * addProcessor
 *    Adds a processor to a pipeline.
 */
void
CPipelineCommandActual::addProcessor(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 4,"Need a pipe and event processor name");
  std::string pipe = objv[2];
  std::string evp  = objv[3];
  
  SpecTcl::getInstance()->appendEventProcessor(pipe.c_str(), evp.c_str());
}
/**
 * removeProcessor - remove a processor from a pipeline.
*/
void
CPipelineCommandActual::removeProcessor(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 4, "Need a pipe and event processor name");
  std::string pipe = objv[2];
  std::string evp  = objv[3];
  
  SpecTcl::getInstance()->RemoveEventProcessor(pipe.c_str(), evp.c_str());
}
/**
 * clearPipeline
 *    Remove all elements from the named pipeline.
 */
void
CPipelineCommandActual::clearPipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 3, "Need a pipeline name");
  std::string pipe = objv[2];
  
  SpecTcl::getInstance()->ClearPipeline(pipe);
}
/**
 * clonePipeline
 *     Create a copy of an existing pipeline.
 */
void
CPipelineCommandActual::clonePipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 4, "Need an old pipeline name and a new pipeline name");
  std::string oldName =  objv[2];
  std::string newName = objv[3];
  
  SpecTcl::getInstance()->clonePipeline(oldName.c_str(), newName.c_str());
}
/**
 * listEvp
 *    Lists the available event processors.
 */
void
CPipelineCommandActual::listEvp(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  std::string pattern="*";
  requireAtMost(objv, 3, "Too many parameters for listEvp");
  if (objv.size() ==3) {
    pattern = std::string(objv[2]);
  }
  
  std::vector<std::string> processors =
    CPipelineManager::getInstance()->getEventProcessorNames();
  CTCLObject result; result.Bind(interp);
  for (int i = 0; i < processors.size(); i++) {
    if (Tcl_StringMatch(processors[i].c_str(), pattern.c_str())) {
      result += processors[i];
    }
  }
  
  interp.setResult(result);
  
}
/**
 * rmEvp
 *    Unregister an event processor from the pipeline manager.
 *    The processor must first be removed from all piplines it belongs to.
 */
void
CPipelineCommandActual::rmEvp(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(
    objv, 3, "rmevp subcommand only requires an event processor name"
  );
  
  std::string evpname = objv[2];
  
  // Ensure there's no existing event processor. If there is,
  // that's an std::invalid_argument.
  //
  
  auto usedBy = CPipelineManager::getInstance()->pipelinesUsing(evpname.c_str());
  if (! usedBy.empty()) {
    // Processor is in use -- bad.
    
    std::stringstream msg;
    msg << "The event processor " << evpname << " is in use.\n";
    msg << "Before it can be unregistered it must e removed from the following pipelines:\n";
    for (int i =0; i < usedBy.size(); i++) {
      msg << usedBy[i] << std::endl;
    }
    throw std::invalid_argument(msg.str());
  }
  
  CPipelineManager::getInstance()->unregisterEventProcessor(evpname);
}
/*-------------------------------------------------------------------------------
 * Utility methods
 */

/**
 * listPipeline
 *    list the contents of a pipeline in to a TclObject as a list.
 *
 *  @param[out] list - reference a CTCLObject that's bound to an interp.
 *                     will get the list of pipeline elements
 *  @param[in] pipename - Name of the pipeline to list.
 */
void
CPipelineCommandActual::listPipeline(CTCLObject& list, std::string pipename)
{
  std::vector<std::string> evpNames =
    CPipelineManager::getInstance()->getEventProcessorsInPipeline(pipename);
    
  for (int i =0; i < evpNames.size(); i++) {
    list += evpNames[i];
  }
}


/// Construct the MPI wrapper.

CPipelineCommand::CPipelineCommand(CTCLInterpreter& rInterp) :
  CMPITclCommandAll(rInterp, "pman", new CPipelineCommandActual(rInterp)) {}