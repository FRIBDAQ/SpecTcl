/* Class CTreeVariable
	This class provides the means to declare variables in a tree-like fashion
	and bind them to the interpreter of SpecTcl.
*/

#ifndef __TREEVARIABLE_H
#define __TREEVARIABLE_H

#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>
#include <histotypes.h>
#include <TCLProcessor.h>

class CTreeVariable
{
private:
	string			name;
	double			value;
	string			unit;
public:
	static vector<CTreeVariable*>	pSelf;

public:
// Dummy Constructor
	CTreeVariable() {}

// Constructor
	CTreeVariable(string variableName, double variableValue, string variableUnit) :
		name(variableName), value(variableValue), unit(variableUnit) {
		// For now we just copy the pointer to a vector so we can later bind to the interpreter
		pSelf.push_back(this);
	}

// Destructor
	~CTreeVariable() {}

// Copy constructor
	CTreeVariable(const CTreeVariable& aCTreeVariable) {
		name = aCTreeVariable.name;
		value = aCTreeVariable.value;
		unit = aCTreeVariable.unit;
	}

// Operator double() to cast our variables to doubles in expressions
	operator double() {
		return (double)(value);
	}
	
// Operator=
	double operator=(const double& rhs) {
		value = rhs;
		return value;
	}
	
// Operator+=
	double operator+=(const double& rhs) {
		value += rhs;
		return value;
	}
 
// Operator-=
	double operator-=(const double& rhs) {
		value -= rhs;
		return value;
	}
 
// Operator*=
	double operator*=(const double& rhs) {
		value *= rhs;
		return value;
	}
 
// Operator/=
	double operator/=(const double& rhs) {
		value /= rhs;
		return value;
	}
	
// Operator++ (postfix)
	double operator++(int) {
		double data = (double)(*this);
		(*this) = data + 1;
		return data;
	}
 
// Operator++ (prefix)
	double operator++() {
		double data = (double)(*this);
		(*this) = data++;
		return data;
	}
 
// Operator-- (postfix)
	double operator--(int) {
		double data = (double)(*this);
		(*this) = data - 1;
		return data;
	}
 
// Operator-- (prefix)
	double operator--() {
		double data = (double)(*this);
		(*this) = data--;
		return data;
	}
 
// Access functions
	string		getName() {return name;}
	double		getValue(){return value;}
	char*		getValueAddr(){return (char*) &value;}
	string		getUnit() {return unit;}
	
// Binding of TCL variables
	static void BindVariables(CTCLInterpreter& rInterp) {
		vector<CTreeVariable*>::iterator s;
		
		for (s = pSelf.begin(); s != pSelf.end(); s++) {
			// Link TCL variable to the CTreeVariable
			Tcl_LinkVar(rInterp, (char*)((*s)->getName()).c_str(), (*s)->getValueAddr(), TCL_LINK_DOUBLE);
		}
//		pSelf.erase(pSelf.begin(), pSelf.end());
	}
};

///////////////////////////////////////////////////////////////////////////////////
/*	Class CTreeVariableArray
	This class allows to define arrays of variables.  The user needs to specify the size
	of the array and the index of the first element
*/

class CTreeVariableArray
{
private:
	vector<CTreeVariable*> array;
	UInt_t start, size;

public:
// Dummy Constructor
	CTreeVariableArray() {}

// Constructor
	CTreeVariableArray(string rootName, double rootValue, string rootUnit, UInt_t theSize, UInt_t theStart) :
	size(theSize), start(theStart) {
		int		i, ndigits;
		string	theName;
		char		str[10], form[20];
		CTreeVariable* pCTreeVariable;


		ndigits = int(log10((double)(size+start))+1);
		
		for (int i=0; i < size; i++) {
			sprintf(form, ".%%%d.%dd", ndigits, ndigits);
			sprintf(str, form, i+start);
			theName = rootName + str;
			pCTreeVariable = new CTreeVariable(theName, rootValue, rootUnit);
			array.push_back(pCTreeVariable);
		}
	}

// Destructor
	~CTreeVariableArray() {
		vector<CTreeVariable*>::iterator i;
		for (i = array.begin(); i != array.end(); i++) delete i;
	}
	
// Operator[].  Allows to access a member of the array using backets (i.e. s800.ic.de[11])
	CTreeVariable& operator[](int i) {
		return (CTreeVariable&)(*array[i-start]);
	}
};

///////////////////////////////////////////////////////////////////////////////////
/*	Class CTreeVariableCommand
	this class is derived from CTCLProcessor and implements the command treevariable
*/
class CTreeVariableCommand : public CTCLProcessor
{
public:
// Constructor
	CTreeVariableCommand(CTCLInterpreter* pInterp) :
		CTCLProcessor("treevariable", pInterp) {}
		
// Functionality
	int operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, int argc, char* argv[]) {
		vector<CTreeVariable*>::iterator s;
		char str[80];
	
		if (strcmp(argv[1], "-list") == 0) {
			for (s = CTreeVariable::pSelf.begin(); s != CTreeVariable::pSelf.end(); s++) {
				sprintf(str, "{%s %g %s}\n", ((*s)->getName()).c_str(), (*s)->getValue(), ((*s)->getUnit()).c_str());
				rResult += str;
			}
		}
		else {
			rResult = "Usage:\n   treevariable -list\n";
		}
		return TCL_OK;
	}
};

#endif
