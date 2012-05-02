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

#include <config.h>
#include "IntegrateCommand.h"
#include <TCLInterpreter.h>
#include <GateContainer.h>

#include <Gate.h>
#include <GateMediator.h>
#include <Spectrum.h>
#include <SpecTcl.h>
#include <histotypes.h>
#include <PointlistGate.h>
#include <Gamma2DW.h>
#include <Cut.h>

#include <math.h>

using namespace std;


static void swapdbl(double& d1, double& d2)
{
  double tmp;
  tmp = d1;
  d1  = d2;
  d2  = tmp;
  
}

static const double fwhmgamma(2.354); // sigma*fwhmgamma = fwhm for gaussians.

///////////////////////////////////////////////////////////////////////////
/*!
    Create the command.. The command will be called 'integrate'
    regardless of what anyone else might want to do:

    \param interp   - Interpreter on which the command is being registered:
*/
CIntegrateCommand::CIntegrateCommand(CTCLInterpreter& interp) :
  CTCLObjectProcessor(interp, "integrate", true)
{}

///////////////////////////////////////////////////////////////////////////
/*!
   Destructor is a no-op but is suppled so that there's a clean
   path of virtual destrcutors all the way back to the base clsss.
*/
CIntegrateCommand::~CIntegrateCommand()
{}


/////////////////////////////////////////////////////////////////////////
/*!
   Command processor for the integrate command.  This is called
   almost directly by the interpreter.
   - The command line must have 3 arguments.
   - objv[1] must be the name of a valid spectrum name.
   - objv[2] must be an area of interest that is valid for the type of
     spectrum named by objv[1]. For a 1-d spectrum, the area of interest
     must be either the name of a cut displayable on that spectrum or a
     pair of floating point values that represent the low/high limits within
     which to integrate.
     For a 2-d spectrum, the area of interest must be either the name of
     a contour displayable on that spectrum or a list of floating point
     x/y points (at least 3 of them) that define a closed polygon that is
     the area of interest.. where insidedness is defined by the 
     the same algorithm used to define insidedness of contour gates.
   - Regardless of how the area of interest is supplied it's turned into
     either spectrum coordinate limits (1d) or  spectrum coordinate
     point lists (2d), then passed to the appropriate integratend function
     for processing.

    \param interp  - The interpreter that is running this command.
    \param objv    - vector of parameter objects. objv[0] is the 
                     command ('integrate') itself.  objv[1], and objv[2]
                     are described above.
    \return int
    \retval TCL_OK    The command  completed normally.  The command result
                      will be the centroid, area and fwhm.  For 2-d spectra,
		      the centroid and fwhm will be x/y value pairs.
    \retval TCL_ERROR The command failed and the result string will be an
                      error message describing why.
*/
int
CIntegrateCommand::operator()(CTCLInterpreter&    interp,
			      vector<CTCLObject>& objv)
{
  // Validate the objv count:

  if  (objv.size() != 3) {
    string result;
    result += "Incorrect parameter count:\n";
    result +=  Usage();
    interp.setResult(result);
    return TCL_ERROR;
  }
  SpecTcl* api = SpecTcl::getInstance();

  objv[1].Bind(interp);
  objv[2].Bind(interp);

  // Pull the spectrum name string out and ensure that's what it is:

  string spectrumName = objv[1];
 
  CSpectrum* pSpectrum = api->FindSpectrum(spectrumName);
  if (!pSpectrum) {
    string result;
    result += spectrumName;
    result += " must be the name of a valid spectrum\n";
    result += Usage();
    return TCL_ERROR;
  }

  // A lot of what we do will depend on the spectrum dimensionality.
  // And that's really straightforward except for summary spectra which
  // have dimensionality of 2 for our purposes, regardless of what they claim.
  //

  string          regionOfInterest = objv[2];
  CGateContainer* pGate            = api->FindGate(regionOfInterest);


  int dimensions = pSpectrum->Dimensionality();
  if (pSpectrum->getSpectrumType() == keSummary) dimensions = 2;

  if (dimensions == 1) {
    // ROI must either be a slice or a properly formatted 2 element
    // tcl list of floats:

    vector<int> limits;

    if (pGate && ((*pGate)->Type() == string("s"))) {
      // Get the limits from the gate

      limits = sliceLimits(*pGate, *pSpectrum);

    }
    else {
      // get the limits from  a list.


      limits = limitsFromList(interp, objv[2], *pSpectrum);
    }
    if (limits.size() != 2) {
      string result;
      result += "1d Spectrum Region of interest is invalid\n";
      result += "Must be the name of a slice displayable on the spectrum \n";
      result += "or a low/high limit pair was: \n";
      result += (string)objv[2];
      interp.setResult(result);
      return TCL_ERROR;
    }
    // Do the integration:

    return integrate1d(interp, *pSpectrum, limits);

  } 
  else if (dimensions == 2) {
    // ROI Must either be a contour or a properly formatted
    // list of coordinate pairs, at least 3.
    //

    vector<CPoint>  coordinates;
    if (pGate && ((*pGate)->Type() == string("c"))) {
      coordinates = contourPoints(*pGate, *pSpectrum);
    }
    else {
      coordinates = roiFromList(interp, objv[2], *pSpectrum);
    }
    if (coordinates.size() < 3) {
      string result;
      result += "2d Spectrum region of interest is invalid\n";
      result += "Must be the name of a contour that is displayable\n";
      result += "on the spectrum or a list of at least 3 coordinates was: \n";
      result += (string)objv[2];
      interp.setResult(result);
      return TCL_ERROR;
    }
    return integrate2d(interp, *pSpectrum, coordinates);

  } 
  else {
    // This should really not happen. but let's catch it cleanly:

    string result;
    result += spectrumName;
    result += " is not a spectrum for which we support integration (yet)\n";
    result += "please make a note of the type of spectrum this is and \n";
    result += "report this to http://daqbugs.nscl.msu.edu";
    return TCL_ERROR;
  }
  
}
/////////////////////////////////////////////////////////////////////
/*
   Produces a vector of 2-d coordinates in spectrum coordinates from
   a contour. The coordinates are not range checked for the spectrum,
   as it's perfectl valid for a figure to be partially or totally out of
   the spectrum range.  However the contour must be displayable on the 
   spectrum.

   Parameters:
       gate     - reference to the gate container for the gate which is assumed to have
                  been checked to be a contour.
       spectrum - reference to the spectrum which is assumed to have been
                  checked to be 2-d.
   Returns
     vector<CPoint> - vector of the points.  Note that if the gate is not
                      displayable this vector will be emtpy to signal the error.
*/
vector<CPoint>
CIntegrateCommand::contourPoints(CGateContainer&     gate,
				 CSpectrum&          spectrum)
{
  vector<CPoint>   points;

  // Make sure this is a displayable gate:

  CGateMediator mediator(gate, &spectrum);
  if (!mediator()) return points; // Empty -- not displayable.

  // Now we need to convert the points to raw channel coordinates.
  // So that we'll know how to integrate,.
  int yIndex;
  if ((spectrum.getSpectrumType() == ke2D)  ||
      (spectrum.getSpectrumType() == keG2DD)) {
    yIndex = 1;
  }
  else {
    CGamma2DW* pS = (CGamma2DW*)(&spectrum);
    yIndex = pS->getnParams();
  }

  CPointListGate& rPointGate((CPointListGate&)(*gate));
  vector<FPoint>   rawPoints = rPointGate.getPoints();

  for (int i=0; i < rawPoints.size(); i++) {
    int x = (int)(spectrum.ParameterToAxis(0, rawPoints[i].X()));
    int  y = (int)(spectrum.ParameterToAxis(yIndex, rawPoints[i].Y()));
    points.push_back(CPoint(x,y));
  }

  return points;
}

////////////////////////////////////////////////////////////////////
/*
  Returns a vector of 2 elements; the lower and upper limits of a 
  slice in spectrum coordinates for the given spectrum.  It is
  acceptable for the slice to be partially off the end of the spectrum,
  it's up to the integration function to decide what that means.

  Parameters:
     gate   - Reference to the gate container for the slice.
              this is already verified to be slice.
     spectrrum- Reference to the spectrum object the points are converted
              to.
  Returns:
      Vector of the low, high limits f the slice, on succes,
      Or an empty vector if the gate is not displayable on the spectrum.

  NOTE:
    There's some wierdness that deals with some pathologies having to do
    with high resolution gates evaluated on low resolution spectra, and
    the fudges that are done to put the gate with its left limit on the
    left side of a channel and the right limit on the right side.
    See the note in Histogrammer.cpp: CHistogrammer::GateToXamineGate
    for more information about this.
*/
vector<int>
CIntegrateCommand::sliceLimits(CGateContainer& gate, CSpectrum& spectrum)
{
  vector<int> points;

  // Make sure this is displayable... if not return point without
  // any effort to fill it.

  CGateMediator mediator(gate, &spectrum);
  if (!mediator()) return points;

  // Now get the limits and convert them to spectrum coordinates.
  // 
  CCut& cut((CCut&)(*gate));
  int low = (int)(spectrum.ParameterToAxis(0, cut.getLow()));
  int hi  = (int)(spectrum.ParameterToAxis(0, cut.getHigh()));

  if (low != hi) hi = hi-1;
  points.push_back(low);
  points.push_back(hi);

  return points;
}
/////////////////////////////////////////////////////////////////////
/*
    Creates a set of spectrum coordinate points from a Tcl object
    that presumably is a list.  The list must be of the form:
    [list [list x1 y1] [list x2 y2] [list x3 y3]...]

    Where there must be at least three x/y coordinate pairs.
    Each coordinate pair is given in 'parameter coordinates', while the
    output array is a set of coordinate pairs in spectrum channel coordinates.

    Parameters:
       interp   - The interpretr that should be bound to the Tcl_Obj wrapper
                  objects.
       obj      - A Tcl object that should contain the list of points
                  as described in the general comments above.
       spectrum - The spectrum to be used for the conversions.

   Returns:
     vector<CPoint>  The vector of coordinate points.  Note that
                    if there is a problem parsing the obj as described
                    above, we're going to return an empty vector.
*/
vector<CPoint>
CIntegrateCommand::roiFromList(CTCLInterpreter& interp,
			       CTCLObject&      obj,
			       CSpectrum&       spectrum)
{
  vector<CPoint> result;
  obj.Bind(interp);
  
  int yIndex = 1;
  if (spectrum.getSpectrumType()  == keG2D) {
    CGamma2DW* pS = (CGamma2DW*)(&spectrum);
    yIndex = pS->getnParams();
  }   
  
  try {
    vector<CTCLObject> list = obj.getListElements();
    if (list.size() < 3) goto error;
    for (int i =0; i < list.size(); i++) {
      CTCLObject element = list[i];              // Should be  2 element list:
      element.Bind(interp);
      
      if (element.llength() != 2) goto error;
      
      double x = element.lindex(0); // Errors will throw causing error exit.
      double y = element.lindex(1);

      int xc   = (int)spectrum.ParameterToAxis(0, x);
      int yc   = (int)spectrum.ParameterToAxis(yIndex, y);
      result.push_back(CPoint(xc, yc));

    }
    
  }
  catch (...) {
    goto error;
  }


  return result;		//  Return the resulting value.

 error:
  result.clear();		//  Return an empty vector.
  return result;
}
//////////////////////////////////////////////////////////////////////
/*
    Get the limits of for an integration when given in raw
    parameter coordinates.  The limits are just a low/high limit pair.
    The user need not give them in order, they'll be appropriately
    shuffled if necessary.

    Parameters:
      interp   - The interpreter that Tcl objects will be bound to.
      obj      - The Tcl object that should be a 2 element coordinate list.
      spectrum - The spectrum that will be used to scale the points to spectrum
                 channels.

   Returns:
     vector<int>   - Containing the low/high limit of the integration
                     parsed from obj.  If there is an error parsing obj,
                     the vector returned will be empty.

*/
vector<int>
CIntegrateCommand::limitsFromList(CTCLInterpreter&  interp,
				  CTCLObject&       object,
				  CSpectrum&        spectrum)
{
  vector<int> result;

  object.Bind(interp);

  try {
    vector<CTCLObject> list = object.getListElements();
    if (list.size() != 2) goto error;

    double low = list[0];
    double hi  = list[1];

    if (low > hi) swapdbl(hi, low);

    // Fill result now:

    result.push_back((int)spectrum.ParameterToAxis(0, low));
    result.push_back((int)spectrum.ParameterToAxis(0, hi));
  }
  catch (...) {
    goto error;
  }
  return result;
    
 error:
  result.clear();
  return result; 
}
///////////////////////////////////////////////////////////////////////
/*
    Produce a string that describes how to use the command:
*/
string
CIntegrateCommand::Usage()
{
  string usage;
  usage += "Usage:\n";
  usage += "   integrate spectrum    gate-name\n";
  usage += "   integrate spectrum-1d [list low high]\n";
  usage += "   integrate spectrum-2d [list [list x1 y1] [list x2 y2] [list x3 y3]...]\n";
  return usage;
}

//////////////////////////////////////////////////////////////////////
/*
   Integrate a 1-d spectrum between a set of limits that are expressed
   as channel coordinates.  The returned value is al ist that
   consists of the centroid (in parameter coordinates), the number of counts
   in the region of interest and the fwhm of the peak under gaussian assumptions.

   Parameters:
     interp   - The intepreter whose result we will set.
     spectrum - The spectrum we will integrate.
     points   - The low/high limit of integration

  Returns:
    TCL_OK   - Integration worked with the result string set as descdribed.
    TCL_ERROR- The integration failed...and why.  At present this can happen
               only if:
               - The limits are completely to the left or right of the spectrum
                 Message: "No spectrum channels are  within the limits"
               - The area is zero in which case both the centroid and FWHM are
                 undefined.
                 Message: "Integration area is zero."

*/
int
CIntegrateCommand::integrate1d(CTCLInterpreter& interp,
			       CSpectrum&      spectrum,
			       vector<int>     limits)
{
  int low = limits[0];
  int hi  = limits[1];


  // Ensure the limits are inside the spectrum:

  if ((low < 0) && (hi << 0)) {
    // Sumregion is entirely to the left:

    interp.setResult("No spectrum channels are within the limits");
    return TCL_ERROR;
  }
  if ((low >= spectrum.Dimension(0))  && (hi >= spectrum.Dimension(0))) {
    // sumregion is entirely to the right:

    interp.setResult("No spectrum channels are within the limits");
    return TCL_ERROR;
  }
  // Force the endpoints to be within the spectrum now that we know
  // that the area of interest does contain some channels:

  if (low < 0) low =0;
  if (hi >= spectrum.Dimension(0)) hi = spectrum.Dimension(0) -1;

  // Now we can start to do the integration:

  double sum   = 0.0;    // Sum of counts.
  double wsum  = 0.0;    // Sum weighted by parameter position.
  double sqsum = 0.0;    // Sum of Square of position weighted by height.


  for (UInt_t chan = low; chan <= hi; chan++) {
    double   position  = spectrum.AxisToParameter(0, chan);
    ULong_t  height    = spectrum[&chan];

    sum   += (double)height;
    double wval = position * (double)height;
    wsum  += wval;
    sqsum += position * wval;
      
  }
  // If there's no area there's no integration.

  if (sum == 0.0) {
    interp.setResult("Integration area is zero");
    return TCL_ERROR;
  }
  // Compute the statistics:

  double centroid = wsum / sum;
  double fwhm     = sqrt(sqsum/sum - centroid*centroid)*fwhmgamma;
  
  // Create the return list:

  CTCLObject result;
  result.Bind(interp);
  result += centroid;
  result += sum;
  result += fwhm;

  interp.setResult(result);
  return TCL_OK;

}

//////////////////////////////////////////////////////////////////////////////

/*  This section of code is responsible for the 2-d integration.
   The key problem that we need to solve is data representation to
   quickly determine how to sum the 'inside' of the object regardless
   of how complex it is.  We use a method called "edge tables" the
   general principle is that we are going to reduce each line segment to 
   a set of points on a scanline. Then we can integrate between pairs
   of points   e.g.  +  integrate here +   don't here + integrate here +.
   
   There are three types of edges we need to consider, and this
   will produce a class hierarchy:
     Horizontal edges:  since points will be inclusive, they will generate
                        start/end points when the scanline they are on is reached.
     Vertical edges:    Generate a point for each scanline in their vertical extent
     Other edges:       For their vertical extent, compute a point to add
                        based on the slope of the line and top point.

   Once these edges produce points, the points are sorted in increasing
   order and integrationdone between pairs of points as shown above.

*/

/*  Let's start off with the class hierarchy that represents the edges.
   The key ability for this class hierarchy is to provide an integration point
   when appropriate to the integration:

   An abstract base class reserves this functionality:
*/

// abstract
class Edge  
{
public:
  virtual void addEdgePoint(vector<int>& existingPoints, int height) = 0;
};

/*

   Horizontal edges just need their height and end points

*/

// final

class HorizontalEdge : public Edge
{
private:
  int m_verticalCoordinate;
  int m_left;
  int m_right;
public:
  HorizontalEdge (int height, int left, int right) :
    m_verticalCoordinate(height),
    m_left(left),
    m_right(right) {}

  virtual void addEdgePoint(vector<int>& existingPoints, int height) {
    if (height == m_verticalCoordinate) {
      existingPoints.push_back(m_left);
      existingPoints.push_back(m_right);
    }
  }
};

/*
   Vertical edges need an upper, lower limit and their horizontal
    position:
*/

// final

class VerticalEdge : public Edge
{
private:
  int m_top;
  int m_bottom;
  int m_horizontalPosition;

public:

  VerticalEdge(int top, int bottom, int position) :
    m_top(top),
    m_bottom(bottom),
    m_horizontalPosition(position) {}

  virtual void addEdgePoint(vector<int>& existingPoints, int height) {
    if ((height >= m_bottom) && (height <= m_top)) {
      existingPoints.push_back(m_horizontalPosition);
    }
  }
};
/*
   Edges that are line segment have a top and bottom coordinate of the
   line segment.  They also have a slope that is pre-computed for the sake
   of efficiency.
*/

// final

class LineSegmentEdge : public Edge
{
private:
  int m_xTop;
  int m_yTop;
  int m_xBottom;
  int m_yBottom;

  double m_slope;

public:
  LineSegmentEdge(CPoint bottom, CPoint top) :
    m_xTop(top.X()),
    m_yTop(top.Y()),
    m_xBottom(bottom.X()),
    m_yBottom(bottom.Y()) 
  {
    
    
    m_slope = ((float)m_yTop - (float)m_yBottom)/
              ((float)m_xTop - (float)m_xBottom);
  }

  // Use the point slope form to locate the x position of
  // a point on the edge specifically:
  // y-y0 = m(x-x0) => x = (y-y0)/m + x0 Arbitrarily use m_xTop,m_yTop as x0/y0.

  virtual void addEdgePoint(vector<int>& existingPoints, int height) {
    if ((height >= m_yBottom) && (height <= m_yTop)) {
      int point = (int)(m_xTop + (height - m_yTop)/m_slope);
      existingPoints.push_back(point);
    }
  }
};


/*
   The point factory is icing on the cake.  Given the coordinates of the
   end points of an edge segment, it can produce the right sort of
   edge.

*/

class EdgeFactory {
public:
  static Edge* createEdge(CPoint first, CPoint second) {
    // vertical?
    
    if (first.X() == second.X()) {
      int low = first.Y();
      int hi  = second.Y();
      if (low > hi) {
	low = second.Y();
	hi = first.Y();
      }
      return new VerticalEdge(hi, low, first.X());
    }

    // Horizontal:

    if (first.Y() == second.Y()) {
      int left  = first.X();
      int right = second.X();
      if (left > right) {
	left  = second.X();
	right = first.X();
      }
      return new HorizontalEdge(first.Y(), left, right);
    }

    // Neither:

    CPoint top    = first;
    CPoint bottom = second;
    if (bottom.Y() > top.Y()) {
      top    = second;
      bottom = first;
    }
    return new LineSegmentEdge(bottom, top);;
  }
};

// Clip a point to the [0,hi) interval.

static int 
clip(int value, int hi) {
  if (value < 0) return 0;
  if (value >= hi) return hi-1;
  return value;
}

// Create the sorted vector of edge points for a scan line.


static vector<int>
makeEdgePoints(vector<Edge*> edges, int y) 
{
  vector<int> result;
  for (int i =0; i < edges.size(); i++) {
    edges[i]->addEdgePoint(result, y);
  }
  // sort the points ascending...

  sort(result.begin(), result.end());

  return result;
}

/*
   Now we have the infrastruture in place to do the 2-d integration:
*/
int 
CIntegrateCommand::integrate2d(CTCLInterpreter&    interp,
			       CSpectrum&          spectrum,
			       STD(vector)<CPoint> points)
{
  // generate the edge tables:

  vector<Edge*> edges;
  for (int i=0; i < points.size()-1;  i++) {
    edges.push_back(EdgeFactory::createEdge(points[i],points[i+1]));
  }

  // Close the figure:

  edges.push_back(EdgeFactory::createEdge(points[0], points[points.size()-1]));

  // One more little efficiency kick before we start summing:
  // We determine the vertical and horizontal limits of the sum for two reasons:
  // 1. If the bounding rectangle is entirely outside the spectrum that's an
  //    error.
  // 2. Once we get down to summing we only have to consider scan-lines
  //    within the vertical extent of the region of interest...clipped to the
  //    spectrum vertical limits that is.

  int minx = points[0].X();
  int maxx = points[0].X();
  int miny = points[0].Y();
  int maxy = points[0].Y();

  for (int i=1; i < points.size(); i++) {
    if (minx > points[i].X()) minx = points[i].X();
    if (maxx < points[i].X()) maxx = points[i].X();

    if ( miny > points[i].Y()) miny = points[i].Y();
    if ( maxy < points[i].Y()) maxy = points[i].Y();
    
  }
  // everything's in channel coords so:

  int xChannels = spectrum.Dimension(0);
  int yChannels = spectrum.Dimension(1);

  // clip bounding box coordinates.. if any pair wind up equal the entire ROI was outside
  // the spectrum:

  minx = clip(minx, xChannels);
  maxx = clip(maxx, xChannels);
  miny = clip(miny, yChannels);
  maxy = clip(maxy, yChannels);

  if ((minx == maxx) || (miny == maxy)) {
    interp.setResult("No spectrum channels are in the region of interest");
    return TCL_ERROR;
  }

  // Ready to sum.

  double sum    = 0.0;		//  Total counts.
  double wxsum  = 0.0;		//  Total of counts weighted by x coord.
  double wysum  = 0.0;          //  Same but weight is by y coord.
  double sqxsum = 0.0;          //  counts weighted by x^2.
  double sqysum = 0.0;          //  counts weighted by y^2.

  // For each scan line in the bounding box, get the edge points and
  // integrate.

  for (int y = miny; y <= maxy; y++) {
    vector<int> edgepoints = makeEdgePoints(edges, y);
    double yc              = spectrum.AxisToParameter(1, y);
    int edgecount = edgepoints.size();
    edgecount--;		// Since .size() is unsigned.
    for (int i =0; i < edgecount; i+=2) {
      if (i+1 < edgepoints.size()) { // in case it's an odd # of edges.
	int xmin = edgepoints[i];
	int xmax = edgepoints[i+1];
	for (int x = xmin; x <= xmax; x++) {
	  double xc        = spectrum.AxisToParameter(0, x);
	  UInt_t    coords[2] = {x, y};
	  double counts    = spectrum[coords];
	  
	  sum   += counts;
	  wxsum += xc*counts;
	  wysum += yc*counts;
	  sqxsum += xc*xc*counts;
	  sqysum += yc*yc*counts;
	}
      }
    }
  }
  // Clean up the edges:

  for (int i=0; i < edges.size(); i++) {
    delete edges[i];
  }

  // Calculate the final statistics, and create the result list:

  if (sum == 0.0 ) {
    interp.setResult("Integration area is 0");
    return TCL_ERROR;
    
  }
  else {
    
    double xCentroid = wxsum/sum;
    double xFwhm     = sqrt(sqxsum/sum - xCentroid*xCentroid)*fwhmgamma;
    
    double yCentroid = wysum/sum;
    double yFwhm     = sqrt(sqysum/sum - yCentroid*yCentroid)*fwhmgamma;
    
    
    
    CTCLObject result;
    CTCLObject centroids;
    CTCLObject fwhms;
    result.Bind(interp);
    centroids.Bind(interp);
    fwhms.Bind(interp);
    
    centroids += xCentroid;
    centroids += yCentroid;
    
    fwhms     += xFwhm;
    fwhms     += yFwhm;
    
    result += centroids;
    result += sum;
    result += fwhms;
    
    interp.setResult(result);
  }
  return TCL_OK;
}
