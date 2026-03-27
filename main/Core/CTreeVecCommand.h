/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2026
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
 * @file CTreeVecCommand.h
 * @brief Defines the "treeparamvec" command class to query about CTreeParameterVector objects.
 * @author Ron Fox <fox at frib dot msu dot edu>
 */

 #ifndef CTREEVECCOMMAND_H
 #define CTREEVECCOMMAND_H

 #include <TCLObjectProcessor.h>
 #include <MPITclCommandAll.h>     // Runs in all ranks.


class CTCLInterpreter;
class CTCLObject;
class CTreeParameterVector;

/**
 *  Provides the treeparamvec command.  This SpecTcl command provides information and
 * some manipulation of tree parameter vectors (CTreeParameterVector objects).
 * 
 * This is a command ensemble with the subcommands:
 * 
 * * -list ?pattern? - lists the properties of matching tree parameter vectors.
 * * -setlow name value - set the low limit of the named vector.
 * * -sethigh name value - set the high limit of the named vector.
 * * -setunits name units - sets the units of measure of the named vector.
 * 
 * -list is the only one I'll say more about.  The result is a list of dicts.
 * which describe tree parameter vectors that match the optional glob pattern (which
 * defaults to '*' matching everthing).  The dict has the following keys:
 * * name - name of the vector (used to generate actual parameter names).
 * * low  - low limit that is defined for all parameters.
 * * high - high limit that is defined for all paramters.
 * * units - units of measure that is defined for all parameters.
 * * parameters - list of actual tree parameters that have been created. example
 * 
 * {name sample low 0.0 high 4096.0 units "arb" parameters {sample(0) sample(1) sample(2)}}
 * 
 * 
 */
class CTreeVecActual : public CTCLObjectProcessor {
public:
    CTreeVecActual(CTCLInterpreter& rInterp);
    virtual ~CTreeVecActual();
    

    // auto generated canonicals are fine as we have no data but the base class data.

public:
    virtual int operator()(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);

    // These execute the subcommands:

protected:
    void list(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void setlow(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void sethigh(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);
    void setunits(CTCLInterpreter& interp, std::vector<CTCLObject>& objv);

private:
    CTCLObject* describeVector(CTCLInterpreter& interp, CTreeParameterVector& vec);

};

// MPI Wrapper:

class CTreeVecCommand : public CMPITclCommandAll {
public:
    CTreeVecCommand(CTCLInterpreter* pInterp);
    ~CTreeVecCommand() {}
};
 #endif