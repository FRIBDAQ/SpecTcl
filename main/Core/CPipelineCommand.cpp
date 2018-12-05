#include "CPipelineCommand.h"
#include <TCLInterpreter.h>
#include <TCLObject.h>
#include <sstream>
#include <stdexcept>
#include <Exception.h>
#include <tcl.h>
#include <SpecTcl.h>
#include <CPipelineManager.h>

CPipelineCommand::CPipelineCommand(CTCLInterpreter& interp) :
  CTCLObjectProcessor(interp, "pman" , true),
  m_manager(CPipelineManager::getInstance())
{

}

int
CPipelineCommand::operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
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
        } else {
          std::string msg = "Invalid subcommand: " ;
          msg += subcommand;
          throw std::string(msg);
        }
      }
    }
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
CPipelineCommand::showCommands(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  std::stringstream sCmds;
  sCmds << "pman mk pipe-name       : Create a new pipeline\n";
  sCmds << "pman ls ?pattern?       : Lists the pipeline names matching pattern (default is *)\n";
  sCmds << "pman current            : Lists name of and event processors in the current pipeline\n";
  sCmds << "pman ls-all ?pattern?   : List names and event processors in all pipeilnes\n";
  sCmds << "pman use name           : Use the pipeline 'name' as the current pipeline\n";
  sCmds << "pman add pipename epname : Add the event processor 'evpname' to the end of \n";
  sCmds << "                        :  Pipeline 'pipename' \n";
  sCmds << "pman rm pipename epname : Remove the event processor 'evpname'  from the\n";
  sCmds << "                        : pipeline named 'pipename'\n";
  sCmds << "pman clear pipename     : Removes all event processors from 'pipename'\n";
  sCmds << "pman clone old new      : Clones the pipline named 'old' creating one \n";
  sCmds << "                          named 'new'\n";
  
  
  throw sCmds.str();
}
/**
 * createPipeline
 *    Create a new pipeline
 *
 */
void
CPipelineCommand::createPipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
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
CPipelineCommand::listPipelines(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
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
CPipelineCommand::listCurrentPipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
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
CPipelineCommand::listAll(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
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
CPipelineCommand::usePipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
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
CPipelineCommand::addProcessor(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
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
CPipelineCommand::removeProcessor(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
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
CPipelineCommand::clearPipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
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
CPipelineCommand::clonePipeline(CTCLInterpreter& interp, std::vector<CTCLObject>& objv)
{
  requireExactly(objv, 4, "Need an old pipeline name and a new pipeline name");
  std::string oldName =  objv[2];
  std::string newName = objv[3];
  
  SpecTcl::getInstance()->clonePipeline(oldName.c_str(), newName.c_str());
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
CPipelineCommand::listPipeline(CTCLObject& list, std::string pipename)
{
  std::vector<std::string> evpNames =
    CPipelineManager::getInstance()->getEventProcessorsInPipeline(pipename);
    
  for (int i =0; i < evpNames.size(); i++) {
    list += evpNames[i];
  }
}