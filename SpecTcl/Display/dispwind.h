/*
** Facility:
**   Display spectrum window management subsystem.
** Abstract:
**   dispwind.h       - This include file defines data types and
**                      functions which manage the window definition
**                      database.
** Author:
**   Ron Fox
**   NSCL
**   Michigan State University
**   East Lansing, MI 48824-1321
**   @(#)dispwind.h	8.4 6/8/97 
*/

#ifndef _DISPWIND_H_INSTALLED
#define _DISPWIND_H_INSTALLED

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef unix
#include <memory.h>
#ifndef linux
#include <strings.h>
#endif
#include <sys/time.h>
#include <sys/types.h>
#endif
#ifdef VMS
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <types.h>
#endif
#include <assert.h>
#ifndef FALSE
#define TRUE  (1)
#define FALSE (0)
#endif
#include "superpos.h"

extern "C" {
#ifdef VMS
  void exit(int code);
#endif
#ifdef ultrix
  time_t time(time_t *tloc);
#endif
}

#define WINDOW_MAXTITLE   81	/* Max chars in a title. */
#define WINDOW_MAXWIN    100	/* Maximum number of windows. */
#define WINDOW_MAXAXIS    10    /* Max # windows on an axis.  */
#define WINDOW_FILETYPE   ".win" /* Type of window file. */
typedef char win_title[WINDOW_MAXWIN];

class win_geometry {
                   protected:
                      unsigned int win_nx; /* Number in X */
		      unsigned int win_ny; /* Number in Y */
		   public:
		      win_geometry(int nx=1, int ny=1)
			{ setgeometry(nx,ny); }
		      int nx() {return win_nx;}
		      int ny() {return win_ny;}
		      void setx(int nx) { win_nx = nx; }
		      void sety(int ny) { win_ny = ny; }
		      void setgeometry(int nx, int ny) 
			{ win_nx = nx; win_ny = ny; }
                      void setgeometry(win_geometry &g)
                        { *this = g; }
                      int write(FILE *f)
			{ return fprintf(f, "Geometry %d,%d\n", win_nx, win_ny); }
		    };


//
//  Base class for window definitions
//
class  win_definition {
                      protected:
			unsigned int win_spnum; // Number of spectrum in window
		      public:
			win_definition()
			  { }
			win_definition(int spnum)
			  { win_spnum = spnum; }
			void use(int spnum)
			  { win_spnum = spnum; }
			void operator=(win_definition &win) 
			  { *this = win; }
			void operator=(int spec) 
			  { win_spnum = spec; }
			int spectrum()  { return win_spnum; }
			virtual int is1d() {
			  fprintf(stderr, 
				  "BUG ... in_attributed::1d called\n");
			  exit(-1);
			  return FALSE;
			}
			virtual int write(FILE *f) {return EOF;} // Cannot call.
		      };
//
//  win_attributed class represents a window pane with a generic spectrum
//  (1-d or 2-d) together with it's display attributes
//
enum reduction_mode {                          // reduction attributes,
                      sampled,                  // to make a spectrum fit a
                      summed,                  // smaller set of pixels
                      averaged                 // than the spectrum.
                    };

class win_attributed : public win_definition {
 protected:
  // Below are flags to indicate which
  // Axis label elements are present
  //
  int axis;
  int ticks;
  int axis_labels;
  //
  // Below are flags to indicate which
  // spectrum titles are turned on in this
  // window
  //
  int name;
  int number;
  int describe;
  int maxpeak;
  int update_info;
  int label_objects;
  
  time_t update_time;
  int flipped;     // TRUE if axes flipped.
  
  reduction_mode reduction;
  //
  //  The attributes below describe the scale
  //  mode and full scale if manual
  //
  int manual;
  unsigned int full_scale;
  
  int log_scale;     // TRUE if log counts scale
  int has_floor,
    floor;           // Defines the display floor
  int has_ceiling,
    ceiling;         // Defines ceiling
  int auto_update;   // True if auto update set
  int update_period; // Secs between updates
  
  int user_mapping;  // True if user supp. map
 public:
  // Constructors
  win_attributed() {
    set_defaults(); }
  win_attributed(int spnum) : 
    win_definition(spnum)
    { 
      set_defaults(); 
    }

  // Set/Get axis attributes  
  void axeson()   { axis = TRUE; }
  void axesoff()  { axis = FALSE; }
  void tickson()  { ticks= TRUE;  }
  void ticksoff() { ticks= FALSE; }
  void axis_labelson()  { axis_labels = TRUE; }
  void axis_labelsoff() { axis_labels = FALSE; }
  void set_axes(int newaxes = FALSE,
		int newticks= FALSE,
		int newalbls= FALSE)
    { 
      axis = newaxes;
      ticks= newticks;
      axis_labels = newalbls;
    }
  int showaxes()  { return axis; }
  int showticks()  { return ticks; }
  int labelaxes()  { return axis_labels; }
  
  // Set/Get spectrum title labels.
  
  void dispname() { name = TRUE; }
  void hidename() { name = FALSE; }
  void dispid()   { number = TRUE; }
  void hideid()   { number = FALSE; }
  void dispdescr() {describe = TRUE; }
  void hidedescr() {describe = FALSE; }
  void disppeak()  { maxpeak = TRUE; }
  void hidepeak()  { maxpeak = FALSE; }
  void dispupd() { update_info = TRUE; }
  void hideupd() { update_info = FALSE; }
  void labelobj() { label_objects = TRUE; }
  void unlabelobj() { label_objects = FALSE; }
  void set_titles( int spname = FALSE,
		   int spnum  = FALSE,
		   int desc   = FALSE,
		   int peak   = FALSE,
		   int upd    = FALSE,
		   int objs   = FALSE )
    { 
      name     = spname;
      number   = spnum;
      describe = desc;
      maxpeak  = peak;
      update_info = upd;
      label_objects = objs;
    }
  
  int showname() { return name; }
  int shownum()  { return number; }
  int showdescrip() { return describe; }
  int showpeak() { return maxpeak; }
  int showupdt() { return update_info; }
  int showlbl()  { return label_objects; }
  
  // Set/show axis orientation
  
  void normal() { flipped = FALSE; }
  void sideways() { flipped = TRUE; }
  void flip()  { flipped = !flipped; }
  void setflip(int newflip) { flipped = newflip;}
  int isflipped() { return flipped; }
  
  // Get/set the reduction mode.
  
  void sample() { reduction = sampled; }
  void sum()    { reduction = summed; }
  void average() { reduction = averaged; }
  void setreduce(reduction_mode r)
    { reduction = r; }
  reduction_mode getreduction() 
    { return reduction; }
  // Get/Set scaling attributes
  
  void autoscale() { manual = FALSE; }
  void setfs(unsigned int fs) 
    { 
      full_scale = fs;
      manual     = TRUE;
    }
  int manuallyscaled() { return manual; }
  int getfs() { return (manual ? (int) full_scale : -1); }
  unsigned int getfsval() { return full_scale; }
  void setfsval(unsigned int fs) { full_scale = fs; }
  // Get/set counts axis attribute
  
  void log() { log_scale = TRUE; }
  void linear() { log_scale = FALSE; }
  void setlog(int log) {log_scale = log; }
  int islog() { return log_scale; }
  
  // Get/set floor/ceiling attributes.
  
  void nofloor() { has_floor = FALSE; }
  void noceiling() { has_ceiling = FALSE; }
  void setfloor(int f) { 
    floor = f;
    has_floor = TRUE;
  }
  void setceiling(int c) { 
    ceiling = c;
    has_ceiling = TRUE; 
  }
  void setcntlimits(int f, int c) { 
    floor   = f;
    ceiling = c;
    has_floor = TRUE;
    has_ceiling = TRUE;
  }
  int hasfloor() { return has_floor; }
  int hasceiling() { return has_ceiling; }
  int getfloor()   { return floor; }
  int getceiling() { return ceiling; }
  void setall(win_attributed &that);
  void setattribs(win_attributed &that);
  
  void markupdate()  // Indicate update time
    { time(&update_time); }
  time_t lastupdated() 
    { return update_time; }
  
  /* Set/Get the auto update attributes */
  
  void noautoupdate() { auto_update = FALSE; }
  void update_interval(int sec) {
    if(sec > 0) {
      auto_update = TRUE;
      update_period = sec;
    } 
    else
      auto_update = FALSE;
  }
  void setmapped(int m) {
    user_mapping = m;
  }

  int update_interval() {
    return ((auto_update) ? update_period : 0);
  }
  int autoupdate_enabled() { 
    return auto_update;
  }

  int ismapped() {
    return user_mapping;
  }
  
  // Set defaults ... will want to glob on
  // additional functionality in derived classes
  
  virtual void set_defaults();  // Back to dflts
  virtual int is1d() { return FALSE;}
  virtual int write(FILE *f);
};
//
//  win_1d is a class which contains a window that has a 1-d spectrum
//  inside it.  This requires additional attributes and methods to modify
//  and access them.
// 
enum rendition_1d {              // Defines the possible display renditions
                    smoothed,    // for a 1-d spectrum.
                    histogram,
                    lines,
                    points
                  };
class win_1d : public win_attributed
             {
              protected:

                  //  Expansion information

                  int expanded;
                  int xlow, xhigh;

                  //   Rendition

                  rendition_1d rendition;

                  // Superpositions: 

                 SuperpositionList additional_spectra;
              public:
                 // Constructors

                 win_1d()  { set_defaults(); }
                 win_1d(int spnum): win_attributed(spnum) { 
                                                             set_defaults(); }
                 win_1d(win_attributed &generics) { set_defaults();
                                              win_attributed::setall(generics);
                                                  }
                 //
                 // Set/get expansion data
                 //
                 void unexpand() { expanded = FALSE; }
                 void expand(int l, int h) { xlow = l;
                                             xhigh= h;
                                             expanded = TRUE;
                                           }
                 int isexpanded() { return expanded; }
                 int lowlimit()   { return xlow; }
                 int highlimit()  { return xhigh; }
                 //
                 // Set/get rendition data
                 //
                 void smooth() { rendition = smoothed; }
                 void bars()   { rendition = histogram;}
                 void pts()    { rendition = points;   }
                 void line()   { rendition = lines;    }
                 void setrend(rendition_1d rend) {rendition = rend; }
                 rendition_1d getrend() { return rendition; }
                 //
                 // Get characteristics
                 //
                 virtual int is1d() { return TRUE; }
                 SuperpositionList &GetSuperpositions() {
                    return additional_spectra; }
                 void RemoveSuperposition(int spno);
                 //
                 // Set all attributes  back to defaults
                 //
                 virtual void set_defaults();
                 //
                 // Write to file:
                 //
                 virtual int write(FILE *f);
            };

//
//  The win_2d class is derived from win_attributed.
//  It contains additional attributes and methods for storing information
//  about a window which is displaying a 2-d display.
//
enum rendition_2d {                 // rendition_2d enumerates the possible
                    scatter,        // renditions for a 2-d spetrum.
                    boxes,
                    color,
                    contour,
                    surface,
                    lego };

class win_2d:public win_attributed
            {
             protected:
                //
                // Attributes which describe the spectrum expansion state
                //
                int expanded;             // True if spectrum expanded.
		int expandedfirst;        // True if expanded before flipped
                int xlow, xhigh;          // X expansion limits.
                int ylow, yhigh;          // Y expansion limits.
                //
                // Attributes which define the spectrum rendition
                //
                rendition_2d rendition;
             public:
                //
                //  Constructor classes
                //
                win_2d() { set_defaults(); }
                win_2d(int spnum) : win_attributed(spnum) { set_defaults(); }
                win_2d(win_attributed &generic) { set_defaults();
                                             win_attributed::setall(generic); }
                //
                //   Get/Set expansion of the window
                //
                void unexpand() { expanded = FALSE; expandedfirst = FALSE; }
                void  expand(int xl,int xh,int yl,int yh)
                                { xlow  = xl;
                                  xhigh = xh;
                                  ylow  = yl;
                                  yhigh = yh;
                                  expanded = TRUE;
				  if(!flipped) expandedfirst = TRUE;
				  else expandedfirst = FALSE;
                                }
		void expandfirst() { expandedfirst = TRUE; }
                 int isexpanded() { return expanded; }
		 int isexpandedfirst() { return expandedfirst; }
                 int xlowlim()    { return xlow; }
                 int xhilim()     { return xhigh; }
                 int ylowlim()    { return ylow;  }
                 int yhilim()     { return yhigh; }
                 //
                 /*  Get/Set the window's drawing rendition: */
                 //
                 void scatterplot() { rendition = scatter; }
                 void boxplot()     { rendition = boxes;   }
                 void colorplot()   { rendition = color;   }
                 void contourplot() { rendition = contour; }
                 void legoplot()    { rendition = lego;    }
                 void setrend(rendition_2d rend) { rendition = rend; }
                 rendition_2d getrend() {return rendition; }
                 //
                 // Get the spectrum shape
                 //
                 virtual int is1d() { return FALSE; }
                 //
                 // Set the default attributes.
                 //
                 virtual void set_defaults();
                 //
                 // Write the definition:
                 //
                 virtual int write(FILE *f);
               };

//
// win_db   - This class is a window database. It contains and keeps track
//            of a set of window definitions.  
//            The window database is limited to WINDOW_MAXWIN entries with
//            at most WINDOW_MAXAXIS  entries on an axis.  This is done 
//            because that's all you can really see on a typical tube.
//            In the present implementation the array of windows is
//            a statically allocated set of pointers, but the 
//            window entries themselves are dynamically allocated.

class win_db : public win_geometry
      {
       protected:
         static int   m_ReadOnce;    // True if flex has been used once.
         int          zoomed;           // Showing one window full?
         int          zx, zy;    // If so, which one?
         int          titled;       // TRUE if title present.
         win_title    title;        // Contains title string.
         win_definition *windows[WINDOW_MAXAXIS][WINDOW_MAXAXIS]; // Window defs.
         void init();
       public:
         //   Constructors
         //
         win_db() { init(); }
         win_db(const char *title) { init();    // Pre-titled windows
                                          settitle(title);
                                        }
         win_db(win_geometry &geom, char *title = (char *)NULL) // Pre-sized 
                  {                                          // with title.
                    init();
                    assert( (geom.nx() >=0) && (geom.ny() >= 0));
                    assert( (geom.nx() < WINDOW_MAXAXIS) && 
                            (geom.nx() < WINDOW_MAXAXIS));
                    setgeometry(geom);
                    if(title != (char *)NULL) settitle(title);
                  }
         win_db(int inx, int iny, char *title=(char *)NULL)
                  {
                    init();
                    assert( (inx >= 0) && (iny >= 0));
                    assert( (inx < WINDOW_MAXAXIS) && (iny < WINDOW_MAXAXIS));
                    setgeometry(inx,iny);
                    if(title != (char *)NULL) settitle(title);
                  }
         // Destructor
         //    Mostly, we need to delete storage associated with the windows
         //    attribute of the object.
         ~win_db() {
                      cleardb();
                   }
          // Clear the database

          void cleardb() {
                           for(int i = 0; i < WINDOW_MAXAXIS; i++)
                              for(int j=0; j < WINDOW_MAXAXIS; j++)
                                 if(windows[i][j] != (win_definition *)NULL) {
                                    delete windows[i][j];
                                    windows[i][j] = (win_definition *)NULL;
                                 }
                           setgeometry(1,1);
                           clrtitle();
                           unzoom();
                         }
          // Manipulate the zooming attributes
          void zoom(int x, int y) { assert(exists(x,y));
                                    zoomed = TRUE;
                                    zx = x;
                                    zy = y;
                                  }
          void unzoom()    { zoomed = FALSE; }
          int  zoomx()     { return zx; }
          int  zoomy()     { return zy; }
          int iszoomed()   { return zoomed; }

          // Manipulate the title attributes of the class
          //
          void settitle(const char *t)  { strcpy(title, t);
                                        titled = TRUE;
                                      }
          char *gettitle(char *t)  { if(titled)
                                           strcpy(t, title);
                                         else
                                           strcpy(t," Untitle ");
                                         return t;
                                 }
          int hastitle()  { return titled; }
          void clrtitle()  { titled = FALSE; }
          //
          // Manipulate spectrum definitions. 
          //
          int exists(int x, int y) {  return ( (x < nx()) && (x >= 0) &&
                                               (y < ny()) && (y >= 0));
                                   }
          int defined(int x, int y) 
                       { return (exists(x,y) ? 
                                    windows[x][y] != (win_definition *)NULL :
                                    FALSE);
                       }
          void define1d(int x, int y,int specnum);
          void define2d(int x, int y,int specnum);
          win_attributed *getdef(int x, int y) 
                  { return (defined(x,y) ? 
                                 (win_attributed *)((windows[x][y])) :
                                 (win_attributed *)NULL); }
          void undefine(int x, int y);
          int refresh_rate(int x, int y) {
             return (defined(x,y) ?
                     getdef(x,y)->update_interval()   :
                     0);
          }
          void refresh_rate(int x, int y, int sec) {
             if(defined(x,y)) getdef(x,y)->update_interval(sec);
          }
          int  write(const char *filename);    // Write database to file. 
          int  read(const  char *filename);             // Read database from file.
      };
#endif




