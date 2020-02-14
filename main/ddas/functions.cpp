/*
    This software is Copyright by the Board of Trustees of Michigan
    State University (c) Copyright 2017.

    You may use this software under the terms of the GNU public license
    (GPL).  The terms of this license are described at:

     http://www.gnu.org/licenses/gpl.txt

     Authors:
             Ron Fox
             Jeromy Tompkins 
	     NSCL
	     Michigan State University
	     East Lansing, MI 48824-1321
*/

/** @file:  functions.h
 *  @brief: Implement functions used to fit DDAS pulses.
 *  @note all functions are in the DDAS namespace.
 *  
 */
#include "functions.h"
#include <math.h>


#include <iostream>
#include <fstream>
#include <cmath>
#include <stdexcept>

using namespace DDAS;



/**
 * logistic
 *    Evaluate a logistic function for the specified parameters and point.
 *    A logistic function is a function with a sigmoidal shape.  We use it
 *    to fit the rising edge of signals DDAS digitizes from detectors.
 *    See e.g. https://en.wikipedia.org/wiki/Logistic_function for
 *    a discussion of this function.
 *
 * @param A  - Amplitude of the signal.
 * @param k1 - steepness of the signal (related to the rise time).
 * @param x1 - Mid point of the rise of the sigmoid.
 * @param x  = Location at which to evaluate the function.
 * @return double
 */
double
DDAS::logistic(double A, double k, double x1, double x)
{
    return A/(1+exp(-k*(x-x1)));
}

/**
 * decay
 *    Signals from detectors usually have a falling shape that approximates
 *    an exponential.  This function evaluates this decay at some point.
 *
 *  @param A1 - amplitude of the signal
 *  @param k1 - Decay time factor f the signal.
 *  @param x1 - Position of the pulse.
 *  @param x  - Where to evaluate the signal.
 *  @return double
 */
double
DDAS::decay(double A, double k, double x1, double x)
{
    return A*(exp(-k*(x-x1)));
}
/**
 * switchOn
 *    Provides a multipier that can be used to turn on a function
 *    at a point in space.  This is provided for GPUs that may want
 *    to define functions with conditional chunks without the associated
 *    flow control divergence conditionals would require.
 *
 *    This is implemented as a _very_ steep logistic with rise centered at the
 *    point in question.
 *
 *  @param x1  - Point at which the function turns on (to the right of that point).
 *  @param x   - Point in space at which to evaluate the switch.
 *  @double    - A value that's very nearly 0.0 left of x1 and very nearly 1.0 to
 *               the right of x1.
 */
double
DDAS::switchOn(double x1, double x)
{
    return logistic(1.0, 10000.0, x1, x);
}

/**
 * singlePulse
 *    Evaluate the value of a single pulse in accordance with our
 *    canonical functional form.  The form is a sigmoid rise with an
 *    exponential decay that sits on top of a constant offset.
 *    The exponential decay is turned on with switchOn() above when
 *    x > the rise point of the sigmoid.
 *
 * @param A1  - pulse amplitiude
 * @parm  k1  - sigmoid rise steepness.
 * @param k2  - exponential decay time constant.
 * @param x1  - sigmoid position.
 * @param C   - Constant offset.
 * @param x   - Position at which to evaluat this function
 * @return double
 */
double
DDAS::singlePulse(
    double A1, double k1, double k2, double x1, double C, double x
)
{
    return (logistic(A1, k1, x1, x)  * decay(1.0, k2, x1, x)) // decay term
        + C;                                        // constant.
}
/**
 * doublePulse
 *    Evaluate the canonical form of a double pulse.  This is done
 *    by summing two single pulses.  The constant term is thrown into the
 *    first pulse.  The second pulse gets a constant term of 0.
 *
 * @param A1   - Amplitude of the first pulse.
 * @param k1   - Steepness of first pulse rise.
 * @param k2   - Decay time of the first pulse.
 * @param x1   - position of the first pulse.
 *
 * @param A2   - Amplitude of the second pulse.
 * @param k3   - Steepness of second pulse rise.
 * @param k4   - Decay time of second pulse.
 * @param x2   - position of second pulse.
 *
 * @param C    - Constant offset the pulses sit on.
 * @param x    - position at which to evaluate the pulse.
 * @return double.
 * 
*/
double
DDAS::doublePulse(
    double A1, double k1, double k2, double x1,
    double A2, double k3, double k4, double x2,
    double C, double x    
)
{
    double p1 = singlePulse(A1, k1, k2, x1, C, x);
    double p2 = singlePulse(A2, k3, k4, x2, 0.0, x);
    return p1 + p2;
}
/**
 * pulseAmplitude
 *    The A term in a pulse is not actually the amplitude.  It's the
 *    value of the logistic 1/2 way up the logistic.  The effect
 *    of the exponential decay is already important causing A to
 *    overestimate the amplitude.
 *
 *    This function computes the amplitude of a pulse given its
 *    parameters.  This is done by noting that the
 *    deriviative of the pulse has a zero at:
 *
 *    x = x0 + ln(k1/k2 - 1)/k1
 *
 *    We plug that position back into the pulse to get the amplitude.
 *
 *  @param A - The scaling term of the pulse.
 *  @param k1 - The steepness term of the logistic.
 *  @param k2 - The fall time term of the decay.
 *  @param x0 - The position of the pulse
 *  @return double - The amplitude of the fitted pulse.
 *  @note   the return value will be negative for pulses where k1/k2 <= 1.
 */
double
DDAS::pulseAmplitude(double A, double k1, double k2, double x0)
{
    double frac = k1/k2;
    if (frac <= 1.0) {
        return -1; 
    }
    double pos = x0 + log(frac-1.0)/k1;
    return singlePulse(A, k1, k2, x0, 0.0, pos);
}
// This is there for compatibility sake

double pulseAmplitude(double A, double k1, double k2,double x0)
{
    static bool warned(false);
    if(!warned) {
        std::cerr << "WARNING the pulseAmplitude function is in the DDAS namespace\n";
        std::cerr << "It should be called as DDAS::pulseAmplitude(...);\n";
        warned = true;
    }
    DDAS::pulseAmplitude(A, k1, k2, x0);
}


/**
 * chiSquare1
 *   Computes the Chi Square goodness of a specific parameterization
 *   of a single pulse canonical form with respect to a trace.
 *
 * @param A1  - Amplitude of pulse
 * @parma k1  - steepness of pulse rise.
 * @param k2  - Decay time of pulse fall
 * @param x1  - Position of the pulse.
 * @param C   - Constant offset of the trace.
 * @param trace - Trace to compute the Chisquare with respect to.
 * @param low,high - region of interest over which to compute the chisquare.
 * @return double
 * 
 */
double
DDAS::chiSquare1(
    double A1, double k1, double k2, double x1, double C,
    const std::vector<uint16_t>& trace, int low, int high    
)
{
    if (high == -1) high = trace.size() -1;
    
    double result = 0.0;
    for  (int i = low; i <= high; i++) {
        double x = i;
        double y = trace[i];
        double pulse = singlePulse(A1, k1, k2, x1 ,C, x);  // Fitted pulse.
        double diff = y-pulse;
        if (y != 0.0) {
            result += (diff/y)*diff;  // This order may control overflows
            if (std::fpclassify(result) == FP_ZERO) result =  0.0;
        }
    }
    return result;
}
/**
 * same as above, but a set of x/y points are passed in as the data
 * rather than a trace.
 */
double
DDAS::chiSquare1(
    double A1, double k1, double k2, double x1, double C,
    const std::vector<std::pair<uint16_t, uint16_t> >& points
)
{
    
    double result = 0.0;
    for  (int i = 0; i < points.size(); i++) {
        double x = points[i].first;
        double y = points[i].second;
        double pulse = singlePulse(A1, k1, k2, x1 ,C, x);  // Fitted pulse.
        double diff = y-pulse;
        if (y != 0.0) {
            result += (diff/y)*diff;  // This order may control overflows
            if (std::fpclassify(result) == FP_ZERO) result =  0.0;
        }
    }
    return result;
}
/**
 * chiSquare2
 *   Computes the Chi square goodness of a specific parameterization
 *   of a double pulse canonical form with respect to a trace.
 *
 * @param A1   - Amplitude of the first pulse.
 * @param k1   - Steepness of first pulse rise.
 * @param k2   - Decay time of the first pulse.
 * @param x1   - position of the first pulse.
 *
 * @param A2   - Amplitude of the second pulse.
 * @param k3   - Steepness of second pulse rise.
 * @param k4   - Decay time of second pulse.
 * @param x2   - position of second pulse.
 *
 * @param C    - Constant offset the pulses sit on.
 * @param trace - Trace to compute the chisquare with respect to.
 * @param low   - low limit of trace over which to compute.
 * @param highy - hitgh limit of trace over which to compute,  -1 means full trace.
 * 
 * @return double
 */
double
DDAS::chiSquare2(
    double A1, double k1, double k2, double x1,
    double A2, double k3, double k4, double x2,
    double C,    
    const std::vector<uint16_t>& trace,
    int low, int high
)
{
    double result = 0.0;
    if (high == -1) high = trace.size() -1;
    
    for (int i = low; i <= high; i++) {
        double x = i;
        double y = trace[i];
        double pulse = doublePulse(A1, k1, k2, x1, A2, k3, k4, x2, C, x);
        double diff = y - pulse;
        if (std::fpclassify(diff) == FP_ZERO) diff = 0.0;
        if (y != 0.0) {
            result += (diff/y)*diff;  // This order may control overflows
            if (std::fpclassify(result) == FP_ZERO) result =  0.0;
        }
        
        
    }
    
    return result;
}
// Same as above but a set of X/Y points is passed in rather than the
// trace:

double
DDAS::chiSquare2(
    double A1, double k1, double k2, double x1,
    double A2, double k3, double k4, double x2,
    double C,
    const std::vector<std::pair<uint16_t, uint16_t> >& points
)
{
    double result = 0.0;
    
    for (int i = 0; i < points.size(); i++) {
        double x = points[i].first;
        double y = points[i].second;
        double pulse = doublePulse(A1, k1, k2, x1, A2, k3, k4, x2, C, x);
        double diff = y - pulse;
        if (std::fpclassify(diff) == FP_ZERO) diff = 0.0;
        if (y != 0.0) {
            result += (diff/y)*diff;  // This order may control overflows
            if (std::fpclassify(result) == FP_ZERO) result =  0.0;
        }
    }
    
    return result;
}

/**
 * writeTrace
 *    Write  a single trace to file.
 *
 *  @param filename - where to write
 *  @param title    - title string
 *  @param trace    - the trace.
 */
void
DDAS::writeTrace(
    const char* filename, const char* title, const std::vector<uint16_t>& trace
)
{
    std::ofstream o(filename);
    o << title << std::endl;    
    for (int i =0; i < trace.size(); i++) {
        o << i << " " << trace[i] << std::endl;
    }
}
/**
 * writeTrace2
 *   Write two traces to file  same as above, but the two traces and the
 *   chisquare component of the difference between trace1, trac2
 *   @note the traces must be the same length.
 */
void
DDAS::writeTrace2(
    const char* filename, const char* title,
    const std::vector<uint16_t>& t1, const std::vector<uint16_t>& t2
)
{
    std::ofstream o(filename);
    
    o << title << std::endl;
    for (int i = 0; i < t1.size(); i++) {
        uint16_t diff = t1[i] - t2[i];
        o << i << " " << t1[i] << " " << t2[i]
          << " " << diff*diff/t1[i] <<std::endl;
    }
}

