/* Class CTreeParameter
	This class provides the means to declare parameters in a tree-like fashion
	and assigns the SpecTcl parameter id automatically, removing the burden to
	do it manually for the user
*/
// Version 1.2 memo:
// Now that SpecTcl has real valued parameters, the scaling part of the CTreeParameter class
// can be removed, and the value directly passed to the CParameter class.
// The limits of the parameters then become only indicative and don't any role other than
// providing "reasonable" default limits for spectrum definitions.
// The class member "max" is renamed "bins", which contains a default number of bins
// The class member "bits" is removed and "bins" is set appropriately when a number of bits is specified
// The following constructors were defined for version 1.1:
// CTreeParameter(string name, UInt_t bits);
// CTreeParameter(string name, UInt_t bits, double start, double other, string unit, bool choice);
// The following constructors are added for version 1.2
// CTreeParameter(string name);
// CTreeParameter(string name, string unit);
// CTreeParameter(string name, double start, double stop, string unit);
// CTreeParameter(string name, UInt_t bins, double start, double stop, string unit);
// Default values are taken for missing values:
// 	unit = "unknown" for version 1.2 constructors and "channels" for version 1.1 constructors
// 	bins = 100
// 	start = 1.0
// 	stop = 100.0

#ifndef __TREEPARAMETER_H
#define __TREEPARAMETER_H

// Version 1.2 takes advantage of the real parameters implemented in SpecTcl 2.1
// It is therefore only compatible with versions of SpecTcl >= 2.1
#define TreeParameterVersion "1.2"

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

extern CEventSink* gpEventSink;
//extern vector<CTreeParameter*> CTreeParameterpSelf;
//extern CEvent* CTreeParameterpEvent;

class CTreeParameter
{
private:
	string				name;
	double			value;
	bool				valid, haschanged;
	UInt_t			id, bins;
	double			start, stop, inc;
	string				unit;
	CParameter*	mpParameter;
public:
	static CEvent* 							pEvent;
	static vector<CTreeParameter*>	pSelf;

public:
// Constructors
	CTreeParameter() {}

// Constructor with only name specification (version 1.2)
	CTreeParameter(string parameterName) : name(parameterName) {
		valid = true;
		haschanged = false;
		start = 1;
		inc = 1;
		bins = 100;
		stop = 100;
		unit = "unknown";
		
		// For now we just copy the pointer to a vector so we can later bind to the histogrammer
		pSelf.push_back(this);
	}
	
// Constructor with name and unit specifications (version 1.2)
	CTreeParameter(string parameterName, string parameterUnit) :
	name(parameterName), unit(parameterUnit) {
		valid = true;
		haschanged = false;
		start = 1;
		inc = 1;
		bins = 100;
		stop = 100;
		
		// For now we just copy the pointer to a vector so we can later bind to the histogrammer
		pSelf.push_back(this);
	}

// Constructor with name, unit, start and stop specifications (version 1.2)
	CTreeParameter(string parameterName, double parameterStart, double parameterStop,
	string parameterUnit) :
	name(parameterName), start(parameterStart), stop(parameterStop), unit(parameterUnit) {
		valid = true;
		haschanged = false;
		bins = 100;
		inc = (stop - start) / (double)(bins - 1);

		// For now we just copy the pointer to a vector so we can later bind to the histogrammer
		pSelf.push_back(this);
	}

// Full-fledged constructor (version 1.2)
	CTreeParameter(string parameterName, UInt_t parameterBins, double parameterStart,
	double parameterStop, string parameterUnit) :
	name(parameterName), bins(parameterBins), start(parameterStart), stop(parameterStop),
	unit(parameterUnit) {
		valid = true;
		haschanged = false;
		inc = (stop - start) / (double)(bins - 1);

		// For now we just copy the pointer to a vector so we can later bind to the histogrammer
		pSelf.push_back(this);
	}

// Constructor with no scale specification (slope=1; offset=0) (version 1.1)
	CTreeParameter(string parameterName, UInt_t parameterBits) :
		name(parameterName) {
		valid = true;
		haschanged = false;
		start = 0;
		inc = 1;
		bins = (UInt_t) pow(2, parameterBits);
		stop = start + inc * (double)(bins - 1);
		unit = "channels";
		
		// For now we just copy the pointer to a vector so we can later bind to the histogrammer
		pSelf.push_back(this);
	}

// Full-fledged constructor (version 1.1)
	CTreeParameter(string parameterName, UInt_t parameterBits, double parameterStart,
	double parameterOther, string parameterUnit, bool slopeOrStop) :
	name(parameterName), start(parameterStart), unit(parameterUnit) {
		valid = true;
		haschanged = false;
		bins = (UInt_t) pow(2, parameterBits);
		if (slopeOrStop)	{		// Slope
			inc = parameterOther;
			stop = start + inc * (double)(bins - 1);
		} else {						// Stop
			stop = parameterOther;
			inc = (stop - start) / (double)(bins - 1);
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
		value = aCTreeParameter.value;
		haschanged = aCTreeParameter.haschanged;
		id = aCTreeParameter.id;
		bins = aCTreeParameter.bins;
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
// Version 1.2: this operation is no longer necessary since parameters are now real valued
	double operator=(const double& rhs) {
//		UInt_t data;
		value = rhs;
//		data = (UInt_t)((rhs - start) / inc);
//		if (data < 0) data = 0;
//		if (data > max) data = max;
		(*pEvent)[id] = value;
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
	string			getName() {return name;}
	UInt_t		getId() {return id;}
	void			setId(UInt_t theId) {id = theId;}
	UInt_t		getBins() {return bins;}
	void			setBins(UInt_t theBins) {bins = theBins; inc = (stop - start) / (double)(bins - 1);}
	double		getStart() {return start;}
	void			setStart(double theStart) {start = theStart; inc = (stop - start) / (double)(bins - 1);}
	double		getStop() {return stop;}
	void			setStop(double theStop) {stop = theStop; inc = (stop - start) / (double)(bins - 1);}
	double		getInc()	{return inc;}
	void			setInc(double theInc) {inc = theInc; stop = start + inc * (bins - 1);}
	string			getUnit() {return unit;}
	void			setUnit(char* theUnit) {unit = theUnit;}
	bool			isValid() {return valid;}
	void			setValid() {valid = true;}
	void			setInvalid() {valid = false; (*pEvent)[id].clear();}
	void			Reset() {(*pEvent)[id].clear();}
	bool			hasChanged() {return haschanged;}
	void			setChanged() {haschanged = true;}
	void			resetChanged() {haschanged = false;}
	
// Binding of histogrammer parameters
	static void BindParameters(CAnalyzer& rAnalyzer) {
	
// The following only worked for SpecTcl versions < 2.1
//		CEventSink* pSink = (CEventSink*)rAnalyzer.getSink();
//		CHistogrammer* mpHistogrammer = (CHistogrammer*)pSink;

// For version 2.1beta
		CHistogrammer* mpHistogrammer = (CHistogrammer*)gpEventSink;

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
		
			// register parameter with histogrammer (bypass the SpecTcl parameter command) (version 1.1)
//			(*s)->mpParameter = mpHistogrammer->AddParameter((*s)->getName(), nextid, (*s)->getBits());
			// register parameter with histogrammer (bypass the SpecTcl parameter command) (version 1.2)
			(*s)->mpParameter = mpHistogrammer->AddParameter(
				(*s)->getName(),
				nextid, (*s)->getUnit().c_str());
		}
//		pSelf.erase(pSelf.begin(), pSelf.end());
	}
	
// get parameter array from analyser
	static void setEvent(CEvent& rEvent) {
		pEvent = (CEvent*)(&rEvent);
	}

//Initialize functions (version 1.1)
void
Initialize(string parameterName, UInt_t parameterBits)
{
	name = parameterName;
	valid = true;
	haschanged = false;
	start = 0;
	inc = 1;
	bins = (UInt_t) pow(2, parameterBits);
	stop = start + inc * (double)(bins - 1);
	unit = "channels";
	pSelf.push_back(this);
}

void
Initialize(string parameterName, UInt_t parameterBits, double parameterStart,
double parameterOther, string parameterUnit, bool slopeOrStop)
{
	name = parameterName;
	start = parameterStart;
	unit = parameterUnit;
	valid = true;
	haschanged = false;
	bins = (UInt_t) pow(2, parameterBits);
	if (slopeOrStop)  {   // Slope
		inc = parameterOther;
		stop = start + inc * (double)(bins - 1);
	} else {            // Stop
		stop = parameterOther;
		inc = (stop - start) / (double)(bins - 1);
	}
	pSelf.push_back(this);
}

//Initialize functions (version 1.2)
void
Initialize(string parameterName)
{
	name = parameterName;
	valid = true;
	haschanged = false;
	start = 1;
	inc = 1;
	bins = 100;
	stop = 100;
	unit = "unknown";
	pSelf.push_back(this);
}
	
void
Initialize(string parameterName, string parameterUnit)
{
	name = parameterName;
	unit = parameterUnit;
	valid = true;
	haschanged = false;
	start = 1;
	inc = 1;
	bins = 100;
	stop = 100;
	pSelf.push_back(this);
}

void
Initialize(string parameterName, double parameterStart, double parameterStop, string parameterUnit)
{
	name = parameterName;
	start = parameterStart;
	stop = parameterStop;
	unit = parameterUnit;
	valid = true;
	haschanged = false;
	bins = 100;
	inc = (stop - start) / (double)(bins - 1);
	pSelf.push_back(this);
	}

void
Initialize(string parameterName, UInt_t parameterBins, double parameterStart,
double parameterStop, string parameterUnit)
{
	name = parameterName;
	bins = parameterBins;
	start = parameterStart;
	stop = parameterStop;
	unit = parameterUnit;
	valid = true;
	haschanged = false;
	inc = (stop - start) / (double)(bins - 1);
	pSelf.push_back(this);
}




void
ResetAll()
{
	vector<CTreeParameter*>::iterator s;
	for (s = pSelf.begin(); s != pSelf.end(); s++)
		(*s)->Reset();
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

// Constructor with no scale specification (slope=1; offset=0) (version 1.1)
	CTreeParameterArray(string rootName, UInt_t rootBits, UInt_t theSize, UInt_t theStart) :
	size(theSize), start(theStart) {
		int							i, ndigits;
		string						theName;
		char						str[10], form[20];
		CTreeParameter*		pCTreeParameter;

		ndigits = int(log10((double)(size+start))+1);
		
		for (int i=0; i < size; i++) {
			sprintf(form, ".%%%d.%dd", ndigits, ndigits);
			sprintf(str, form, i+start);
			theName = rootName + str;
			pCTreeParameter = new CTreeParameter(theName, rootBits);
			array.push_back(pCTreeParameter);
		}
	}

// Full-fledged constructor (version 1.1)
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

// Constructor with only name specification (version 1.2)
	CTreeParameterArray(string rootName, UInt_t theSize, UInt_t theStart) :
	size(theSize), start(theStart) {
		int							i, ndigits;
		string						theName;
		char						str[10], form[20];
		CTreeParameter*		pCTreeParameter;

		ndigits = int(log10((double)(size+start))+1);
		
		for (int i=0; i < size; i++) {
			sprintf(form, ".%%%d.%dd", ndigits, ndigits);
			sprintf(str, form, i+start);
			theName = rootName + str;
			pCTreeParameter = new CTreeParameter(theName);
			array.push_back(pCTreeParameter);
		}
	}

// Constructor with name and unit specifications (version 1.2)
	CTreeParameterArray(string rootName, string rootUnit, UInt_t theSize, UInt_t theStart) :
	size(theSize), start(theStart) {
		int							i, ndigits;
		string						theName;
		char						str[10], form[20];
		CTreeParameter*		pCTreeParameter;

		ndigits = int(log10((double)(size+start))+1);
		
		for (int i=0; i < size; i++) {
			sprintf(form, ".%%%d.%dd", ndigits, ndigits);
			sprintf(str, form, i+start);
			theName = rootName + str;
			pCTreeParameter = new CTreeParameter(theName, rootUnit);
			array.push_back(pCTreeParameter);
		}
	}

// Constructor with name, unit, start and stop specifications (version 1.2)
	CTreeParameterArray(string rootName, double rootStart, double rootStop, string rootUnit,
	UInt_t theSize, UInt_t theStart) :
	size(theSize), start(theStart) {
		int							i, ndigits;
		string						theName;
		char						str[10], form[20];
		CTreeParameter*		pCTreeParameter;

		ndigits = int(log10((double)(size+start))+1);
		
		for (int i=0; i < size; i++) {
			sprintf(form, ".%%%d.%dd", ndigits, ndigits);
			sprintf(str, form, i+start);
			theName = rootName + str;
			pCTreeParameter = new CTreeParameter(theName, rootStart, rootStop, rootUnit);
			array.push_back(pCTreeParameter);
		}
	}

// Full-fledged constructor (version 1.2)
	CTreeParameterArray(string rootName, UInt_t rootBins, double rootStart, double rootStop, string rootUnit,
	UInt_t theSize, UInt_t theStart) :
	size(theSize), start(theStart) {
		int							i, ndigits;
		string						theName;
		char						str[10], form[20];
		CTreeParameter*		pCTreeParameter;

		ndigits = int(log10((double)(size+start))+1);
		
		for (int i=0; i < size; i++) {
			sprintf(form, ".%%%d.%dd", ndigits, ndigits);
			sprintf(str, form, i+start);
			theName = rootName + str;
			pCTreeParameter = new CTreeParameter(theName, rootBins, rootStart, rootStop, rootUnit);
			array.push_back(pCTreeParameter);
		}
	}

	
// Destructor
	~CTreeParameterArray() {
		vector<CTreeParameter*>::iterator i;
		for (i = array.begin(); i != array.end(); i++) delete (*i);
	}
	
// Operator[].  Allows to access a member of the array using backets (i.e. s800.ic.de[11])
	CTreeParameter& operator[](int i) {
		return (CTreeParameter&)(*array[i-start]);
	}

// Reset function
void
Reset()
{
	vector<CTreeParameter*>::iterator s;
	for (s = array.begin(); s != array.end(); s++)
		(*s)->Reset();
}

// Initialize functions (version 1.1)
void
Initialize(string rootName, UInt_t rootBits, UInt_t theSize, UInt_t theStart)
{
	int	i, ndigits;
	string	theName;
	char		str[10], form[20];
	CTreeParameter* pCTreeParameter;

	size = theSize;
	start = theStart;
	ndigits = int(log10((double)(size+start))+1);
	
	for (int i=0; i < size; i++) {
		sprintf(form, ".%%%d.%dd", ndigits, ndigits);
		sprintf(str, form, i+start);
		theName = rootName + str;
		pCTreeParameter = new CTreeParameter(theName, rootBits);
		array.push_back(pCTreeParameter);
	}
}

void
Initialize(string rootName, UInt_t rootBits, double rootStart,
double rootOther, string rootUnit, bool slopeOrStop, UInt_t theSize,
UInt_t theStart)
{
	int	i, ndigits;
	string	theName;
	char		str[10], form[20];
	CTreeParameter* pCTreeParameter;

	size = theSize;
	start = theStart;
	ndigits = int(log10((double)(size+start))+1);
	
	for (int i=0; i < size; i++) {
		sprintf(form, ".%%%d.%dd", ndigits, ndigits);
		sprintf(str, form, i+start);
		theName = rootName + str;
		pCTreeParameter = new CTreeParameter(theName, rootBits, rootStart, rootOther, rootUnit, slopeOrStop);
		array.push_back(pCTreeParameter);
	}
}

// Initialize functions (version 1.2)
void
Initialize(string rootName, UInt_t theSize, UInt_t theStart)
{
	int	i, ndigits;
	string	theName;
	char		str[10], form[20];
	CTreeParameter* pCTreeParameter;

	size = theSize;
	start = theStart;
	ndigits = int(log10((double)(size+start))+1);
		
	for (int i=0; i < size; i++) {
		sprintf(form, ".%%%d.%dd", ndigits, ndigits);
		sprintf(str, form, i+start);
		theName = rootName + str;
		pCTreeParameter = new CTreeParameter(theName);
		array.push_back(pCTreeParameter);
	}
}

void
Initialize(string rootName, string rootUnit, UInt_t theSize, UInt_t theStart)
{
	int	i, ndigits;
	string	theName;
	char		str[10], form[20];
	CTreeParameter* pCTreeParameter;

	size = theSize;
	start = theStart;
	ndigits = int(log10((double)(size+start))+1);
		
	for (int i=0; i < size; i++) {
		sprintf(form, ".%%%d.%dd", ndigits, ndigits);
		sprintf(str, form, i+start);
		theName = rootName + str;
		pCTreeParameter = new CTreeParameter(theName, rootUnit);
		array.push_back(pCTreeParameter);
	}
}

void
Initialize(string rootName, double rootStart, double rootStop, string rootUnit, UInt_t theSize, UInt_t theStart)
{
	int	i, ndigits;
	string	theName;
	char		str[10], form[20];
	CTreeParameter* pCTreeParameter;

	size = theSize;
	start = theStart;
	ndigits = int(log10((double)(size+start))+1);
		
	for (int i=0; i < size; i++) {
		sprintf(form, ".%%%d.%dd", ndigits, ndigits);
		sprintf(str, form, i+start);
		theName = rootName + str;
		pCTreeParameter = new CTreeParameter(theName, rootStart, rootStop, rootUnit);
		array.push_back(pCTreeParameter);
	}
}

void
Initialize(string rootName, UInt_t rootBins, double rootStart, double rootStop, string rootUnit, UInt_t theSize, UInt_t theStart)
{
	int	i, ndigits;
	string	theName;
	char		str[10], form[20];
	CTreeParameter* pCTreeParameter;

	size = theSize;
	start = theStart;
	ndigits = int(log10((double)(size+start))+1);
		
	for (int i=0; i < size; i++) {
		sprintf(form, ".%%%d.%dd", ndigits, ndigits);
		sprintf(str, form, i+start);
		theName = rootName + str;
		pCTreeParameter = new CTreeParameter(theName, rootBins, rootStart, rootStop, rootUnit);
		array.push_back(pCTreeParameter);
	}
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
				sprintf(str, "{%s %d %g %g %g %s}\n", ((*s)->getName()).c_str(), (*s)->getBins(),
				(*s)->getStart(), (*s)->getStop(), (*s)->getInc(), ((*s)->getUnit()).c_str());
				rResult += str;
			}
		}

		else if (argc == 3 && strcmp(argv[1], "-list") == 0) {
			for (s = CTreeParameter::pSelf.begin(); s != CTreeParameter::pSelf.end(); s++) {
				if (strcmp(((*s)->getName()).c_str(), argv[2]) == 0) {
					sprintf(str, "{%s %d %g %g %g %s}\n", ((*s)->getName()).c_str(), (*s)->getBins(),
					(*s)->getStart(), (*s)->getStop(), (*s)->getInc(), ((*s)->getUnit()).c_str());
					rResult += str;
					break;
				}
			}
		}

		else if (argc == 8 && strcmp(argv[1], "-set") == 0) {
			for (s = CTreeParameter::pSelf.begin(); s != CTreeParameter::pSelf.end(); s++) {
				if (strcmp(((*s)->getName()).c_str(), argv[2]) == 0) {
					(*s)->setBins(atoi(argv[3]));
					(*s)->setStart(atof(argv[4]));
					(*s)->setStop(atof(argv[5]));
					(*s)->setInc(atof(argv[6]));
					(*s)->setUnit(argv[7]);
					(*s)->setChanged();
					break;
				}
			}
		}

		else if (argc == 4 && strcmp(argv[1], "-setinc") == 0) {
			for (s = CTreeParameter::pSelf.begin(); s != CTreeParameter::pSelf.end(); s++) {
				if (strcmp(((*s)->getName()).c_str(), argv[2]) == 0) {
					(*s)->setInc(atof(argv[3]));
					(*s)->setChanged();
					break;
				}
			}
		}

		else if (argc == 4 && strcmp(argv[1], "-setbins") == 0) {
			for (s = CTreeParameter::pSelf.begin(); s != CTreeParameter::pSelf.end(); s++) {
				if (strcmp(((*s)->getName()).c_str(), argv[2]) == 0) {
					(*s)->setBins(atoi(argv[3]));
					(*s)->setChanged();
					break;
				}
			}
		}

		else if (argc == 4 && strcmp(argv[1], "-setunit") == 0) {
			for (s = CTreeParameter::pSelf.begin(); s != CTreeParameter::pSelf.end(); s++) {
				if (strcmp(((*s)->getName()).c_str(), argv[2]) == 0) {
					(*s)->setUnit(argv[3]);
					(*s)->setChanged();
					break;
				}
			}
		}

		else if (argc == 5 && strcmp(argv[1], "-setlimits") == 0) {
			for (s = CTreeParameter::pSelf.begin(); s != CTreeParameter::pSelf.end(); s++) {
				if (strcmp(((*s)->getName()).c_str(), argv[2]) == 0) {
					(*s)->setStart(atof(argv[3]));
					(*s)->setStop(atof(argv[4]));
					(*s)->setChanged();
					break;
				}
			}
		}

		else if (argc == 3 && strcmp(argv[1], "-check") == 0) {
			for (s = CTreeParameter::pSelf.begin(); s != CTreeParameter::pSelf.end(); s++) {
				if (strcmp(((*s)->getName()).c_str(), argv[2]) == 0) {
					if ((*s)->hasChanged()) rResult = "1";
					else rResult = "0";
					break;
				}
			}
		}

		else if (argc == 3 && strcmp(argv[1], "-uncheck") == 0) {
			for (s = CTreeParameter::pSelf.begin(); s != CTreeParameter::pSelf.end(); s++) {
				if (strcmp(((*s)->getName()).c_str(), argv[2]) == 0) {
					(*s)->resetChanged();
				}
			}
		}
		else if (argc == 2 && strcmp(argv[1], "-version") == 0) {
			rResult = TreeParameterVersion;
		}

		else {
			rResult = "\
Usage:\n\
   treeparameter -list\n\
   treeparameter -list name\n\
   treeparameter -set name bins low high inc unit\n\
   treeparameter -setinc name inc\n\
   treeparameter -setbins name bins\n\
   treeparameter -setunit name unit\n\
   treeparameter -setlimits name low high\n\
   treeparameter -check name\n\
   treeparameter -uncheck name\n\
   treeparameter -version\n\
";
		}
		return TCL_OK;
	}

};

#endif
