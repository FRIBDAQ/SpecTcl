//  CContour.cpp
// Describes a contour gate.  The gate is
// represented internally in a manner which makes
// it efficient to lookup the result of the gate.
// This is not space efficient, although some
//  optimizations have been done.
//  1. m_Lower  - is the lowest legal value
//                        for the y parameter.
//  2. m_Upper  - Is the highest legal value for
//                        the y parameter.
//   Now let p = rEvent[m_nYid] - m_Upper.
// 3. m_aInterior is a vector of structs.  Each struct contains:
//           m_LowX         - Lowest valid X parameter for m_aInterior[p]
//           m_HiX            - Highest valid X parameter for m_aInterior[p]
//           m_vOk           - Bit mask vector where bit number m_nXid is
//                                   set if in the interior of the contour.
//  All of this is implemented in the internal class:  CInterior
//
//  Note that CInterior allows for arbitrarily shaped contours.. with holes,
//  and multilobed regions.  Construction time uses interior fill algorithms
//  to construct the interior from the point array.

//
//   Author:
//      Ron Fox
//      NSCL
//      Michigan State University
//      East Lansing, MI 48824-1321
//      mailto:fox@nscl.msu.edu
//
//////////////////////////.cpp file/////////////////////////////////////////////////////

//
// Header Files:
//


#include "Contour.h"                               
#include <algorithm>
#include <histotypes.h>
#include <iostream.h>

static const char* Copyright = 
"CContour.cpp: Copyright 1999 NSCL, All rights reserved\n";


// Local data types:

struct Edge {
  Float_t xnow;
  Float_t ylast;
  Float_t xadjust;
  Edge(Float_t x1 = 0.0, Float_t yl = 0.0, Float_t invslope = 0.0) :
    xnow(x1),
    ylast(yl),
    xadjust(invslope) {}
  
};


typedef vector<Edge> EdgeList, *pEdgeList;

struct EdgeTable {
  UInt_t nLines;
  UInt_t nLowline;
  vector<EdgeList> Edges;
};

// Debugging output functions:
//
ostream& operator<<(ostream& of, const Edge& rEdge) {
  of << " xnow= " << rEdge.xnow << " ylast = " << rEdge.ylast;
  of << " xadjust = " << rEdge.xadjust << endl;

  return of;
}

ostream& operator<<(ostream& of, EdgeList& rList) {
  of << "Edge list with " << rList.size() << " elements\n";
  for(UInt_t i = 0; i < rList.size(); i++) {
    of << "Edge " << i << endl;
    of << rList[i] << endl;
  }
  return of;
}
ostream& operator<<(ostream& of, EdgeTable& rTbl) {
  of << "-------------- Edge table -----------" << endl;
  of << " nLines = "   << rTbl.nLines;
  of << " nLowline = " << rTbl.nLowline << endl;
  of << rTbl.Edges.size() << " edges follow: \n";
  for(UInt_t i = 0; i < rTbl.Edges.size(); i++) {
    of << "Table " << i << rTbl.Edges[i] << endl;
  }
  return of;
}

//

class CXnowCompare {
public:
  int operator()(const Edge& p1, const Edge& p2) {
    return p1.xnow < p2.xnow;
  }
};
class YCompare {
public:
  int operator()(const CPoint& p1, const CPoint& p2) {
    return (p1.Y() < p2.Y());
  }
};
class XCompare {
public:
  int operator()(const CPoint& p1, CPoint& p2) {
    return (p1.X() < p2.X());
  }
};


// Open functions:
/////////////////////////////////////////////////////////////////////////
//
// Function:
//   SortEdgeList(EdgeList& rEdges)
// 
static void
SortEdgeList(EdgeList& rEdges)
{
  // Sort the resulting edge list by xnow.
  
  CXnowCompare c;
  
  sort(rEdges.begin(), rEdges.end(), c);
  
}

// DeleteEdge - Removes an edge from an edgelist:

static inline void
DeleteEdge(EdgeList& rlist, EdgeList::iterator& which) {
  rlist.erase(which);
}

// Add Edge - Adds an edge to an existing edge list.  The addition
//            keeps the list sorted by increasing xnow.
//

static void
AddEdge(EdgeList& rlist, Edge edge)
{
  EdgeList::iterator p;
  for(p = rlist.begin(); p!= rlist.end(); p++) {
    if(edge.xnow > (*p).xnow) {
      rlist.insert(p, edge);
      return ;
    }
  }
  rlist.push_back(edge);
}

// MakeEdge  - Creates an edge given a pair of points.  Returns the
//             y coordinate of the bottom point.
//
static int 
MakeEdge(Edge& rEdge, CPoint p1, CPoint p2) 
{

  // Set the lower and higer points appropriately..

  CPoint low, hi;
  if(p1.Y() < p2.Y()) {
    low = p1;
    hi  = p2;
  }
  else {
    low = p2;
    hi  = p1;
  }

  // Compute slope inverse, and construct the edge:

  Float_t x0 = (Float_t) low.X();
  Float_t x1 = (Float_t) hi.X();
  Float_t y0 = (Float_t) low.Y();
  Float_t y1 = (Float_t) hi.Y();

  rEdge = Edge(x0,y1, (x1-x0)/(y1-y0));
  return low.Y();
  
}

// UpdateEdgeList - Updates an edge list: All used up edges are thrown
//                   out, new xnow's are copmuted for active edges.
//                   (Assumption - this is called to go to the next highest
//                    scanline).
//
static void
UpdateEdgeList(EdgeList& rList, UInt_t y)
{
  Float_t fY = (Float_t)y;

  // Update the entire edge list, deleting all 'used up edges'.

  EdgeList::iterator i;
  for(i = rList.begin(); i < rList.end();) {
    (*i).xnow += (*i).xadjust;
    if( (*i).ylast <= y) {
      rList.erase(i);
    } else {
      i++;
    }
  }
    

  SortEdgeList(rList);
}
// MergeEdgeTable - Merges new edges into an active edge list from
//                  a complete polygon edgelist.
//
static void
MergeEdgeTable(EdgeList& rActive, EdgeTable& rTable, UInt_t nScan)
{
  EdgeList& rNewEdges(rTable.Edges[nScan]); // Edge list to add...

  for(UInt_t i = 0; i < rNewEdges.size(); i++) {
    AddEdge(rActive, rNewEdges[i]); // Maintains sortedness.
  }
  SortEdgeList(rActive);
}
// MakeEdgeTable Make a new edge table from a polygon of points.
//
static void
MakeEdgeTable(EdgeTable& rTable, vector<CPoint>& rPolygon)
{
  YCompare c;
  vector<CPoint>::iterator plow = min_element(rPolygon.begin(),
					      rPolygon.end(), c);
  vector<CPoint>::iterator phi  = max_element(rPolygon.begin(),
					      rPolygon.end(), c);

  int yh = (*phi).Y();
  int yl = (*plow).Y();
  //
  // set up the edge table (it's assumed empty).
  //
  rTable.nLowline = yl;
  rTable.nLines  = yh - yl + 1;
  EdgeList  aList;
  rTable.Edges.insert(rTable.Edges.begin(), // Stock the edge table.
		      (Int_t)rTable.nLines, aList);
  //
  // for each pair of non-horizontal points in the polygon an edge
  // is added to the edgetable at its starting scan-line.
  //
  vector<CPoint>::iterator p1 = rPolygon.begin();
  vector<CPoint>::iterator p2 = p1; p2++;

  while(p2 != rPolygon.end()) {
    if((*p2).Y() != (*p1).Y()) {
      Edge e;
      int y0 = MakeEdge(e, *p1, *p2);
      AddEdge(rTable.Edges[y0-yl], e);
    }
    p1 = p2;
    p2++;
  }
  // Lastly, the polygon is closed:
  p2 = rPolygon.begin();
  if((*p2).Y() != (*p1).Y()) {
    Edge e;
    int y0 = MakeEdge(e, *p1, *p2);
    AddEdge(rTable.Edges[y0-yl], e);
  }

#ifdef __DEBUG__
  cout << rTable;
#endif


}
// functions for class CContour

//////////////////////////////////////////////////////////////////////////
// 
// Function:
//   CContour(UInt_t nXId, UInt_t nYId, 
//  	      vector<CPoint>& Points)
// Operation Type:
//   Constructor.
//
CContour::CContour(UInt_t nXId, UInt_t nYId, const  vector<CPoint>& Points) :
  CPointListGate(nXId, nYId, Points)
{
  GenerateInterior();
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CContour (UInt_t nXId, UInt_t NYId, UInt_t nPts,
// 	        UInt_t *xCoords, UInt_t *yCoords)
// Operation type:
//    Construction:
//
CContour::CContour (UInt_t nXId, UInt_t nYId, UInt_t nPts,
		    UInt_t *xCoords, UInt_t *yCoords) :
  CPointListGate(nXId, nYId, nPts, xCoords, yCoords)
{
  GenerateInterior();
}
//////////////////////////////////////////////////////////////////////////
//
// Function:
//    CContour(UInt_t nXId, UInt_t NYId, UInt_t nPts,
//	       CPOint* pPonts)
// Operation Type:
//    Constructor
//
CContour::CContour (UInt_t nXId, UInt_t nYId, UInt_t nPts,
		    CPoint* pPoints) :
  CPointListGate(nXId, nYId, nPts, pPoints)
{
  GenerateInterior();
}

Bool_t
CContour::operator()(CEvent& rEvent)
{
  if(!wasChecked()) {
    Set(inGate(rEvent));
  }
  return getCachedValue();
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    Bool_t inGate ( CEvent& rEvent, vector<UInt_t>& Params )
//  Operation Type:
//     Evaulator
//
Bool_t
CContour::inGate(CEvent& rEvent, const vector<UInt_t>& Params)
// Evaluates the gate.  This determines
// if the parameter has made the gate.
// Note that if there is a cached evaulation
// we use that instead.  Otherwise the gate is
// evaluated and cached for future use.
//
// Formal Parameters:
//     CEvent& rEvent:
//           The event against which the gate is being checked.
//     vector<UInt_t>& Params
//           The vector of params in the gate (empty)
//
// Exceptions:  
{
  return inGate(rEvent);
}
// Parameterless inGate:
Bool_t
CContour::inGate(CEvent& rEvent)
{
  UInt_t nx = getxId();
  UInt_t ny = getyId();
  if((nx < rEvent.size()) && (ny < rEvent.size())) {
    if(rEvent[nx].isValid() && rEvent[ny].isValid()) {
      Int_t x = rEvent[nx] - getLowerLeft().X();
      Int_t y = rEvent[ny] - getLowerLeft().Y();
      if((x < 0) || (y < 0)) {
	return kfFALSE;
      }
      else {
	return (getInterior().Inside(x, y));
      }
    }
    else {
      return kfFALSE;
    }
  }
  return kfFALSE;
}

//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    CGate* clone (  )
//  Operation Type:
//     Virtual construction
//
CGate* 
CContour::clone() 
{
// Constructs a copy of this gate.  
// This must eventually be explicitly deleted.
//
// Exceptions:  

  return new CContour(*this);
}
//////////////////////////////////////////////////////////////////////////
//
//  Function:   
//    std::string Type (  ) const
//  Operation Type:
//     Selector
//
std::string 
CContour::Type() const
{
// Return the gate type which in this
// case is "c" for contour.
//
// Exceptions:  

  return std::string("c");
}
///////////////////////////////////////////////////////////////////////
//
// Function:
//    GenerateInterior()
// Operation Type:
//    Utility (initialization)
//
void
CContour::GenerateInterior()
{
  //  First figure out the corners of the interior region,
  //  from that, we'll be able to size the interior, as well as set the
  //  corner members.
  //
  vector<CPoint> Polygon = getPoints();
  vector<CPoint>::iterator p;
  YCompare yc;
  XCompare xc;

  p = min_element(Polygon.begin(), Polygon.end(), yc);
  UInt_t yl = (*p).Y();
  p = max_element(Polygon.begin(), Polygon.end(),  yc);
  UInt_t yh = (*p).Y();

  p = min_element(Polygon.begin(), Polygon.end(), xc);
  UInt_t xl = (*p).X();
  p = max_element(Polygon.begin(), Polygon.end(), xc);
  UInt_t xh = (*p).X();
  m_LowerLeft   = CPoint(xl, yl);
  m_UpperRight  = CPoint(xh, yh);

  // Relocated all of the points so that the origin is relative to
  // lower left:

  for(p = Polygon.begin(); p < Polygon.end(); p++) {
    CPoint pt = *p;
    pt = CPoint(pt.X() - xl,
		pt.Y() - yl);
    *p = pt;
  }

  // Set the size of the interior and clear it:
  //
  m_Interior.setShape((yh - yl + 2), (xh - xl + 2));
#ifdef __DEBUG__
  cout << "Rows = " << yh-yl+2;
  cout << " Cols = " << xh-xl+2 << endl;
#endif
  m_Interior.Clear();
  //
  // Next make the edge table and use it to set the points which
  // are in the interior.
  //
  EdgeTable Table;		// Full edge table.
  EdgeList Active;		// Active edge list.
  MakeEdgeTable(Table, Polygon); 

  // Loop over all scan lines represented by the edge table.
  // For each loop pass, merge new entries into the edge table,
  // Figure out which pixels are interior in that scan line and
  // add them to our interior member.

  int y = 0;
  for(int l = 0; l < Table.Edges.size(); l++) {
    MergeEdgeTable(Active, Table, l); // Merge in new edges.
    EdgeList::iterator p1;
    EdgeList::iterator p2;
    p1 = p2 = Active.begin();
    p2++;
    while(p1 != Active.end()) {
      UInt_t x1 = (UInt_t)(*p1).xnow;
      UInt_t x2 = (UInt_t)(*p2).xnow;
      for(UInt_t i = x1; i <= x2; i++) {
	m_Interior.Set(i ,y, kfTRUE);
      }
      p1++;			// This region is outside.
      p2++;

      p1++;			// This gets us back to an inside region.
      p2++;
    }
    // On to the next scan line: 

    y++;
    UpdateEdgeList(Active, y);    // Retire old edges.
    
  }
#ifdef __DEBUG__
  cout << "Interior bitmap: ";
  cout << m_Interior;
  cout << endl;
#endif

}
