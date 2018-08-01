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

/*
** Facility:
**   DISPLAY graphical object manager.
** Abstract:
**   dispgrop.h   - This include file defines the structures and functions
**                  which manipulate graphical objects.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
** Version:
**    @(#)dispgrob.h	8.4 6/25/97 
*/


#ifndef DISPGROB_H_INSTALLED
#define DISPGROB_H_INSTALLED

#include "xamineDataTypes.h"
#include <stdio.h>
#include <assert.h>
#include <memory.h>
#include <string.h>

#include "XMWidget.h"

#include "dispwind.h"
#include "XBatch.h"
#include "convert.h"

#include <vector>
#include <string>




static const float GROBJ_MINDIST = 0.03;  // Fraction of height peak markers
				         // ride above spectrum. 

/*
** Data type and class definitions.
*/


class grobj_point {                       // X-Y point in graphical object 
                  protected:
                    unsigned int xpos;
		    unsigned int ypos;
		  public:
		    // Constructors initialize the positions to origin.

		    grobj_point() { set();}
		    grobj_point(int x, int y = 0)
		      { set(x,y); }
		    
		    // Set values of the coordinates

		    void setx(int x) { xpos = x; }
		    void sety(int y) { xpos = y; }
		    void set(int x = 0, int y = 0) 
		      { xpos = x; ypos  = y; }

		    // Access members

		    int getx() { return xpos; }
		    int gety() { return ypos; }
			
		  };

		      
		      



class grobj_generic {                       /* Generic graphical object */
                    protected:
		      grobj_name name; /* Title of the object. */
		      int      hasname; /* True if has name. */
		      unsigned int id; /* Object identifier */
		      int spectrum; /* Spectrum the object is set on. */
		      int npts;	/* Number of points in object. */
		      int where; /* Where we are in interator. */
		      grobj_point pts[GROBJ_MAXPTS];
		    public:
		      grobj_generic() {
			               init();
				     }
		      /* Manipulate the object identification fields */

		      void        setname(const grobj_name n)
			{ memset(name, 0, sizeof(name));
			  strncpy(name, n, GROBJ_NAMELEN);
			  hasname = -1; }
		      char *getname(grobj_name n)
			{  if(hasname) strcpy(n, name);
			   else n[0] = '\0';
			   return n; }
		      int named() { return hasname; }
		      void init() {
		                        npts     = 0;
					where    = 0;
					spectrum = 0;
					id       = 0;
					hasname  = 0;
					memset(name, 0, sizeof(name)); 

                                    }
		      void setid(const unsigned int newid)
		        { id = newid; }
		      unsigned int getid() {return id; }
		      void setspectrum(int s) {spectrum = s; }
		      int  getspectrum()      {return spectrum; }

		      /* Manipulate points in the object's point list. */

		      void clrpts()  { npts = 0; }
		      virtual int addpt(int x, int y = 0);
                      int delpt(int loc);
		      int delpt() { return delpt(npts-1); }
		      virtual int insertpt(int loc, int x, int y=0);
                      grobj_point *getpt(int loc);
                      grobj_point *firstpt(int loc = 0) {where = loc;
							 return getpt(where); }
		      grobj_point *thispt() {return getpt(where); }
                      grobj_point *nextpt();
		      int  pointcount() { return npts; }
                      int  cursor() { return where; }
                      
		      /* write graphical object definitions */
		    
		      virtual int  write(FILE *f);

		      /* Get the type of the grahpical object: */

                      virtual grobj_type type();

                      /* Get the software version:  */

		      char *getversion();
		      

		      virtual grobj_generic *clone()
			{
			  grobj_generic *t;
			  t = new grobj_generic;
			  if(t != NULL)
			    *t = *this;
			  return t;
			}
		      virtual void draw(XMWidget *p, win_attributed *at, Boolean f)
			{ }
		    };

/*  Derived classes from grobj_generic are the graphical object types we */
/*  actually use.                                                        */

class grobj_cut1d :public grobj_generic
                  {
		  public:
		    virtual int addpt(int x, int y = 0)
		      { return (npts >= 2) ? 0 : grobj_generic::addpt(x,y); }
		    virtual int insertpt(int loc, int x, int y=0)
		      { return (npts >= 2) ? 0 : 
                                             grobj_generic::insertpt(loc,x,y);
		      }
		    virtual grobj_type type();

		      /* write graphical object definitions */
		    
		    virtual int  write(FILE *f);
		    grobj_generic *clone() {
		      grobj_cut1d *t;
		      t = new grobj_cut1d;
		      if(t != NULL) 
			*t = *this;
		      return t;
		    }
		    virtual void draw(XMWidget *p, win_attributed *at, Boolean f);
		  };

class grobj_sum1d : public grobj_generic
                  {
		  public:
		    virtual int addpt(int x, int y = 0)
		      { return (npts >= 2) ? 0 : grobj_generic::addpt(x,y); }
		    virtual int insertpt(int loc, int x, int y=0)
		      { return (npts >= 2) ? 0 : 
                                             grobj_generic::insertpt(loc,x,y);
		      }
		    virtual grobj_type type();

		      /* write graphical object definitions */
		    
		    virtual int  write(FILE *f);
		    virtual grobj_generic *clone()
		      {
			grobj_sum1d *t;
			t = new grobj_sum1d;
			if(t != NULL)
			  *t = *this;
			return t;
		      }
		    virtual void draw(XMWidget *p, win_attributed *at, Boolean f);
		  };

class grobj_mark1d : public grobj_generic
                   {
		   public:
		    virtual int addpt(int x, int y = 0)
		      { return (npts >= 1) ? 0 : grobj_generic::addpt(x,y); }
		    virtual int insertpt(int loc, int x, int y=0)
		      { return (npts >= 1) ? 0 : 
                                             grobj_generic::insertpt(loc,x,y);
		      }
		    virtual grobj_type type();

		      /* write graphical object definitions */
		    
		    virtual int  write(FILE *f);
		    virtual grobj_generic *clone()
		      {
			grobj_mark1d *t;
			t = new grobj_mark1d;
			if(t != NULL)
			  *t = *this;
			return t;
		      }
		    virtual void draw(XMWidget *p, win_attributed *at, Boolean f);
		   };

class grobj_contour : public grobj_generic
                   {
		   public:
		     virtual grobj_type type();

		      /* write graphical object definitions */
		     
		     virtual int  write(FILE *f);
		     virtual grobj_generic *clone()
		      {
			grobj_contour *t;
			t = new grobj_contour;
			if(t != NULL)
			  *t = *this;
			return t;
		      }
		     virtual void draw(XMWidget *p, win_attributed *at, Boolean f);
		   };

class grobj_band : public grobj_generic
                 {
		 public:
		   virtual grobj_type type();

		      /* write graphical object definitions */
		    
		   virtual int  write(FILE *f);
		   virtual grobj_generic *clone()
		      {
			grobj_band *t;
			t = new grobj_band;
			if(t != NULL)
			  *t = *this;
			return t;
		      }
		   virtual void draw(XMWidget *p, win_attributed *at, Boolean f);
		 };

class grobj_sum2d : public grobj_generic
                 {
		 public:
		   virtual grobj_type type();

		      /* write graphical object definitions */
		    
		    virtual int  write(FILE *f);
		    virtual grobj_generic *clone()
		      {
			grobj_sum2d *t;
			t = new grobj_sum2d;
			if(t != NULL)
			  *t = *this;
			return t;
		      }
		   virtual void draw(XMWidget *p, win_attributed *at, Boolean f);

		 };

class grobj_mark2d : public grobj_generic
                  {
		  public:
		    virtual int addpt(int x, int y = 0)
		      { return (npts >= 1) ? 0 : grobj_generic::addpt(x,y); }
		    virtual int insertpt(int loc, int x, int y=0)
		      { return (npts >= 1) ? 0 : 
                                             grobj_generic::insertpt(loc,x,y);
		      }
		    virtual grobj_type type();

		      /* write graphical object definitions */
		    
		    virtual int  write(FILE *f);
		    virtual grobj_generic *clone()
		      {
			grobj_mark2d *t;
			t = new grobj_mark2d;
			if(t != NULL)
			  *t = *this;
			return t;
		      }
		    virtual void draw(XMWidget *p, win_attributed *at, Boolean f);
		  };

/*
**  The following are graphical objects which are only temporary.
**  They are used to maintain point lists associated with 
**  sets of points being accepted for client functions.
*/

class grobj_ptlist1 : public grobj_generic /* List of points on 1-d.. */
{
 public:
  grobj_ptlist1(int mpts)  { maxpts = mpts; }

  virtual int addpt(int x, int y = 0);
  virtual int insertpt(int loc, int x, int y=0);
  virtual grobj_type type() {return pointlist_1d; }
  virtual void draw(XMWidget *p, win_attributed *at, Boolean final);
  virtual grobj_generic *clone();
 private:
  int maxpts;			/* Max number of points allowed. */
};

class grobj_ptlist2 : public grobj_generic /* List of points on 2-d... */
{
 public:
  grobj_ptlist2(int mpts = GROBJ_MAXPTS) { maxpts = mpts; }

  virtual int addpt(int x, int y=0);
  virtual int insertpt(int loc, int x, int y = 0);
  virtual grobj_type type() { return pointlist_2d; }
  virtual void draw(XMWidget *p, win_attributed *at, Boolean final);
  virtual grobj_generic *clone();
 private:
  int maxpts;			/* Max number of points allowed. */
};
/*
**  The class of grobj_database is a database of graphical objects.
**  for rapid prototyping purposes, the database is just a linear
**  list of graphical objects.
*/
class grobj_database  {
                        protected:
                           static int    m_ReadOnce;  // Need to reset lexer.
                           grobj_name    dbname;
			   int           hasname;
			   // grobj_generic *objects[GROBJ_MAXOBJECTS];
                           std::vector<grobj_generic*> objects;
                           // int   obj_count; /* Number of objects. */
			   int   lastid;    /* Last ID assigned. */
			   int   cursor;    /* Cursor in sequential search */
			   int   confined;  /* TRUE If seq srch in spectrum. */
			   int   searchspec; /* Which spectrum confined to. */
			 public:
			   grobj_database() 
			     { 
			       lastid    = 0;
			       cursor    = 0;
			       confined  = 0;
			       hasname   = 0;
			     }
			   ~grobj_database();
			   void name(const grobj_name n)
			     { memset(dbname, 0, sizeof(dbname));
			       strncpy(dbname, n, GROBJ_NAMELEN);
			       hasname   = -1;
			     }
			   int named() { return hasname; }
			   char *getname(grobj_name n)
			     { strcpy(n, dbname);
			       return hasname ? n : (char *)NULL;
			     }
			   grobj_generic *enter(grobj_generic *object);
			   grobj_generic *enteras(grobj_generic *object);
			   int            remove(grobj_generic *object);
			   grobj_generic *replace(int id, 
						  grobj_generic *object);
			   grobj_generic *replace(int spec,
						  int id,
						  grobj_generic *object);
			   grobj_generic *find(int id);
			   grobj_generic *find(int id, int spec);
			   grobj_generic *find_first(int spec);
			   grobj_generic *find_first();
			   grobj_generic *find_next();
			   int write(FILE *f);
			   int read(FILE *f);
			   int newid()    { return lastid++; }
			   int nextid()   { 
			     return lastid; }        /* Next ID assigned */
			   void clear();             /* Clear the database. */
			 };



class grobj_Peak1d : public grobj_generic
{
  // Data attributes:

private:
  float m_fFWHM;		// Width of the peak.
  float m_fPosition;		// Centroid of the peak.
  //
  // Utility:
  //
protected:
  unsigned ComputeHeight(float fChan);  // Figure out 'good' heights for stuff.
  unsigned ComputeBarHeight(float fChan); // Figure out drop on bars.
  unsigned ComputeDeltaHeight(float fChan); // How tall should bars be.
  void ComputeLocation(Xamine_Convert1d &cvt,
		       float fChannel, unsigned  nHeight, short &xpix, 
		       short  &ypix);
  void DrawMarker(XSegmentBatch &XSseg, Boolean flipped,
		  short npLeft, short npLBottom, 
		  short npHeight, short npRight, 
		  short npRBottom,short npMid,short npPeakht);
	     
  
  //
  // Constructors:
  //
public:
  grobj_Peak1d(int nSpectrum,	    // Create an instance of ourself.
	       int nId,	 	    // nSpectrum and nId identify the object. 
	       grobj_name pszName,  // pszName is comment/naming text. 
	       float fCentroid,	    // fCentroid and fWidth are the peak 
	       float fWidth);	    // characteristics the object marks.
  
  //  Selectors and other informatives:
  //
public:
  virtual grobj_type type() { return peak1d; }
  float Centroid() { return m_fPosition; }
  float Width()    { return m_fFWHM; } 
  //
  // Operations:
public:
  virtual void draw(XMWidget* pWindow,  // Draw the object.
		    win_attributed* pAttributes, // in the window.
		    Boolean   fFinal);
  virtual grobj_generic* clone();	         // Produce copy of self.

};

/*!

     This is a graphical object that displays a fitline. 
     we are quite a bit different than other grobjs in that our points
     re relatively meaningless.. instead, we have a region of interest
     and a Tcl proc that is capable of evaluating to the height of the
     fitline at any position within the area of interest.
*/
class grobj_Fitline : public grobj_generic
{
  // Attributes:
private:
  int         m_low;
  int         m_high;
  std::string m_evalProc;
  // constructors.. we can support the full cannonical set too!

public:
  grobj_Fitline(int nSpectrum, int nId, grobj_name pszName,
		int low, int high, const char* evalProc);
  grobj_Fitline(const grobj_Fitline& rhs);
  virtual ~grobj_Fitline();

  grobj_Fitline&  operator=(const grobj_Fitline& rhs);
  int operator==(const grobj_Fitline& rhs) const;
  int operator!=(const grobj_Fitline& rhs) const;

  // Virtual function overrides:

  virtual grobj_type type();
  virtual void draw(XMWidget* pWindow, win_attributed* pAttributes, 
		    Boolean final);
  virtual grobj_generic* clone();


  // Utility functions:
private:
  void copyIn(const grobj_Fitline& rhs);

  std::vector<std::pair<int,float> > computePoints(win_attributed* pAttrib);
  std::pair<int,int> computePosition(Xamine_Convert1d& cvt,
				     int               channel,
				     float             height);
  void drawFitline(Display* d, Drawable w, GC& gc, XMWidget* pWindow,
		   Boolean flipped, Boolean labelit,
		   std::vector<std::pair<int, int> >& points);
};
#endif
