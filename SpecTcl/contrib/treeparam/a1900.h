// Actual construction of the parameter tree structure
// which has to be done only once
// this file has to be edited in parallel with a1900Parameters.h
// which contains the structure declarations

CEvent*				CTreeParameter::pEvent = NULL;
vector<CTreeParameter*> CTreeParameter::pSelf;

struct fragmentseparator a1900 = {
	CTreeParameter("a1900.rf", 12),
/* focal plane parameters */
	{
		{
			CTreeParameter("a1900.fp.ppac0.down", 12),
			CTreeParameter("a1900.fp.ppac0.up", 12),
			CTreeParameter("a1900.fp.ppac0.left", 12),
			CTreeParameter("a1900.fp.ppac0.right", 12),
			CTreeParameter("a1900.fp.ppac0.anode", 12),
			CTreeParameter("a1900.fp.ppac0.time", 12),
			CTreeParameter("a1900.fp.ppac0.sumx", 12),
			CTreeParameter("a1900.fp.ppac0.sumy", 12),
			CTreeParameter("a1900.fp.ppac0.sumxy", 12),
			CTreeParameter("a1900.fp.ppac0.x", 12, -25, 25, "mm", false),
			CTreeParameter("a1900.fp.ppac0.y", 12, -25, 25, "mm", false)
		},
		{
			CTreeParameter("a1900.fp.ppac1.down", 12),
			CTreeParameter("a1900.fp.ppac1.up", 12),
			CTreeParameter("a1900.fp.ppac1.left", 12),
			CTreeParameter("a1900.fp.ppac1.right", 12),
			CTreeParameter("a1900.fp.ppac1.anode", 12),
			CTreeParameter("a1900.fp.ppac1.time", 12),
			CTreeParameter("a1900.fp.ppac1.sumx", 12),
			CTreeParameter("a1900.fp.ppac1.sumy", 12),
			CTreeParameter("a1900.fp.ppac1.sumxy", 12),
			CTreeParameter("a1900.fp.ppac1.x", 12, -25, 25, "mm", false),
			CTreeParameter("a1900.fp.ppac1.y", 12, -25, 25, "mm", false)
		},
		CTreeParameter("a1900.fp.x", 12, -25, 25, "mm", false),
		CTreeParameter("a1900.fp.y", 12, -25, 25, "mm", false),
		CTreeParameter("a1900.fp.theta", 12, -40, 40, "mrad", false),
		CTreeParameter("a1900.fp.phi", 12, -40, 40, "mrad", false),
		{
			CTreeParameter("a1900.fp.pin.de", 12),
			CTreeParameter("a1900.fp.pin.time", 12),
			CTreeParameter("a1900.fp.pin.mev", 12, 0, 1000, "MeV", false)
		},
		{
			{
				CTreeParameter("a1900.fp.stack.0.de", 12),
				CTreeParameter("a1900.fp.stack.0.mev", 12, 0, 2000, "MeV", false)
			},
			{
				CTreeParameter("a1900.fp.stack.1.de", 12),
				CTreeParameter("a1900.fp.stack.1.mev", 12, 0, 2000, "MeV", false)
			},
			{
				CTreeParameter("a1900.fp.stack.2.de", 12),
				CTreeParameter("a1900.fp.stack.2.mev", 12, 0, 2000, "MeV", false)
			},
			{
				CTreeParameter("a1900.fp.stack.3.de", 12),
				CTreeParameter("a1900.fp.stack.3.mev", 12, 0, 2000, "MeV", false)
			}
		},
		{
			CTreeParameter("a1900.fp.scint.enorth", 12),
			CTreeParameter("a1900.fp.scint.esouth", 12),
			CTreeParameter("a1900.fp.scint.timenorth", 12),
			CTreeParameter("a1900.fp.scint.timesouth", 12),
			CTreeParameter("a1900.fp.scint.e", 12, 0, 5000, "MeV", false)
		}
	},
	{
		{
			CTreeParameterArray("a1900.im2.ppac0.anode", 12, 4, 0),
			CTreeParameterArray("a1900.im2.ppac0.time", 12, 4, 0),
			CTreeParameterArray("a1900.im2.ppac0.cathode", 12, 192, 0),
			CTreeParameter("a1900.im2.ppac0.x", 12, -200, 200, "mm", false),
			CTreeParameter("a1900.im2.ppac0.y", 12, -40, 40, "mm", false),
		},
		{
			CTreeParameterArray("a1900.im2.ppac1.anode", 12, 4, 0),
			CTreeParameterArray("a1900.im2.ppac1.time", 12, 4, 0),
			CTreeParameterArray("a1900.im2.ppac1.cathode", 12, 192, 0),
			CTreeParameter("a1900.im2.ppac1.x", 12, -200, 200, "mm", false),
			CTreeParameter("a1900.im2.ppac1.y", 12, -40, 40, "mm", false),
		},
		{
			CTreeParameter("a1900.im2.scint.denorth", 12),
			CTreeParameter("a1900.im2.scint.desouth", 12),
			CTreeParameter("a1900.im2.scint.timenorth", 12),
			CTreeParameter("a1900.im2.scint.timesouth", 12)
		},
		CTreeParameter("a1900.im2.x", 12, -200, 200, "mm", false),
		CTreeParameter("a1900.im2.y", 12, -40, 40, "mm", false),
		CTreeParameter("a1900.im2.theta", 12, -60, 60, "mrad", false),
		CTreeParameter("a1900.im2.phi", 12, -40, 40, "mrad", false),
	},
	{
		{
			CTreeParameter("a1900.pid.tof.rfpin", 12, 200, 400, "ns", false),
			CTreeParameter("a1900.pid.tof.rfscint", 12, 200, 400, "ns", false),
			CTreeParameter("a1900.pid.tof.im2fp", 12, 100, 300, "ns", false)
		},
		CTreeParameter("a1900.pid.Z", 12, 0, 30, "Z", false),
		CTreeParameter("a1900.pid.AoQ", 12, 1, 4, "AoQ", false),
		CTreeParameter("a1900.pid.A", 12, 0, 60, "A", false)
	}
};
