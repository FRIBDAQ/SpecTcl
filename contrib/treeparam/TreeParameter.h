/* Class CTreeParameter
	This class provides the means to declare parameters in a tree-like fashion
	and assigns the SpecTcl parameter id automatically, removing the burden to
	do it manually for the user
*/

#ifndef __TREEPARAMETER_H
#define __TREEPARAMETER_H

#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>
#include <Analyzer.h>
#include <EventSink.h>
#include <Histogrammer.h>
#include <histotypes.h>
#include <Parameter.h>
#include <TCLProcessor.h>

class CTreeParameter
{
private:
	string			name;
	double			value;
	bool				valid;
	UInt_t			id, bits, max;
	double			start, stop, inc;
	string			unit;
	CParameter*	mpParameter;
public:
	static CEvent* 				pEvent;
	static vector<CTreeParameter*>	pSelf;

public:
// Constructors
	CTreeParameter() {}

// Constructor with no scale specification (slope=1; offset=0)
	CTreeParameter(string parameterName, UInt_t parameterBits) :
		name(parameterName), bits(parameterBits) {
		valid = true;
		start = 0;
		inc = 1;
		max = (UInt_t) (pow(2, bits) - 1);
		stop = start + inc * (pow(2, bits) - 1);
		unit = "ch";
		
		// For now we just copy the pointer to a vector so we can later bind to the histogrammer
		pSelf.push_back(this);
	}

// Full-fledged constructor
	CTreeParameter(string parameterName, UInt_t parameterBits, double parameterStart,
	double parameterOther, string parameterUnit, bool slopeOrStop) :
	name(parameterName), bits(parameterBits), start(parameterStart), unit(parameterUnit) {
		valid = true;
		max = (UInt_t) (pow(2, bits) - 1);
		if (slopeOrStop)	{		// Slope
			inc = parameterOther;
			stop = start + inc * (pow(2, bits) - 1);
		} else {						// Stop
			stop = parameterOther;
			inc = (stop - start) / (pow(2, bits) - 1);
		}

		// For now we just copy the pointer to a vector so we can later bind to the histogrammer
		pSelf.push_back(this);
	}

// Destructor
	~CTreeParameter() {}

// Copy constructor
	CTreeParameter(const CTreeParameter& aCTreeParameter) {
		name = aCTreeParameter.name;
		valid = aCTreeParameter.valid;
		id = aCTreeParameter.id;
		bits = aCTreeParameter.bits;
		start = aCTreeParameter.start;
		stop = aCTreeParameter.stop;
		inc = aCTreeParameter.inc;
		unit = aCTreeParameter.unit;
		mpParameter = aCTreeParameter.mpParameter;
	}

// Operator double() to cast our parameters to doubles in expressions
	operator double() {
		return value;
//		return (double)(start + inc * (*pEvent)[id]);
	}
	
// Operator= assignement operator to a double: this actually stuffs the rEvent array
// with the correct id, so instead of rEvent[12] = myint, one writes for instance
// s800.crdc1.xfit = mydouble and doesn't need to know that s800.crdc1.xfit is parameter 12!
// Also, that way I don't need to do
// this painful and repetitive conversion from real units to channels, once I have provided the slope
// and offset on the parameter.
	double operator=(const double& rhs) {
		Int_t data;
		value = rhs;
		data = (Int_t)((rhs - start) / inc);
		if (data < 0) data = 0;
		if (data > max) data = max;
		(*pEvent)[id] = data;
		return (double)rhs;
	}
	
// Operator+=
	double operator+=(const double& rhs) {
		double data = (double)(*this);
		data += rhs;
		(*this) = data;
		return data;
	}
 
// Operator-=
	double operator-=(const double& rhs) {
		double data = (double)(*this);
		data -= rhs;
		(*this) = data;
		return data;
	}
 
// Operator*=
	double operator*=(const double& rhs) {
		double data = (double)(*this);
		data *= rhs;
		(*this) = data;
		return data;
	}
 
// Operator/=
	double operator/=(const double& rhs) {
		double data = (double)(*this);
		data /= rhs;
		(*this) = data;
		return data;
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
	UInt_t		getId() {return id;}
	void			setId(UInt_t theId) {id = theId;}
	UInt_t		getBits() {return bits;}
	double		getStart() {return start;}
	void			setStart(double theStart) {start = theStart;}
	double		getStop() {return stop;}
	void			setStop(double theStop) {stop = theStop;}
	double		getInc()	{return inc;}
	void			setInc(double theInc) {inc = theInc;}
	string		getUnit() {return unit;}
	void			setUnit(char* theUnit) {unit = theUnit;}
	
// Binding of histogrammer parameters
	static void BindParameters(CAnalyzer& rAnalyzer) {
	
		CEventSink* pSink = (CEventSink*)rAnalyzer.getSink();
		CHistogrammer* mpHistogrammer = (CHistogrammer*)pSink;

		vector<CTreeParameter*>::iterator s;
		ParameterDictionaryIterator i;
		vector<UInt_t> vid;
		vector<UInt_t>::iterator viditer;
		UInt_t nextid;

		for (s = pSelf.begin(); s != pSelf.end(); s++) {
			// get next available parameter id and assign it to this parameter
			for (i = mpHistogrammer->ParameterBegin(); i != mpHistogrammer->ParameterEnd(); i++) {
				vid.push_back((*i).second.getNumber());
			}
			sort(vid.begin(), vid.end());
			for (viditer = vid.begin(), nextid = 0; viditer != vid.end(); viditer++) {
				if ((*viditer) != nextid) break;
				nextid++;
			}
			vid.erase(vid.begin(), vid.end());
			(*s)->setId(nextid);
		
			// register parameter with histogrammer (bypass the SpecTcl parameter command)
			(*s)->mpParameter = mpHistogrammer->AddParameter((*s)->getName(), nextid, (*s)->getBits());
		}
//		pSelf.erase(pSelf.begin(), pSelf.end());
	}
	
// get parameter array from analyser
	static void setEvent(CEvent& rEvent) {
		pEvent = (CEvent*)(&rEvent);
	}
	
};

///////////////////////////////////////////////////////////////////////////////////
/*	Class CTreeParameterArray
	This class allows to define arrays of parameters.  The user needs to specify the size
	of the array and the index of the first element
*/

class CTreeParameterArray
{
private:
	vector<CTreeParameter*> array;
	UInt_t start, size;

public:
// Constructors
	CTreeParameterArray() {}

// Constructor with no scale specification (slope=1; offset=0)
	CTreeParameterArray(string rootName, UInt_t rootBits, UInt_t theSize, UInt_t theStart) :
	size(theSize), start(theStart) {
		int		i, ndigits;
		string	theName;
		char		str[10], form[20];
		CTreeParameter* pCTreeParameter;

		ndigits = int(log10((double)(size+start))+1);
		
		for (int i=0; i < size; i++) {
			sprintf(form, ".%%%d.%dd", ndigits, ndigits);
			sprintf(str, form, i+start);
			theName = rootName + str;
			pCTreeParameter = new CTreeParameter(theName, rootBits);
			array.push_back(pCTreeParameter);
		}
	}

// Full-fledged constructor
	CTreeParameterArray(string rootName, UInt_t rootBits, double rootStart, double rootOther,
	string rootUnit, bool slopeOrStop, UInt_t theSize, UInt_t theStart) :
	size(theSize), start(theStart) {
		int		i, ndigits;
		string	theName;
		char		str[10], form[20];
		CTreeParameter* pCTreeParameter;

		ndigits = int(log10((double)(size+start))+1);
		
		for (int i=0; i < size; i++) {
			sprintf(form, ".%%%d.%dd", ndigits, ndigits);
			sprintf(str, form, i+start);
			theName = rootName + str;
			pCTreeParameter = new CTreeParameter(theName, rootBits, rootStart, rootOther, rootUnit, slopeOrStop);
			array.push_back(pCTreeParameter);
		}
	}		
	
// Destructor
	~CTreeParameterArray() {
		vector<CTreeParameter*>::iterator i;
		for (i = array.begin(); i != array.end(); i++) delete i;
	}
	
// Operator[].  Allows to access a member of the array using backets (i.e. s800.ic.de[11])
	CTreeParameter& operator[](int i) {
		return (CTreeParameter&)(*array[i-start]);
	}
};

///////////////////////////////////////////////////////////////////////////////////
/*	Class CTreeParameterCommand
	this class is derived from CTCLProcessor and implements the command treeparameter
*/
class CTreeParameterCommand : public CTCLProcessor
{
public:
// Constructor
	CTreeParameterCommand(CTCLInterpreter* pInterp) :
		CTCLProcessor("treeparameter", pInterp) {}
		
// Functionality
	int operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, int argc, char* argv[]) {
		vector<CTreeParameter*>::iterator s;
		char str[200];
		
		if (argc == 2 && strcmp(argv[1], "-list") == 0) {
			for (s = CTreeParameter::pSelf.begin(); s != CTreeParameter::pSelf.end(); s++) {
				sprintf(str, "{%s %d %g %g %g %s}\n", ((*s)->getName()).c_str(), (*s)->getBits(),
				(*s)->getStart(), (*s)->getStop(), (*s)->getInc(), ((*s)->getUnit()).c_str());
				rResult += str;
			}
		}
		else if (argc == 7 && strcmp(argv[1], "-set") == 0) {
			for (s = CTreeParameter::pSelf.begin(); s != CTreeParameter::pSelf.end(); s++) {
				if (strcmp(((*s)->getName()).c_str(), argv[2]) == 0) {
					(*s)->setStart(atof(argv[3]));
					(*s)->setStop(atof(argv[4]));
					(*s)->setInc(atof(argv[5]));
					(*s)->setUnit(argv[6]);
				}
			}
		}
		else {
			rResult = "Usage:\n  treeparameter -list\n  treeparameter -set name start stop inc unit\n";
		}
		return TCL_OK;
	}

};

#endif
