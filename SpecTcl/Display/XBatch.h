/*
** Facility:
**   X-11 object support for C++
** Abstract:
**  XBatch.h:
**   Many of the X-11 drawin primitives can be multiply buffered up to
**   some server specific maximum.  This file contains objects which build
**   classes that allow user un-aware batching of these primitives.
**   In addition to a constructor and destructor, each class has the
**   following methods:
**     1) draw  - adds a primitive to a batch list and, if the list is
**                full calls the appropriate X function to pass the draw
**                request on to the server.
**     2)flush  - requests the server to draw the current buffer of
**                draws.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**
**    @(#)XBatch.h	8.1 6/23/95 
*/

#ifndef _XBATCH_H
#define _XBATCH_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#define REQUEST_LIMIT   1024	/* Limit to keep memory use in check. */
/*
** The class below defines the base behavior for all of the batching
** drawing classes.  It is not intended to be used directly, but is a
** class to use to derive other classes.
*/

class XBatch {			/* Base Batched drawing class. */
 protected:
  int maxreqsize;		/* Largest request size. */
  int requestsize;		/* Size of one request */
  int numrequests;		/* Number of requests  */
  int spaceleft;		/* Amount of space left in current request. */
  Display *disp;		/* Convenience storage of display fcn. */
  Drawable win;   	        /* Convenience storage of drawable id. */
  GC       gc;			/* Convenience storage of graphic context. */  
  
  /* Private functions: */

  void addreq() {		/* Book-keeping associated with adding prim. */
    spaceleft -= requestsize;
    numrequests++;
    if(spaceleft < requestsize) flush();
  }
  void postflush() {		/* Resets book-keeping post flush */
    spaceleft   = maxreqsize;
    numrequests = 0;
  }
 public:
  XBatch(Display *d, Drawable w, GC c) {
    disp = d;
    win  = w;
    gc   = c;
    maxreqsize = XMaxRequestSize(disp);
    if(maxreqsize > REQUEST_LIMIT) maxreqsize = REQUEST_LIMIT;
    numrequests= 0;
    spaceleft  = maxreqsize;
    requestsize= 0;		/* Actually not known yet. */
  }
  virtual ~XBatch() {}
  virtual void flush() = 0;
};


/*
** XSegmentBatch  - This class derives from XBatch to build a class that
**                  batches disjoint line segment drawing using the
**                  function XDrawSegments()
*/
class  XSegmentBatch : public XBatch {
 protected:
  XSegment *segments;
  XSegment *insert;
 public:
  XSegmentBatch(Display *d, Drawable w, GC c) : XBatch(d,w,c) {
    requestsize = 2;		/* Longwords in a request. */
    segments = new XSegment[maxreqsize/requestsize];
    insert   = segments;
    maxreqsize -= 3;
  }
  virtual ~XSegmentBatch() {		/* Destructor... */
    flush();			/* Flush out buffered segments. */
    delete segments;		/* Delete segment storage. */
  }
  void draw(short x0, short y0, short x1, short y1) { /* Add a segment. */
    insert->x1 = x0;
    insert->y1 = y0;
    insert->x2 = x1;		/* Add segment to the list. */
    insert->y2 = y1;
    insert++;
    addreq();			/* Do book-keeping, flush if needed. */
  }
  virtual void flush() {
    if(numrequests > 0) {
      XDrawSegments(disp, win, gc, /* Draw the segments. */
		    segments, numrequests);
      insert = segments;	/* Reset the insertion pointer. */
      postflush();		/* Do standard post flush bookkeeping. */
    }
  }
};
/*
**  XLineBatch does a batch of connected line draws.
*/
class XLineBatch :  public XBatch {
 protected:
  XPoint *points;
  XPoint *insert;
 public:
  XLineBatch(Display *d, Drawable w, GC c) : XBatch (d,w,c) { /* Constructor. */
    requestsize = 2;
    points      = new XPoint[maxreqsize/requestsize];
    insert      = points;
    maxreqsize -= 3;
  }
  virtual ~XLineBatch() {	/* Destructor. */
    flush();			/* Flush pending draws. */
    delete points;		/* Kill off allocated storage. */
  }

  void draw(short x0, short y0) { /* Add a line point */
    insert->x = x0;
    insert->y = y0;
    insert++;
    addreq();			/* Do flush bookkeeping as needed. */
  }
  virtual void flush() {	/* Flush requests in a way that connects */
    if(numrequests > 1) {
      XPoint last;
      XDrawLines(disp, win, gc, points, numrequests, 
		 CoordModeOrigin);
      insert--;			/* Back up to last point. */
      last = *insert;		/* Save the last point for joining... */
      insert = points;
      postflush();
      draw(last.x, last.y);
    }
  }

};

/*
** XPointBatch does a batch of points.  This is almost identical to
** XLineBatch except that it's not necessary to 'connect'
*/
class XPointBatch :  public XBatch {
 protected:
  XPoint *points;
  XPoint *insert;
 public:
  XPointBatch(Display *d, Drawable w, GC c) : XBatch (d,w,c) { /* Constructor. */
    requestsize = 1;
    points      = new XPoint[maxreqsize/requestsize];
    insert      = points;
    maxreqsize -= 3;
  }
  virtual ~XPointBatch() {	/* Destructor. */
    flush();			/* Flush pending draws. */
    delete points;		/* Kill off allocated storage. */
  }

  void draw(short x0, short y0) { /* Add a line point */
    insert->x = x0;
    insert->y = y0;
    insert++;
    addreq();			/* Do flush bookkeeping as needed. */
  }
  virtual void flush() {	/* Flush requests in a way that connects */
    if(numrequests > 0) {
      XDrawPoints(disp, win, gc, points, numrequests, 
		 CoordModeOrigin);
      insert = points;
      postflush();
    }
  }

};

/*
** XFilRectBatch -- Draw filled rectangles in batch mode.
*/
class XFilRectBatch :  public XBatch {
 protected:
  XRectangle *points;
  XRectangle *insert;
 public:
  XFilRectBatch(Display *d, Drawable w, GC c) : XBatch (d,w,c) { /* Constructor. */
    requestsize = 2;
    points      = new XRectangle[maxreqsize/requestsize];
    insert      = points;
    maxreqsize -= 3;
  }
  virtual ~XFilRectBatch() {	/* Destructor. */
    flush();			/* Flush pending draws. */
    delete points;		/* Kill off allocated storage. */
  }

  void draw(short x0, short y0, short width, short height) { /* Add a rectangle. */
    insert->x = x0;
    insert->y = y0;
    insert->width = width;
    insert->height= height;
    insert++;
    addreq();			/* Do flush bookkeeping as needed. */
  }
  virtual void flush() {	/* Flush requests in a way that connects */
    if(numrequests > 0) {
      XFillRectangles(disp, win, gc, points, numrequests);
      insert = points;
      postflush();
    }
  }

};

/*
** XFilPoly  -- Draw a filled polygon.
*/
class XFilPoly : public XBatch {
 protected:
   XPoint *points;
   XPoint *insert;
 public:
    XFilPoly(Display *d, Drawable w, GC c) : XBatch (d,w,c) { /* Ctor. */
      requestsize = 2;
      points      = new XPoint[maxreqsize/requestsize];
      insert      = points;
      maxreqsize -= 3;
    }

   virtual ~XFilPoly() {	/* Destructor plots the polygon.  */
     flush();
     delete points;
   }

   void draw(short x0, short y0) { /* Add a point to the polygon: */
     insert->x   = x0;
     insert->y   = y0;
     insert++;
     addreq();			/* Flush as needed. */
   }

   virtual void flush() {
     if(numrequests > 0) {
       XFillPolygon(disp, win, gc, points, numrequests, Convex, 
		    CoordModeOrigin);
       insert = points;
       postflush();
     }
   }

};
#endif

