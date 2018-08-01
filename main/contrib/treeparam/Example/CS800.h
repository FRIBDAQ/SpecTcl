#ifndef CS800_H
#define CS800_H

// The next include statements assume that the -I$(INSTDIR)/Include switch is in the Makefile
#include <../contrib/treeparam/TreeParameter.h>
#include <../contrib/treeparam/TreeVariable.h>

#define CS800_FP_CRDC_CHANNELS 224
#define CS800_FP_CRDC_SAMPLES 512
#define CS800_FP_CRDC_MAX_WIDTH 32
#define CS800_FP_IC_CHANNELS 16
#define CS800_TRACK_PARAMETERS 6
#define CS800_TRACK_COEFFICIENTS 200

extern class CS800 s800;

// crdc raw parameters
class CS800FpCrdcPads
{
	private:
		CS800* top;
	public:
		CTreeParameterArray raw;
		CTreeVariableArray ped;
		CTreeVariableArray slope;
		CTreeVariableArray offset;
		CTreeParameterArray cal;
	public:
		void Initialize(CS800* top, string name);
		void Integrate(unsigned short data[CS800_FP_CRDC_CHANNELS][CS800_FP_CRDC_MAX_WIDTH],\
			unsigned short width);
		void Calibrate();
};

class CS800FpCrdcDrift
{
	private:
		CS800* top;
	public:
		CTreeParameter raw;
		CTreeVariable slope;
		CTreeVariable offset;
		CTreeParameter cal;
		CTreeParameter hits;
	public:
		void Initialize(CS800* top, string name);
		void Calibrate();
};

// crdc calculated parameters
class CS800FpCrdcCalc
{
	private:
		CS800* top;
	public:
		CTreeParameter padmax;
		CTreeParameter padsum;
		CTreeParameter x_gravity;
		CTreeParameter x_fit;
		CTreeParameter x_chi2;
		CTreeVariable x_width;
		CTreeVariable x_method;
		CS800FpCrdcPads *m_pad;
	public:
		void Initialize(CS800* top, string name);
		void CalculateGravity();
		void CalculateFit();
};

// crdc
class CS800FpCrdc
{
	private:
		CS800* top;
		Bool_t hasdata;
		string m_name;
		unsigned short m_data[CS800_FP_CRDC_CHANNELS][CS800_FP_CRDC_MAX_WIDTH];
		unsigned short m_threshold;
		unsigned short m_sampleBegin;
		unsigned short m_sampleWidth;
	public:
		CS800FpCrdcPads pad;
		CS800FpCrdcDrift drift;
		CS800FpCrdcCalc calc;
		CTreeParameter x;
		CTreeParameter y;
		CTreeVariable x_offset;
		CTreeVariable x_slope;
		CTreeVariable y_offset;
		CTreeVariable y_slope;
		CTreeVariable padsnap;
	public:
		void Initialize(CS800* top, string name);
		Bool_t HasData() {return hasdata;}
		void Reset() {hasdata = kfFALSE;}
		TranslatorPointer<UShort_t> Unpack(TranslatorPointer<UShort_t> pData);
		TranslatorPointer<UShort_t> UnpackSamples(TranslatorPointer<UShort_t> pData);
		TranslatorPointer<UShort_t> UnpackPads(TranslatorPointer<UShort_t> pData);
		void CalculatePosition();
		void FillSnapshotSpectra(CAnalyzer& rAnalyzer);
};

// Ion Chamber
class CS800FpIonChamber
{
	private:
		CS800* top;
		Bool_t hasdata;
	public:
		CTreeParameterArray raw;
		CTreeVariableArray slope;
		CTreeVariableArray offset;
		CTreeParameterArray cal;
		CTreeParameter sum;
		CTreeParameter de;
		CTreeVariable de_slope;
		CTreeVariable de_offset;
	public:
		void Initialize(CS800* top, string name);
		Bool_t HasData() {return hasdata;}
		void Reset() {hasdata = kfFALSE;}
		TranslatorPointer<UShort_t> Unpack(TranslatorPointer<UShort_t> pData);
		void Calibrate();
		void Calculate();
};

// Scintillator
class CS800FpScintillator
{
	private:
		CS800* top;
		Bool_t hasdata;
	public:
		CTreeParameter de_up;
		CTreeParameter de_down;
		CTreeParameter time_up;
		CTreeParameter time_down;
		CTreeParameter de;
		CTreeParameter time;
	public:
		void Initialize(CS800* top, string name);
		Bool_t HasData() {return hasdata;}
		void Reset() {hasdata = kfFALSE;}
		TranslatorPointer<UShort_t> Unpack(TranslatorPointer<UShort_t> pData);
		void Calculate();
};

// Map
class CS800Map
{
	private:
		CS800* top;
	public:
		UShort_t maxcoefficient[CS800_TRACK_PARAMETERS];
		UShort_t order[CS800_TRACK_PARAMETERS][CS800_TRACK_COEFFICIENTS];
		UShort_t exponent[CS800_TRACK_PARAMETERS][CS800_TRACK_PARAMETERS][CS800_TRACK_COEFFICIENTS];
		double coefficient[CS800_TRACK_PARAMETERS][CS800_TRACK_COEFFICIENTS];
	public:
		CTreeVariable maxcoefficients;
		CTreeVariable maxparameters;
		CTreeVariable maxorder;
	public:
		void Initialize(CS800* top, string name);
		Bool_t WasLoaded() {if (maxorder == 0) return kfFALSE; else return kfTRUE;}
		double Calculate(int calcorder, int parameter, double *input);
};

///////////////////////////////////////////////////////////////////////////////////
/*	Class CS800MapCommand
	this class is derived from CTCLProcessor and implements the command s800map
*/
class CS800MapCommand : public CTCLProcessor
{
public:
	CS800Map *theMap;
// Constructor
	CS800MapCommand(CTCLInterpreter* pInterp, CS800Map *aMap) :
		CTCLProcessor("s800map", pInterp) {theMap = aMap;}
		
// Functionality
	int operator()(CTCLInterpreter& rInterp, CTCLResult& rResult, int argc, char* argv[]) {
		char str[80];
		int p, o, c;
	
		if (argc == 3 && strcmp(argv[1], "-get") == 0) {
			p = strtol(argv[2], 0, 10);
			if (p >= CS800_TRACK_PARAMETERS) {
				sprintf(str, "Invalid parameter number - must be between 0 and %d\n", CS800_TRACK_PARAMETERS-1);
				rResult += str;
				return TCL_OK;
			}
			for (int i=0; i<theMap->maxcoefficient[p]; i++) {
				sprintf(str, "{%d %g %d {%d %d %d %d %d %d}}\n", i, theMap->coefficient[p][i], theMap->order[p][i],\
				theMap->exponent[p][0][i], theMap->exponent[p][1][i], theMap->exponent[p][2][i], theMap->exponent[p][3][i], theMap->exponent[p][4][i],\
				theMap->exponent[p][5][i]);
				rResult += str;
			}
		}

		else if (argc == 4 && strcmp(argv[1], "-get") == 0) {
			p = strtol(argv[2], 0, 10);
			o = strtol(argv[3], 0, 10);
			if (p >= CS800_TRACK_PARAMETERS) {
				sprintf(str, "Invalid parameter number - must be between 0 and %d\n", CS800_TRACK_PARAMETERS-1);
				rResult += str;
				return TCL_OK;
			}
			for (int i=0; i<theMap->maxcoefficient[p]; i++) {
				if (theMap->order[p][i] == o) {
					sprintf(str, "{%d %g %d {%d %d %d %d %d %d}}\n", i, theMap->coefficient[p][i], theMap->order[p][i],\
					theMap->exponent[p][0][i], theMap->exponent[p][1][i], theMap->exponent[p][2][i], theMap->exponent[p][3][i], theMap->exponent[p][4][i],\
					theMap->exponent[p][5][i]);
					rResult += str;
				}
			}
		}

		else if (argc == 12 && strcmp(argv[1], "-set") == 0) {
			p = strtol(argv[2], 0, 10);
			o = strtol(argv[3], 0, 10);
			c = strtol(argv[4], 0, 10);
			if (p >= CS800_TRACK_PARAMETERS) {
				sprintf(str, "Invalid parameter number - must be between 0 and %d\n", CS800_TRACK_PARAMETERS-1);
				rResult += str;
				return TCL_OK;
			}
			if (c >= CS800_TRACK_COEFFICIENTS) {
				sprintf(str, "Too many coefficients - maximum is %d\n", CS800_TRACK_COEFFICIENTS);
				rResult += str;
				return TCL_OK;
			}
			if (c >= theMap->maxcoefficient[p]) theMap->maxcoefficient[p] = c+1;
			if (o > theMap->maxorder) theMap->maxorder = o;
			theMap->order[p][c] = o;
			theMap->exponent[p][0][c] = strtol(argv[5], 0, 10);
			theMap->exponent[p][1][c] = strtol(argv[6], 0, 10);
			theMap->exponent[p][2][c] = strtol(argv[7], 0, 10);
			theMap->exponent[p][3][c] = strtol(argv[8], 0, 10);
			theMap->exponent[p][4][c] = strtol(argv[9], 0, 10);
			theMap->exponent[p][5][c] = strtol(argv[10], 0, 10);
			theMap->coefficient[p][c] = strtod(argv[11], 0);
		}

		else {
			rResult = "\
Usage:\n\
   s800map -get parameter\n\
   s800map -get parameter order\n\
   s800map -set parameter order coefficient exp0 exp1 exp2 exp3 exp4 exp5 value\n\
";
		}
		return TCL_OK;
	}
};

// Tracking
class CS800FpTrack
{
	private:
		CS800* top;
	public:
		CTreeParameter xfp;
		CTreeParameter afp;
		CTreeParameter yfp;
		CTreeParameter bfp;
		CTreeVariable zfp;
		CTreeParameter ata;
		CTreeParameter yta;
		CTreeParameter bta;
		CTreeParameter dta;
		CTreeVariable anglea;
		CTreeVariable angleb;
		CTreeVariable brho;
		CTreeVariable mass;
		CTreeVariable charge;
		CTreeParameter scatter;
		CTreeParameter energy;
		CTreeVariable order;
		CS800Map map;
	public:
		void Initialize(CS800* top, string name);
		void CalculateTracking();
};	

// Focal Plane
class CS800FocalPlane
{
	private:
		CS800* top;
	public:
		CS800FpScintillator e1;
		CS800FpScintillator e2;
		CS800FpScintillator e3;
		CS800FpIonChamber ic;
		CS800FpCrdc crdc1;
		CS800FpCrdc crdc2;
		CS800FpTrack track;
		CTreeVariable gap;
	public:
		void Initialize(CS800* top, string name);
		void LoadValues();
};

// PPAC
class CS800Ppac
{
	private:
		CS800* top;
		Bool_t hasdata;
	public:
		CTreeParameter u;
		CTreeParameter d;
		CTreeParameter l;
		CTreeParameter r;
		CTreeParameter a;
		CTreeParameter t;
		CTreeParameter sumx;
		CTreeParameter sumy;
		CTreeParameter x;
		CTreeParameter y;
		CTreeVariable z;
		CTreeVariable u_offset;
		CTreeVariable u_slope;
		CTreeVariable d_offset;
		CTreeVariable d_slope;
		CTreeVariable l_offset;
		CTreeVariable l_slope;
		CTreeVariable r_offset;
		CTreeVariable r_slope;
		CTreeVariable x_offset;
		CTreeVariable x_slope;
		CTreeVariable y_offset;
		CTreeVariable y_slope;
	public:
		void Initialize(CS800* top, string name);	
		Bool_t HasData() {return hasdata;}
		void Reset() {hasdata = kfFALSE;}
		TranslatorPointer<UShort_t> Unpack(TranslatorPointer<UShort_t> pData);
		void Calibrate();
		void CalculatePosition();
};

// PIN
class CS800Pin
{
	private:
		CS800* top;
		Bool_t hasdata;
	public:
		CTreeParameter e;
		CTreeParameter t;
		CTreeParameter dE;
		CTreeVariable e_offset;
		CTreeVariable e_slope;
	public:
		void Initialize(CS800* top, string name);	
		Bool_t HasData() {return hasdata;}
		void Reset() {hasdata = kfFALSE;}
		TranslatorPointer<UShort_t> Unpack(TranslatorPointer<UShort_t> pData);
		void Calibrate();
};

// Target Chamber
class CS800Target
{
	private:
		CS800* top;
	public:
		CS800Ppac ppac2;
		CS800Pin pin1;
		CS800Pin pin2;
		CTreeParameter E;
	public:
		void Initialize(CS800* top, string name);
		void Calculate();
};

// Intermediate Image
class CS800Image
{
	private:
		CS800* top;
	public:
		CS800Ppac ppac0;
		CS800Ppac ppac1;
		CTreeParameter x;
		CTreeParameter theta;
		CTreeParameter y;
		CTreeParameter phi;
		CTreeParameter dp;
		CTreeVariable z;
		CTreeVariable gap;
		CTreeVariable dpphi;
	public:
		void Initialize(CS800* top, string name);
		void Calculate();
};

// Time of Flight
class CS800TimeOfFlight
{
	private:
		CS800* top;
		Bool_t hasdata;
	public:
		CTreeParameter rf;
		CTreeParameter obj;
		CTreeParameter xfp;
		CTreeParameter obje1;
		CTreeParameter xfpe1;
		CTreeParameter mraw;
		CTreeParameter mlive;
		CTreeVariable rfCorrection;
		CTreeVariable objCorrection;
		CTreeVariable xfpCorrection;
	public:
		void Initialize(CS800* top, string name);
		Bool_t HasData() {return hasdata;}
		void Reset() {hasdata = kfFALSE;}
		TranslatorPointer<UShort_t> Unpack(TranslatorPointer<UShort_t> pData);
		void CalculateTOF();
};

// Bit register
class CS800BitRegister
{
	private:
		CS800* top;
	public:
		CTreeParameter reg;
	public:
		void Initialize(CS800* top, string name);
		TranslatorPointer<UShort_t> Unpack(TranslatorPointer<UShort_t> pData);
};

// Particle ID
class CS800PID
{
	private:
		CS800* top;
	public:
		CS800TimeOfFlight tof;
		CS800BitRegister bit;
	public:
		void Initialize(CS800* top, string name);
};

// The whole S800
class CS800
{
	public:
		CS800FocalPlane fp;
		CS800Target ta;
		CS800Image im;
		CS800PID pid;
	public:
		CS800(string name);
		void Reset();
};

#endif
