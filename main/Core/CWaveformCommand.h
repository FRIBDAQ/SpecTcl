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
#ifndef CWAVEFORMCOMMAND_H
#define CWAVEFORMCOMMAND_H
/**
 * @file CWaveformCommand.h
 * @brief Header for the waveform command execution class.
 */
#include <TCLObjectProcessor.h>
#include <MPITclCommandAll.h>
class CTCLInterpreter;
class CTCLObject;
class CWaveform;
/*
 *  CWaveformCommand
 *    Provides the serial version of the waveform command.  The form of the command is a
 * command ensemble:
 * 
 * \verbatim
 * waveform ?create? name samples
 * waveform list ?pattern?
 * waveform get name...
 * waveform getall name...
 * waveform metadata set wfname name value ...
 * waveform metadata get wfname ?name?
 * waveform resize name samples
 * waveform fits name ?pattern?; #212
 * \endverbatim
 * 
 * Note it is not supported to delete waveforms.
 * 
 * Note that ```waveform channel 100``` is the same as ```waveform create channel 100```
 * as with most spectcl commands that manage analysis objects.
 * 
 * The comments in the individual execution methods will describe each subcommand
 * more fully.
 * 
 * The programmer guide and reference will describe how to use waveform objects in the
 * user's analysis code more completely.  The ReST interface description will also
 * describe the set of requests that provide support for waveforms.
 */
class CWaveformCommand : public CTCLObjectProcessor {
public:
    CWaveformCommand(CTCLInterpreter& interp);
    virtual ~CWaveformCommand();

private:
    CWaveformCommand(const CWaveformCommand&);
    CWaveformCommand& operator=(const CWaveformCommand&);
    int operator==(const CWaveformCommand&);
    int operator!=(const CWaveformCommand&);

public:
    int operator()(CTCLInterpreter& interp,  std::vector<CTCLObject>& objv);

    // subcommand execution - note there are three for metadata:

protected:
    void create(CTCLInterpreter& interp,  std::vector<CTCLObject>& objv, unsigned nameIdx);
    void list(CTCLInterpreter& interp,  std::vector<CTCLObject>& objv);
    void get(CTCLInterpreter& interp,  std::vector<CTCLObject>& objv);
    void getAll(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void metadata(CTCLInterpreter& interp,  std::vector<CTCLObject>& objv);
    void metadataSet(CTCLInterpreter& interp,  std::vector<CTCLObject>& objv);
    void metadataGet(CTCLInterpreter& interp,  std::vector<CTCLObject>& objv);
    void resize(CTCLInterpreter& interp,  std::vector<CTCLObject>& objv);
    void listFits(CTCLInterpreter& interp,  std::vector<CTCLObject>& objv); // Issue #212

    // Utilities
private:
    
    CWaveform* find(const char* name);      //  hides all the API junk.
    void save(const CWaveform& wf);         //  Saves a constructed waveform.
    void listWaveform(CTCLObject& result, const CWaveform& wf);
    void getWaveform(CTCLObject& result, const CWaveform& wf);
    void getFits(CTCLObject& result, const CWaveform& wf, const char* pattern = "*");
};

// MPI wrapper:

class CWaveformCommandWrapper : public CMPITclCommandAll {
public:
    CWaveformCommandWrapper(CTCLInterpreter& interp, const char* command = "waveform");
    ~CWaveformCommandWrapper() {}
};
#endif