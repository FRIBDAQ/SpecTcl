#include "CS800.h"
#include <EventProcessor.h>
#include <TCLAnalyzer.h>
#include <Event.h>
#include <math.h>
#include "S800Packets.h"

//---------------------------------------------------------//
// class CS800
CS800::CS800(string name)
{
	fp.Initialize(this, name+".fp");
	ta.Initialize(this, name+".ta");
	im.Initialize(this, name+".im");	
	pid.Initialize(this, name+".pid");
}

void
CS800::Reset()
{
	fp.e1.de.ResetAll(); // Reset all TreeParameters
	fp.e1.Reset();
	fp.e2.Reset();
	fp.e3.Reset();
	fp.ic.Reset();
	fp.crdc1.Reset();
	fp.crdc2.Reset();
    ta.ppac2.Reset();
    ta.pin1.Reset();
    ta.pin2.Reset();
    im.ppac0.Reset();
    im.ppac1.Reset();
    pid.tof.Reset();
}

//---------------------------------------------------------//
// class CS800FpScintillator
void
CS800FpScintillator::Initialize(CS800* theTop, string name)
{
	top = theTop;
	de_up.Initialize(name+".de_up", 11);
	de_down.Initialize(name+".de_down", 11);
	time_up.Initialize(name+".time_up", 12);
	time_down.Initialize(name+".time_down", 12);
	de.Initialize(name+".de", 11);
	time.Initialize(name+".time", 12);
}

TranslatorPointer<UShort_t>
CS800FpScintillator::Unpack(TranslatorPointer<UShort_t> p)
{
	de_up = *p++;
	de_down = *p++;
	time_up = (*p++)&0xfff;
	time_down = (*p++)&0xfff;
	hasdata = kfTRUE;
	return p;
}

void
CS800FpScintillator::Calculate()
{
	de = sqrt(de_up*de_up + de_down*de_down) / 2;
	time = (time_up + time_down) / 2;
}

//---------------------------------------------------------//
// class CS800FpIonChamber
void
CS800FpIonChamber::Initialize(CS800* theTop, string name)
{
	top = theTop;
	raw.Initialize(name+".raw", 12, CS800_FP_IC_CHANNELS, 0);
	slope.Initialize(name+".slope", 1, "au", CS800_FP_IC_CHANNELS, 0);
	offset.Initialize(name+".offset", 1, "au", CS800_FP_IC_CHANNELS, 0);
	cal.Initialize(name+".cal", 12, CS800_FP_IC_CHANNELS, 0);
	sum.Initialize(name+".sum", 12);
	de.Initialize(name+".de", 12, 0, 1000, "MeV", false);
	de_slope.Initialize(name+".de_slope", 0, "MeV/ch");
	de_offset.Initialize(name+".de_offset", 0, "MeV");
}

TranslatorPointer<UShort_t>
CS800FpIonChamber::Unpack(TranslatorPointer<UShort_t> p)
{
	for (int i=0; i<CS800_FP_IC_CHANNELS; i++)
		raw[i] = (*p++)&0xfff;
	hasdata = kfTRUE;
	return p;
}

void
CS800FpIonChamber::Calibrate()
{
	for(int i=0; i<CS800_FP_IC_CHANNELS; i++) {
		cal[i] = raw[i]*slope[i] + offset[i];
	}
}

void
CS800FpIonChamber::Calculate()
{
	sum = 0;
	for(int i=0; i<CS800_FP_IC_CHANNELS; i++)
		sum += cal[i];
	sum /= CS800_FP_IC_CHANNELS;
	de = sum*de_slope + de_offset;
}

//---------------------------------------------------------//
// class CS800FpCrdcPads
void
CS800FpCrdcPads::Initialize(CS800* theTop, string name)
{
	top = theTop;
	raw.Initialize(name+".raw", 12, CS800_FP_CRDC_CHANNELS, 0);
	ped.Initialize(name+".ped", 0, "au", CS800_FP_CRDC_CHANNELS, 0);
	slope.Initialize(name+".slope", 1, "au", CS800_FP_CRDC_CHANNELS, 0);
	offset.Initialize(name+".offset", 0, "au", CS800_FP_CRDC_CHANNELS, 0);
	cal.Initialize(name+".cal", 12, CS800_FP_CRDC_CHANNELS, 0);
}

void
CS800FpCrdcPads::Integrate(unsigned short data[CS800_FP_CRDC_CHANNELS][CS800_FP_CRDC_MAX_WIDTH],\
	unsigned short sampleWidth)
{
	// Integrate samples into pads and subtract pedestals
	for (int q=0; q<CS800_FP_CRDC_CHANNELS; q++) {
		raw[q] = 0;
		for (int s=0; s<sampleWidth; s++) {
			if (data[q][s] != 0) {
				raw[q] += data[q][s] - ped[q];
			}
		}
	}
}

void
CS800FpCrdcPads::Calibrate()
{
	for( int i=0; i<CS800_FP_CRDC_CHANNELS; i++)
		cal[i] = raw[i]*slope[i] + offset[i];
}

//---------------------------------------------------------//
// class CS800FpCrdcDrift
void
CS800FpCrdcDrift::Initialize(CS800* theTop, string name)
{
	top = theTop;
	raw.Initialize(name+".raw", 16);
	slope.Initialize(name+".slope", 1, "ns/ch");
	offset.Initialize(name+".offset", 0, "ns");
	cal.Initialize(name+".cal", 12, 0, 10000, "ns", false);
	hits.Initialize(name+".hits", 4);
}

void
CS800FpCrdcDrift::Calibrate()
{
	cal = raw*slope + offset;
}

//---------------------------------------------------------//
// class CS800FpCrdcCalc
void
CS800FpCrdcCalc::Initialize(CS800* theTop, string name)
{
	top = theTop;
	padmax.Initialize(name+".padmax", 12, 0, CS800_FP_CRDC_CHANNELS, "pad", false);
	padsum.Initialize(name+".padsum", 12);
	x_gravity.Initialize(name+".x_gravity", 12, 0, CS800_FP_CRDC_CHANNELS, "pad", false);
	x_fit.Initialize(name+".x_fit", 12, 0, CS800_FP_CRDC_CHANNELS, "pad", false);
	x_chi2.Initialize(name+".x_chi2", 8, 0, 10, "chi2", false);
	x_width.Initialize(name+".x_width", 9, "pad");
	x_method.Initialize(name+".x_method", 1, "method");
}

void
CS800FpCrdcCalc::CalculateGravity()
{
	double maxpad = 0;
	for (int i=0; i<CS800_FP_CRDC_CHANNELS; i++) {
		if 	(m_pad->cal[i] > maxpad) {
			maxpad = m_pad->cal[i];
			padmax = i;
		}
	}
	int lowpad = (int)padmax - (int)x_width/2;
	int highpad = lowpad + (int)x_width - 1;
	if (lowpad < 0) {
		lowpad = 0;
		highpad = (int)padmax * 2;
	}
	if (highpad >= CS800_FP_CRDC_CHANNELS) {
		highpad = CS800_FP_CRDC_CHANNELS - 1;
		lowpad = (int)padmax - (CS800_FP_CRDC_CHANNELS-1-(int)padmax);
	}
	double sum = 0, mom = 0;
	for (int i=lowpad; i<= highpad; i++) {
		sum += m_pad->cal[i];
		mom += i*m_pad->cal[i];
	}
	x_gravity = mom / sum;
}

void
CS800FpCrdcCalc::CalculateFit()
{
}

//---------------------------------------------------------//
// class CS800FpCrdc
void
CS800FpCrdc::Initialize(CS800* theTop, string name)
{
	top = theTop;
	m_name = name;
	pad.Initialize(top, name+".pad");
	drift.Initialize(top, name+".drift");
	calc.Initialize(top, name+".calc");
	x.Initialize(name+".x", 12, -300, 300, "mm", false);
	y.Initialize(name+".y", 12, -150, 150, "mm", false);
	x_offset.Initialize(name+".x_offset", -300, "mm");
	x_slope.Initialize(name+".x_slope", 2.54, "mm/pad");
	y_offset.Initialize(name+".y_offset", -150, "mm");
	y_slope.Initialize(name+".y_slope", 1, "mm/ns");
	padsnap.Initialize(name+".padsnap", 0, "pad");
	calc.m_pad = &pad;
}

TranslatorPointer<UShort_t>
CS800FpCrdc::Unpack(TranslatorPointer<UShort_t> p)
{
        unsigned short test;
	drift.hits = *p++;
	if (drift.hits == 0) return p;	// That's it! there was not hit
	drift.raw = *p++;	// take first hit as good one
	for (int i=1; i<drift.hits; i++) p++;	// skip other hits
	test = *(p+3);
	if (test != 0x5804) return p; // if we have gotten rid of ULM data we need to return now
	m_threshold = *p++;
	m_sampleBegin = *p++;
	if (*(p+1) == S800_FP_CRDC_SAMPLE_PACKET) {	// we are reading samples
		p = UnpackSamples(p);
		hasdata = kfTRUE;
	}
	if (*(p+1) == S800_FP_CRDC_PAD_PACKET) {	// we are reading pads
		p = UnpackPads(p);
		hasdata = kfTRUE;
	}
	return p;
}

TranslatorPointer<UShort_t>
CS800FpCrdc::UnpackSamples(TranslatorPointer<UShort_t> p)
{
	UShort_t sampleLength, dap, data, currentSample;
	memset(m_data, 0, sizeof(m_data));
	currentSample = m_sampleBegin;

	// stuff our padxsample array with the data
	do {
		sampleLength = *p++;
		p++;					// skip packet id
		for (int i=0; i<sampleLength-2; i++) {
			dap = (*p)>>8;		// upper byte = pad
			data = (*p)&0xff;	// lower byte = data
			if (data < m_threshold) {
				printf("Warning: data lower than threshold in %s (pad=%d; sample=%d)\n",\
				m_name.c_str(), dap, currentSample);
			} else {
				m_data[dap][currentSample-m_sampleBegin] = data;
			}
			p++;
		}
		currentSample++;
	} while (*(p+1) == S800_FP_CRDC_SAMPLE_PACKET);
	m_sampleWidth = currentSample - m_sampleBegin;

	// integrate samples into pads
	pad.Integrate(m_data, m_sampleWidth);

	return p;
}

TranslatorPointer<UShort_t>
CS800FpCrdc::UnpackPads(TranslatorPointer<UShort_t> p)
{
}

void
CS800FpCrdc::CalculatePosition()
{
	y = drift.cal * y_slope + y_offset;
	if (calc.x_method == 1) {
		calc.CalculateGravity();
		x = calc.x_gravity * x_slope + x_offset;
	} else if (calc.x_method == 2) {
		calc.CalculateGravity();
		calc.CalculateFit();
		x = calc.x_fit * x_slope + x_offset;
	} else {
		x = 0;
	}
}

void
CS800FpCrdc::FillSnapshotSpectra(CAnalyzer& rAnalyzer)
{
	UInt_t channel[2];
	string aString;
	CSpectrum *snapSpectrum;
	CHistogrammer *histogrammer = (CHistogrammer*)rAnalyzer.getSink();

	// Fill 2D snapshot
	aString = m_name + ".snapshots";
	
	snapSpectrum = histogrammer->FindSpectrum(aString);
	if (snapSpectrum != NULL) {
		snapSpectrum->Clear();
		Size_t xSize = snapSpectrum->Dimension(0);
		Size_t ySize = snapSpectrum->Dimension(1);
		for (int p=0; p<CS800_FP_CRDC_CHANNELS; p++) {
			if (p > xSize) break;
			channel[0] = p;
			for (int s=m_sampleBegin, i=0; i<m_sampleWidth; s++, i++) {
				if (s < ySize) {
					channel[1] = s;
					snapSpectrum->set((const UInt_t*)&channel,
					(ULong_t)m_data[p][i]);
				} else {
					break;
				}
			}
		}
	}

	// Fill 1D snapshot
	aString = m_name + ".snapshot";
	
	snapSpectrum = histogrammer->FindSpectrum(aString);
	if (snapSpectrum != NULL) {
		snapSpectrum->Clear();
		Size_t xSize = snapSpectrum->Dimension(0);
		for (int s=m_sampleBegin, i=0; i<m_sampleWidth; s++, i++) {
			if (s < xSize) {
       			channel[0] = s;
       			snapSpectrum->set((const UInt_t*)&channel,
       			(ULong_t)m_data[(int)padsnap][i]);
       		} else {
       			break;
       		}
		}
	}
	
	// Fill 1D pad snapshot
	aString = m_name + ".padsnap";
	
	snapSpectrum = histogrammer->FindSpectrum(aString);
	if (snapSpectrum != NULL) {
		snapSpectrum->Clear();
		for (int p=0; p<CS800_FP_CRDC_CHANNELS; p++) {
			channel[0] = p;
       		snapSpectrum->set((const UInt_t*)&channel, (ULong_t)pad.cal[p]);
		}
	}
}

//---------------------------------------------------------//
// class CS800Map
void
CS800Map::Initialize(CS800* theTop, string name)
{
	char str[20];
	top = theTop;
	maxcoefficients.Initialize(name+".maxcoefficients", CS800_TRACK_COEFFICIENTS, "number");
	maxparameters.Initialize(name+".maxparameters", CS800_TRACK_PARAMETERS, "number");
	maxorder.Initialize(name+".maxorder", 0, "order");
	for (int i=0; i<CS800_TRACK_PARAMETERS; i++) {
		maxcoefficient[i] = 0;
		for (int j=0; j<CS800_TRACK_COEFFICIENTS; j++) {
			order[i][j] = 0;
			coefficient[i][j] = 0;
			for (int k=0; k<CS800_TRACK_PARAMETERS; k++) {
				exponent[i][k][j] = 0;
			}
		}
	}
}

double
CS800Map::Calculate(int calcorder, int parameter, double *input)
{
	double cumul=0;
	double multiplicator;
	for (int index=0; index<maxcoefficient[parameter]; index++) {
		if (calcorder < order[parameter][index]) break;
		multiplicator = 1;
		for (int nex=0; nex<CS800_TRACK_PARAMETERS; nex++) {
			if (exponent[parameter][nex][index] != 0) {
				multiplicator *= pow(input[nex], exponent[parameter][nex][index]);
			}
		}
		cumul += multiplicator * coefficient[parameter][index];
	}
	return cumul;
}

//---------------------------------------------------------//
// class CS800FpTrack
void
CS800FpTrack::Initialize(CS800* theTop, string name)
{
	top = theTop;
	map.Initialize(top, name+".map");
	xfp.Initialize(name+".xfp", 12, -0.3, 0.3, "m", false);
	afp.Initialize(name+".afp", 12, -0.1, 0.1, "rad", false);
	yfp.Initialize(name+".yfp", 12, -0.15, 0.15, "m", false);
	bfp.Initialize(name+".bfp", 12, -0.15, 0.15, "rad", false);
	zfp.Initialize(name+".zfp", 0, "m");
	ata.Initialize(name+".ata", 12, -0.1, 0.1, "rad", false);
	yta.Initialize(name+".yta", 12, -0.01, 0.01, "m", false);
	bta.Initialize(name+".bta", 12, -0.15, 0.15, "rad", false);
	dta.Initialize(name+".dta", 12, -0.06, 0.06, "part", false);
	anglea.Initialize(name+".anglea", 0, "degree");
	angleb.Initialize(name+".angleb", 0, "degree");
	brho.Initialize(name+".brho", 3, "Tm");
	mass.Initialize(name+".mass", 1, "amu");
	charge.Initialize(name+".charge", 1, "q");
	scatter.Initialize(name+".scatter", 12, 0, 300, "mrad", false);
	energy.Initialize(name+".energy", 12, -1000, 1000, "MeV", false);
	order.Initialize(name+".order", 5, "order");
}

void
CS800FpTrack::CalculateTracking()
{
	double input[CS800_TRACK_PARAMETERS];
	double pi=3.14159265;
	double amu=931.5016;
	double betagamma, gamma, energy0;
	if (!map.WasLoaded()) return;
	input[0] = xfp;
	input[1] = afp;
	input[2] = yfp;
	input[3] = bfp;
	if ((double)order > (double)map.maxorder) order = map.maxorder;
	ata = map.Calculate(order, 0, input);
	yta = map.Calculate(order, 1, input);
	bta = map.Calculate(order, 2, input);
	dta = map.Calculate(order, 3, input);
	ata += anglea/180*pi;
	bta += angleb/180*pi;
	scatter = atan(sqrt(tan(ata)*tan(ata) + tan(bta)*tan(bta))) * 1000;
	betagamma = brho / 3.107 * charge / mass;
	gamma = sqrt(betagamma*betagamma + 1);
	energy0 = mass * amu * (gamma - 1);
	energy = dta * energy0; 
}

//---------------------------------------------------------//
// class CS800FocalPlane
void
CS800FocalPlane::Initialize(CS800* theTop, string name)
{
	top = theTop;
	e1.Initialize(top, name+".e1");
	e2.Initialize(top, name+".e2");
	e3.Initialize(top, name+".e3");
	ic.Initialize(top, name+".ic");
	crdc1.Initialize(top, name+".crdc1");
	crdc2.Initialize(top, name+".crdc2");
	track.Initialize(top, name+".track");
	gap.Initialize(name+".gap", 1073.0, "mm");
}

void
CS800FocalPlane::LoadValues()
{
	track.afp = atan((crdc2.x - crdc1.x) / gap);
	track.bfp = atan((crdc2.y - crdc1.y) / gap);
        track.xfp = crdc1.x / 1000 + track.zfp * tan(track.afp);
	track.yfp = crdc1.y / 1000 + track.zfp * tan(track.bfp);
}
	
//---------------------------------------------------------//
// Class CS800Ppac
void
CS800Ppac::Initialize(CS800* theTop, string name)
{
	top = theTop;
	u.Initialize(name+".u", 12);
	d.Initialize(name+".d", 12);
	l.Initialize(name+".l", 12);
	r.Initialize(name+".r", 12);
	a.Initialize(name+".a", 12);
	t.Initialize(name+".t", 12);
	sumx.Initialize(name+".sumx", 12);
	sumy.Initialize(name+".sumy", 12);
	x.Initialize(name+".x", 12, -51.2, 51.175, "mm", false);
	y.Initialize(name+".y", 12, -51.2, 51.175, "mm", false);
	z.Initialize(name+".z", 0, "mm");
	u_offset.Initialize(name+".u_offset", 0, "");
	u_slope.Initialize(name+".u_slope",  1, "");
	d_offset.Initialize(name+".d_offset", 0, "");
	d_slope.Initialize(name+".d_slope",  1, "");
	l_offset.Initialize(name+".l_offset", 0, "");
	l_slope.Initialize(name+".l_slope",  1, "");
	r_offset.Initialize(name+".r_offset", 0, "");
	r_slope.Initialize(name+".r_slope",  1, "");
	x_offset.Initialize(name+".x_offset", 0, "mm");
	x_slope.Initialize(name+".x_slope",  1, "mm/ch");
	y_offset.Initialize(name+".y_offset", 0, "mm");
	y_slope.Initialize(name+".y_slope",  1, "mm/ch");
}

TranslatorPointer<UShort_t>
CS800Ppac::Unpack(TranslatorPointer<UShort_t> p)
{
	u = (*p++)&0xfff;
	d = (*p++)&0xfff;
	l = (*p++)&0xfff;
	r = (*p++)&0xfff;
	t = (*p++)&0xfff;
	hasdata = kfTRUE;
	return p;
}

void
CS800Ppac::Calibrate()
{
	l = l * l_slope + l_offset;
	r = r * r_slope + r_offset;
	u = u * u_slope + u_offset;
	d = d * d_slope + d_offset;

	sumx = l + r;
	sumy = u + d;
}

void
CS800Ppac::CalculatePosition()
{
	if (sumx > 0) {
		x = ((r - l) / sumx) * x_slope + x_offset;
		x.setValid();
	} else {
		x = 0.0;
		x.setInvalid();
		sumx.setInvalid();
	}

	if (sumy > 0) {
		y = ((u - d) / sumy) * y_slope + y_offset;
		y.setValid();
	} else {
		y = 0.0;
		y.setInvalid();
		sumy.setInvalid();
	}
}

//---------------------------------------------------------//
// class CS800Pin
void
CS800Pin::Initialize(CS800* theTop, string name)
{
	top = theTop;
	e.Initialize(name+".e", 12);
	t.Initialize(name+".t", 12);
	dE.Initialize(name+".dE", 12, 0, 1023, "MeV", false);
	e_offset.Initialize(name+".e_offset", 0, "MeV");
	e_slope.Initialize(name+".e_slope", 1, "MeV/ch");
}

TranslatorPointer<UShort_t>
CS800Pin::Unpack(TranslatorPointer<UShort_t> p)
{
	e = (*p++)&0xfff;
	t = (*p++)&0xfff;
	hasdata = kfTRUE;
	return p;
}

void
CS800Pin::Calibrate()
{
	dE = e * e_slope + e_offset;
}

//---------------------------------------------------------//
// class CS800Target
void
CS800Target::Initialize(CS800* theTop, string name)
{
	top = theTop;
	ppac2.Initialize(top, name+".ppac2");
	pin1.Initialize(top, name+".pin1");
	pin2.Initialize(top, name+".pin2");
	E.Initialize(name+".E", 12, 0, 4095, "MeV", false);
}

void
CS800Target::Calculate()
{
  E = pin1.dE + pin2.dE;
}

//---------------------------------------------------------//
// class CS800Image
void
CS800Image::Initialize(CS800* theTop, string name)
{
	top = theTop;
	ppac0.Initialize(top, name+".ppac0");
	ppac1.Initialize(top, name+".ppac1");
	x.Initialize(name+".x", 12, -51.2, 51.175, "mm", false);
	theta.Initialize(name+".theta", 12, -51.2, 51.175, "mrad", false);
	y.Initialize(name+".y", 12, -51.2, 51.175, "mm", false);
	phi.Initialize(name+".phi", 12, -51.2, 51.175, "mrad", false);
	dp.Initialize(name+".dp", 12, -50, 50, "mom", false);
	z.Initialize(name+".z", 120.6, "mm");
	gap.Initialize(name+".gap", 482.6, "mm");
	dpphi.Initialize(name+".dpphi", 0, "slope");
}

void
CS800Image::Calculate()
{
  theta = atan((ppac1.x-ppac0.x)/gap)*1000;
  phi = atan((ppac1.y-ppac0.y)/gap)*1000;
  x = ppac0.x+z*tan(theta/1000);
  y = ppac0.y+z*tan(phi/1000);
  dp = y + phi * dpphi;
}

//---------------------------------------------------------//
// class CS800TimeOfFlight
void
CS800TimeOfFlight::Initialize(CS800* theTop, string name)
{
	top = theTop;
	rf.Initialize(name+".rf", 12);
	obj.Initialize(name+".obj", 12);
	xfp.Initialize(name+".xfp", 12);
	obje1.Initialize(name+".obje1", 12);
	xfpe1.Initialize(name+".xfpe1", 12);
	mraw.Initialize(name+".mraw", 12);
	mlive.Initialize(name+".mlive", 12);
	rfCorrection.Initialize(name+".rfCorrection", 0, "rf/dta");
	objCorrection.Initialize(name+".objCorrection", 0, "obj/dta");
	xfpCorrection.Initialize(name+".xfpCorrection", 0, "xfp/dta");
}

TranslatorPointer<UShort_t>
CS800TimeOfFlight::Unpack(TranslatorPointer<UShort_t> p)
{
	UShort_t length = *(p-2);
	rf = (*p++)&0xfff;
	hasdata = kfTRUE;
	if (length == 3) return p;
	obj = (*p++)&0xfff;
	if (length == 4) return p;
	xfp = (*p++)&0xfff;
	if (length == 5) return p;
	mraw = (*p++)&0xfff;
	if (length == 6) return p;
	mlive = (*p++)&0xfff;
	return p;
}

void
CS800TimeOfFlight::CalculateTOF()
{
	obje1 = obj - top->fp.e1.time;
	xfpe1 = xfp - top->fp.e1.time;
}

//---------------------------------------------------------//
// class CS800BitRegister
void
CS800BitRegister::Initialize(CS800* theTop, string name)
{
	top = theTop;
	reg.Initialize(name+".reg", 4);
}

TranslatorPointer<UShort_t>
CS800BitRegister::Unpack(TranslatorPointer<UShort_t> p)
{
	reg = *p++;
	return p;
}

//---------------------------------------------------------//
// class CS800PID
void
CS800PID::Initialize(CS800* theTop, string name)
{
	top = theTop;
	tof.Initialize(top, name+".tof");
	bit.Initialize(top, name+".bit");
}
