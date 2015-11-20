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

static const char* Copyright = "(C) Copyright Michigan State University 1994, All rights reserved";
/*
** Facility:
**   Xamine -- NSCL display program.
** Abstract:
**   integrate.cc:
**      This file implements the class methods for the spectrum integrators.
**      At present, the only thing implemented here are various flavors of
**      the Perform function which computes the stuff needed to derive 
**      the statistical measures that we need to know.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
*/


/*
** Include files:
*/
#include <config.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>

#include <memory.h>
#include "integrate.h"
#include "dispshare.h"
#include "dispgrob.h"

using namespace std;

/*
** Some compilers such as ultrix have problems including both math.h
** and limits.h
** Therefore we assume that maxint is for a 32 bit system and:
*/
#ifndef INT_MAX
#define INT_MAX  0x7fffffff
#endif
/*
** External references:
*/
extern spec_shared *xamine_shared; /* shared memory region. */

/*
** Local types:
*/

struct edge_descriptor {
                         float xnow;       /* Current X position of edge. */
			 float ylast;      /* Actually end of scan.       */
			 float xadjust;	   /* Amount X changes by per y+1 */
		       };
struct edge_list  {
                    int num_edges;         /* number of edge descriptors. */
		    edge_descriptor bases[GROBJ_MAXPTS * 2]; /* List of them */
                  };
typedef edge_list *edge_list_pointer;
struct edge_table {
                   int scan_lines;         /* # scan lines represented.   */
		   int ybase;	           /* Lowest scan line represented */
		   edge_list_pointer  *edges;      /* List of edge tables.         */
		 };


// inline utility to determin if a point is inside limits:

bool inside(int pt, int low, int high)
{
  return (pt >= low) && (pt <= high);
}


/*
** Functional Description:
**   DeleteEdge:
**     This function deletes a specified edge from the edge list
** Formal Parameters:
**   edge_list *e:
**     Pointer to the edge list.
**   int l:
**     Index of the edge to delete.
*/
static void DeleteEdge(edge_list *e, int l) 
{
  e->num_edges--;
  for(int i = l; i < e->num_edges; i++) {
    memcpy(&(e->bases[i]), &(e->bases[i+1]), sizeof(edge_descriptor));
  }
}

/*
** Functional Description:
**   AddEdge:
**     This function adds an edge to an existing edge list.  The add is done
**     in a manner which keeps the edge entries sorted by xnow.
**     At present this addition is an insertion sort. which should be good
**     enough since there are typically very few edges in an edge_list for
**     a polygon (2 is typical).
** Formal Parameters:
**     edge_list *e:
**       Points to the edge list that we're adding to.
**     edge_descriptor *ed:
*        Describes the edge that we're adding.
*/
static void AddEdge(edge_list *e, edge_descriptor *ed) 
{
  int i = e->num_edges;
  e->num_edges++;

  while(i > 0) {
    if(ed->xnow >  e->bases[i-1].xnow) {
      memcpy(&(e->bases[i]), ed, sizeof(edge_descriptor));
      return;
    }
    memcpy(&(e->bases[i]), &(e->bases[i-1]), sizeof(edge_descriptor));
    i--;
  }
  memcpy(&(e->bases[0]), ed, sizeof(edge_descriptor));
      
}

/*
** Functional Description:
**   MakeEdge:
**      This function creates an edge from a pair of points on a polygon region.
** Formal Parameters:
**   edge_descriptor *e:
**     Points to the buffer that we will fill in with an edge descriptor.
**   grobj_point *pt1, *pt2:
**     Points to the pair of points which define the edge.
** Returns:
**     int:
**       The lowest y coordinat of the edge.
*/
static int MakeEdge(edge_descriptor *e, grobj_point *pt1, grobj_point *pt2)
{

  /* We want (x0,y0)-(x1,y1) to be the line segment with smallest y coordinate
  ** in (x0,y0)
  */
  float y0,y1,x0,x1;
  x0  = (float)pt1->getx();
  x1  = (float)pt2->getx();
  y0  = (float)pt1->gety();
  y1  = (float)pt2->gety();
  if(y0 > y1) {
    float t;
    t  = x0;
    x0 = x1;
    x1 = t;
    t  = y0;
    y0 = y1;
    y1 = t;
  }
  /* Compute the slope inverse... we are gaurenteed by checks in the caller that 
  ** y1 <> y2.
  */
  e->xnow = x0;			/* We are now at the bottom. */
  e->ylast= y1;			/* y1 is the top y coordinate. */
  e->xadjust = (x1-x0)/(y1-y0);

  /* By our definition of y0, y0 is the return value of the function too.
  */
  return (int)y0;
  
}

/*
** Functional Description:
**   Destroy<EdgeTable:
**     This local function releases the storage which is taken up by an
**     edge table.
** Formal Parameters:
**   edge_table *t:
**     Points to the edge table.
*/
static void DestroyEdgeTable(edge_table *e)
{
  for(int i = 0; i < e->scan_lines; i++) {
    if(e->edges[i] != NULL) delete e->edges[i];	/* Get rid of nonempty lists. */
    e->edges[i] = NULL;
  } 
  delete e->edges;		/* Get rid of the pointer array. */
  delete e;			/* Get rid of the table root structure. */

}    

/*
** Functional Description:
**   UpdateEdgeTable:
**     This function udpates an edge table.  This involves throwing out all
**     edges which are 'used up' and computing new xnow values for the
**     edges that are still active.  The assumption is that y is changing
**     by +1.
** Formal Parameters:
**     edge_list *e:
**        List of edges to update.
**     int y:
**        Current y value.
*/
static void UpdateEdgeTable(edge_list *e, int y)
{
  /* Loop over all the entries in the edge list, updating and sorting. */
  /* Then whip through the sorted list and eliminate inactive edges.   */

  int i;
  for(i = 0; i < e->num_edges; i++) { 
    edge_descriptor ed;
    memcpy(&ed, &(e->bases[i]), sizeof(edge_descriptor));
    DeleteEdge(e, i);		/* Deleting, recalculating and */
    ed.xnow += ed.xadjust;	/* adding again ensures that the updated */
    AddEdge(e, &ed);		/* edge list is sorted. */
  }
  
  
  for(i = 0; i < e->num_edges;) {
    if(e->bases[i].ylast <= y) {
      DeleteEdge(e, i);		/* Remove the edge from the list. */
				/* i should not be adjusted because */
				/* this item will be deleted and become */
				/* what was the next item	*/
    }
    else {
	i++;			/* Look at next edge in table. */
    }
  }
  
}

/*
** Functional Description:
**   MergeEdgeTable:
**      This function merges an active edge table with the corresponding
**      scan line edge list in a polygon edge table.
**  
** Formal Parameters:
**    edge_list *active:
**      Points to the edge list which constitutes the active edge table.
**    edge_table *table:
**      Points to the polygon edge table.
**    int sl:
**      Scan line relative to start of table to merge.
*/
static void MergeEdgeTable(edge_list *active, edge_table *table, int sl)
{
  /* Use sl to make the edge list pointer: */

  edge_list *src;
  src = table->edges[sl];
  if(src == NULL) return;	/* Done if there are no edges. */

  /* Loop over the set of edges in src and add them one by one to the
  ** edges active:
  */
  for(int i = 0; i < src->num_edges; i++)
    AddEdge(active, &(src->bases[i]));

}

/*
** Functional Description:
**   MakeEdgeTable:
**      This function makes the edge table for 2-d integration.
**      An edge table describes the polygon edges completely.
**      Each edge is described by the following:
**            xnow     : The x position at the vertex with the smallest y val.
**            ylast    : The y coordinat of the upper vertex.
**            xadjust  : The change in x coordinate for a change of +y in y
**                       (1/slope).
**      The edge table consists of a set of scan line descriptors.  Each
**      scan line descriptor is NULL if there are no edges which originate
**      inside that scan line or an edge_list consisting of the set of edges
**      which do originate inside the scan line.  The edge table represents
**      only a subset of the spectrum.  In particular it represents that
**      subset which runs from the lowest y coordinate to the highest y
**      coordinate of all the edges.
** Formal Parameters:
**    grobj_generic *polygon:
**       Describes the polygon for which we're building the edge table.
** Returns:
**    A pointer to the edge table.
*/
static edge_table *MakeEdgeTable(grobj_generic *polygon) 
{
  /* First we locate the lower and upper limits of the polygon: */

  int yh = 0;
  int yl = INT_MAX;

  grobj_point *pt1 = polygon->firstpt();
  while(pt1 != NULL) {
    int y = pt1->gety();
    if(y > yh) yh = y;
    if(y < yl) yl = y;
    pt1 = polygon->nextpt();
  }
  // Limit yl,yh to spectrum boundaries: 
  // Now the edge table will be inside the spectrum.



  /* Next allocate the root of the edge table and the edge list pointers: */
  /* We initialize all of the edges pointers to null.                     */

  edge_table *tbl = new edge_table;
  tbl->scan_lines = yh - yl + 1;
  tbl->ybase      = yl;
  tbl->edges      = new edge_list_pointer[tbl->scan_lines];
  for(int i = 0; i < tbl->scan_lines; i++) {
    tbl->edges[i] = NULL;
  }
  /* Now we construct the edge table entries for each line segment. 
  ** we must close the polygon ourself by mating the last point with the first.
  ** We assume that the polygon has at least two points.  Note that if both
  ** points are on a horizontal line, then the polygon has no interior.
  ** Note also that in Xamine anyway, the graphical object size is enforced
  ** by the acceptance routines.
  */

  pt1               = polygon->firstpt();
  grobj_point *pt2  = polygon->nextpt();
  edge_descriptor edge;
  int y0;
  while(pt2 != NULL) {
    if(pt1->gety() != pt2->gety()) { /* Only add non horizontal edges. */
      y0 = MakeEdge(&edge, pt1, pt2);	/* Make an edge table entry. */
      
      /* If this is the first entry on the scan line, we need to create the
       ** edgelist and initialize it.
       */
      if(tbl->edges[y0-yl] == NULL) {
	tbl->edges[y0-yl]  = new edge_list;
	tbl->edges[y0-yl]->num_edges = 0;
      }
      AddEdge(tbl->edges[y0-yl], &edge); /* Add the edge to the edge list. */
    }
    pt1 = pt2;
    pt2 = polygon->nextpt();

  }
    /* Now we must close the polygon by going one more time with pt2 = first pt
     */

  pt2 = polygon->firstpt();
  if(pt1->gety() != pt2->gety()) {
    y0  = MakeEdge(&edge, pt1, pt2);
    
    /* If this is the first entry on the scan line, we need to create the
     ** edgelist and initialize it.
     */
    if(tbl->edges[y0-yl] == NULL) {
      tbl->edges[y0-yl]  = new edge_list;
      tbl->edges[y0-yl]->num_edges = 0;
    }
    AddEdge(tbl->edges[y0-yl], &edge); /* Add the edge to the edge list. */ 
  }

  /* Return the edge table to the caller: */

  return tbl;
}

/*
** Functional Description:
**   Integrate_1dw::Perform:
**     This function performs a 1-d integration of a word spectrum.
**     The centroid and std-deviation from the mean position are stored in 
**     the centroid and deviance attributes of the object.
*/
void Integrate_1dw::Perform()
{
  /* First figure out the high and low limit.  */

  int lolim, hilim;
  grobj_point *pt = sumregion->firstpt();
  lolim = pt->getx();
  pt              = sumregion->nextpt();
  hilim = pt->getx();
  if(hilim < lolim) {
    int temp = hilim;
    hilim    = lolim;
    lolim    = temp;
  }
  // Limit to spectrum boundaries.

  int specid = sumregion->getspectrum();
  int spec_xmax   = xamine_shared->getxdim(specid);

  lolim = max(0,lolim);
  hilim = max(0,hilim);
  lolim = min(lolim, spec_xmax);
  hilim = min(hilim, spec_xmax);

  /* Hoist the spectrum base pointer to allow efficient pointer usage in the
  ** summing pass..
  */
  unsigned short *s = &(spectrum[lolim]);

  /* Do the sum. */

  double  sum     = 0.0;
  double  chansum = 0.0;
  double  sqsum   = 0.0;
  double  chan;
  for(int i = lolim; i <= hilim; i++) {
    chan = *s++;
    sum += chan;
    chan *= Channel(i);		/* Weight the channel. */
    chansum += chan;
    sqsum   += chan*Channel(i);		/* Weight channel**2 by height. */
  }

  /* Use the sums to produce the centroid and stddev: */

  if(sum > 0.0) {
    volume   = sum;
    centroid = chansum/sum;
    deviance = (sqsum)/sum - (centroid*centroid);
    if(deviance < 0) deviance = 0;
    deviance = sqrt(deviance);
  }
  else {
    volume   = 0.0;
    centroid = 0.0;
    deviance = 0.0;
  }

}

/*
** Functional Description:
**   Integrate_1dl::Perform:
**     This function performs a 1-d integration of a longword spectrum.
**     The centroid and std-deviation from the mean position are stored in 
**     the centroid and deviance attributes of the object.
*/
void Integrate_1dl::Perform()
{
  /* First figure out the high and low limit.  */

  int lolim, hilim;
  grobj_point *pt = sumregion->firstpt();
  lolim = pt->getx();
  pt              = sumregion->nextpt();
  hilim = pt->getx();
  if(hilim < lolim) {
    int temp = hilim;
    hilim    = lolim;
    lolim    = temp;
  }
  // Limit to spectrum boundaries.

  int specid = sumregion->getspectrum();
  int spec_xmax   = xamine_shared->getxdim(specid);

  lolim = max(0,lolim);
  hilim = max(0,hilim);
  lolim = min(lolim, spec_xmax);
  hilim = min(hilim, spec_xmax);

  /* Hoist the spectrum base pointer to allow efficient pointer usage in the
  ** summing pass..
  */
  unsigned int *s = (unsigned int *)&spectrum[lolim];

  /* Do the sum. */

  double  sum     = 0.0;
  double  chansum = 0.0;
  double  sqsum   = 0.0;
  double  chan;
  for(int i = lolim; i <= hilim; i++) {
    chan = *s++;
    sum += chan;
    chan *= Channel(i);		/* Weight the channel. */
    chansum += chan;
    sqsum   += chan*Channel(i);		/* Weight channel**2 by height. */
  }

  /* Use the sums to produce the centroid and stddev: */

  if(sum > 0.0) {
    volume   = sum;
    centroid = chansum/sum;
    deviance = sqsum/sum - (centroid*centroid);
    if(deviance < 0) deviance = 0;
    deviance = sqrt(deviance);
  }
  else {
    volume   = 0.0;
    centroid = 0.0;
    deviance = 0.0;
  }


}

/*
** Functional Description:
**   Integrate_2db:
**     Integrates a 2-d byte spectrum.  The interior of the region is
**     found using an algorithm developed by Backer Foley and VanDam and
**     described on pp 105-111 of _Computer_Graphics_Systems_and_Concepts
**     Salmon and Slater Addison Wesley.
*/
void Integrate_2db::Perform()
{
  /* First produce the edge table:  */

  edge_table *e  = MakeEdgeTable(sumregion);

  /*  The first line scan line by definition must have edge descriptions in
  **  it.  These are also sorted.  Therefore we produce the active edge table
  **  from it:
  */

  edge_list active;
  active.num_edges = 0;		/* Initially empty will be merged in loop. */

  /* Get the y limits of the scan and produce some easy access pointers to
  ** the spectrum:
  */
  int nlines             = e->scan_lines;	/* Loop pass count. */
  unsigned char *line    = spectrum + (e->ybase * xchans);  /* base of scan */

  /* Now loop over all the scan lines represented by the edge table:      */
  /* In each pass of the loop, pairs of points in the active edge table   */
  /* define integration limits.  After each loop pass, dead line segments */
  /* are eliminated and new ones merged in.                               */
  double sum    = 0;
  double xsum   = 0;
  double ysum   = 0;
  double xsqsum = 0;
  double ysqsum = 0;
  int  y       = e->ybase;	/* Keep track of y coord. */

  for(int l = 0; l < nlines; l++) { /* Loop over scan lines. */
    MergeEdgeTable(&active, e, l); /* Merge with next scan line.       */
    for(int j = 0; j < active.num_edges; j += 2) { /* scan over all pairs  */
      int lo = (int)active.bases[j].xnow;
      int hi = (int)active.bases[j+1].xnow; /* sum limits. */
      unsigned char *s = line + lo;
      for(int k = lo; k <= hi; k ++) { /* Sum over one interior region. */
	double  xchannel = XChannel(k);
	double  ychannel = YChannel(y);
	float   ch       = (float)*s++;
	sum     += ch;
	xsum    += ch * xchannel;
	ysum    += ch * ychannel;
	xsqsum  += ch * (xchannel*xchannel);
	ysqsum  += ch * (ychannel*ychannel);

      }				/* Sum over one interior region. */
    }				/* Scan over all pairs.          */
    /* Now on to the next scan line:  */

    y++;
    UpdateEdgeTable(&active, y); /* Update contents of current edge tbl. */
    line += xchans;
  }

  /* Now that we've summed over the interior region, we can compute the */
  /* desired quantities for the integrator.                             */

  if(sum > 0.0) {
    volume    = sum;
    xcentroid = xsum / sum;
    ycentroid = ysum / sum;
    xdeviance = xsqsum/sum - xcentroid*xcentroid;
    if(xdeviance < 0) xdeviance = 0;
    xdeviance = sqrt(xdeviance);
    ydeviance = ysqsum/sum - ycentroid*ycentroid;
    if(ydeviance < 0) ydeviance = 0;
    ydeviance = sqrt(ydeviance);
  }
  else {
    volume    = 0.0;
    xcentroid = 0.0;
    ycentroid = 0.0;
    xdeviance = 0.0;
    ydeviance = 0.0;
  }

  /* Get rid of the dynamic storage associated with the edge table. */

  DestroyEdgeTable(e);
}

/*
** Functional Description:
**   Integrate_2dw:
**     Integrates a 2-d word spectrum.  The interior of the region is
**     found using an algorithm developed by Backer Foley and VanDam and
**     described on pp 105-111 of _Computer_Graphics_Systems_and_Concepts
**     Salmon and Slater Addison Wesley.
*/
void Integrate_2dw::Perform()
{
  /* First produce the edge table:  */

  edge_table *e  = MakeEdgeTable(sumregion);

  /*  The first line scan line by definition must have edge descriptions in
  **  it.  These are also sorted.  Therefore we produce the active edge table
  **  from it:
  */

  edge_list active;
  active.num_edges = 0;		/* Initially empty will be merged in loop. */

  /* Get the y limits of the scan and produce some easy access pointers to
  ** the spectrum:
  */
  int nlines             = e->scan_lines;	/* Loop pass count. */
  unsigned short *line    = spectrum + (e->ybase * xchans);  /* base of scan */

  /* Now loop over all the scan lines represented by the edge table:      */
  /* In each pass of the loop, pairs of points in the active edge table   */
  /* define integration limits.  After each loop pass, dead line segments */
  /* are eliminated and new ones merged in.                               */
  double sum    = 0;
  double xsum   = 0;
  double ysum   = 0;
  double xsqsum = 0;
  double ysqsum = 0;
  int    spectrumid = sumregion->getspectrum();
  int    xdimension = xamine_shared->getxdim(spectrumid) - 1;
  int    ydimension = xamine_shared->getydim(spectrumid) - 1;

  int  y       = e->ybase;	/* Keep track of y coord. */

  for(int l = 0; l < nlines; l++) { /* Loop over scan lines. */
    MergeEdgeTable(&active, e, l); /* Merge with next scan line.       */
    for(int j = 0; j < active.num_edges; j += 2) { /* scan over all pairs  */
      int lo = (int)active.bases[j].xnow;
      int hi = (int)active.bases[j+1].xnow; /* sum limits. */
      unsigned short *s = line + lo;
      for(int k = lo; k <= hi; k ++) { /* Sum over one interior region. */
	double xchannel = XChannel(k);
	double ychannel = YChannel(y);
	if (inside(k, 0, xdimension) && inside(y, 0, ydimension)) {
	  float  ch       = (float)*s++;
	  sum     += ch;
	  xsum    += ch * xchannel;
	  ysum    += ch * ychannel;
	  xsqsum  += ch * (xchannel*xchannel);
	  ysqsum  += ch * (ychannel*ychannel);
	} else {
	  s++;
	}
      }				/* Sum over one interior region. */
    }				/* Scan over all pairs.          */
    /* Now on to the next scan line:  */

    y++;
    UpdateEdgeTable(&active, y); /* Update contents of current edge tbl. */
    line += xchans;
  }

  /* Now that we've summed over the interior region, we can compute the */
  /* desired quantities for the integrator.                             */

  if(sum > 0.0) {
    volume    = sum;
    xcentroid = xsum / sum;
    ycentroid = ysum / sum;
    xdeviance = (xsqsum/sum) - xcentroid*xcentroid;
    if(xdeviance < 0) xdeviance = 0;
    xdeviance = sqrt(xdeviance);
    ydeviance = (ysqsum/sum) - ycentroid*ycentroid;
    if(ydeviance < 0) ydeviance = 0;
    ydeviance = sqrt(ydeviance);
  }
  else {
    volume    = 0.0;
    xcentroid = 0.0;
    ycentroid = 0.0;
    xdeviance = 0.0;
    ydeviance = 0.0;
  }


  /* Get rid of the dynamic storage associated with the edge table. */

  DestroyEdgeTable(e);
}
// Integrate for a 2d longword. See the above functions for call conventions.

void
Integrate_2dl::Perform()
{
  /* First produce the edge table:  */

  edge_table *e  = MakeEdgeTable(sumregion);

  /*  The first line scan line by definition must have edge descriptions in
  **  it.  These are also sorted.  Therefore we produce the active edge table
  **  from it:
  */

  edge_list active;
  active.num_edges = 0;		/* Initially empty will be merged in loop. */

  /* Get the y limits of the scan and produce some easy access pointers to
  ** the spectrum:
  */
  int nlines             = e->scan_lines;	/* Loop pass count. */
  unsigned int *line    = spectrum + (e->ybase * xchans);  /* base of scan */

  /* Now loop over all the scan lines represented by the edge table:      */
  /* In each pass of the loop, pairs of points in the active edge table   */
  /* define integration limits.  After each loop pass, dead line segments */
  /* are eliminated and new ones merged in.                               */
  double sum    = 0;
  double xsum   = 0;
  double ysum   = 0;
  double xsqsum = 0;
  double ysqsum = 0;
  int    spectrumid = sumregion->getspectrum();
  int    xdimension = xamine_shared->getxdim(spectrumid) - 1;
  int    ydimension = xamine_shared->getydim(spectrumid) - 1;


  int  y       = e->ybase;	/* Keep track of y coord. */

  for(int l = 0; l < nlines; l++) { /* Loop over scan lines. */
    MergeEdgeTable(&active, e, l); /* Merge with next scan line.       */
    for(int j = 0; j < active.num_edges; j += 2) { /* scan over all pairs  */
      if (j+1 < active.num_edges) {		   // Deal with stuff coming to a point.
	int lo = (int)active.bases[j].xnow;
	int hi = (int)active.bases[j+1].xnow; /* sum limits. */
	unsigned int *s = line + lo;
	for(int k = lo; k <= hi; k ++) { /* Sum over one interior region. */
	  double xchannel = XChannel(k);
	  double ychannel = YChannel(y);

	  // Only sum points that are actually in the spectrum:

	  if (inside(k, 0, xdimension) && inside(y, 0, ydimension)) {

	    float  ch       = (float)*s++;
	    sum     += ch;
	    xsum    += ch * xchannel;
	    ysum    += ch * ychannel;
	    xsqsum  += ch * (xchannel*xchannel);
	    ysqsum  += ch * (ychannel*ychannel);
	  } else {
	    s++;
	  }
	}
      }				/* Sum over one interior region. */
    }				/* Scan over all pairs.          */
    /* Now on to the next scan line:  */

    y++;
    UpdateEdgeTable(&active, y); /* Update contents of current edge tbl. */
    line += xchans;
  }

  /* Now that we've summed over the interior region, we can compute the */
  /* desired quantities for the integrator.                             */

  if(sum > 0.0) {
    volume    = sum;
    xcentroid = xsum / sum;
    ycentroid = ysum / sum;
    xdeviance = (xsqsum/sum) - xcentroid*xcentroid;
    if(xdeviance < 0) xdeviance = 0;
    xdeviance = sqrt(xdeviance);
    ydeviance = (ysqsum/sum) - ycentroid*ycentroid;
    if(ydeviance < 0) ydeviance = 0;
    ydeviance = sqrt(ydeviance);
  }
  else {
    volume    = 0.0;
    xcentroid = 0.0;
    ycentroid = 0.0;
    xdeviance = 0.0;
    ydeviance = 0.0;
  }


  /* Get rid of the dynamic storage associated with the edge table. */

  DestroyEdgeTable(e);

}
