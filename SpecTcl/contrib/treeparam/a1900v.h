// Actual construction of the variable tree structure
// which has to be done only once
// this file has to be edited in parallel with a1900Variables.h
// which contains the structure declarations

vector<CTreeVariable*> CTreeVariable::pSelf;

struct fragmentseparatorcalibrator a1900v = {
	{
		CTreeVariable("a1900v.pin.offset", 0, "MeV"),
		CTreeVariable("a1900v.pin.gain", 1, "MeV/ch"),
		CTreeVariable("a1900v.pin.thickness", 500, "µm")
	},
	{
		CTreeVariable("a1900v.rftof.offset", 0, "ns"),
		CTreeVariable("a1900v.rftof.slope", 1, "ns/ch")
	},
	{
		CTreeVariable("a1900v.pid.brho1", 3, "T.m."),
		CTreeVariable("a1900v.pid.length1", 17.33, "m"),
		CTreeVariable("a1900v.pid.brho2", 3, "T.m."),
		CTreeVariable("a1900v.pid.length2", 17.33, "m")
	}
};

