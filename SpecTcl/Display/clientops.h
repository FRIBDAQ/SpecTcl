/*
** Facility:
**   Xamine - NSCL Display program.
** Abstract:
**   clientops.h:
**     This file contains definitions which are common between client program
**     and Xamine for the handling of client buttons.  Xamine supports client
**     buttons as follows:
**      On request, Xamine will display a single rectangular array of buttons
**    as described by the user.  The buttons may be either pushbuttons
**    or toggle buttons.  A button press can be programmed to return any
**    one of the following to the user in addition to fact that it has been
**    pressed:
**       No additional data   - Simple button.
**       Text string          - Prompted for using a text string prompt
**                              dialog widget.
**       Spectrum             - Prompted for by a spectrum selection dialog
**                              widget box. 
**       Point set            - Prompted for by a graphical input dialog
**                              box.
** Author:
**    Ron Fox
**    NSCL
**    Michigan State University
**    East Lansing, MI 48824-1321
**   @(#)clientops.h	8.2 6/25/97 
*/

#ifndef _CLIENTOPS_H
#define _CLIENTOPS_H

#include <Xamine.h>

#define BUTTON_LABELSIZE  16
#define BUTTON_PROMPTSIZE 80
#define BUTTON_MAXTEXT    256

typedef char ButtonText[BUTTON_MAXTEXT];


#define GROBJ_MAXPTS	50	/* Maximum number of points in a grob. */
#define GROBJ_NAMELEN   80	/* Characters in a graphical object name. */
#define GROBJ_MAXOBJECTS 2048   /* Maximum # of graphical objects      */

typedef char grobj_name[GROBJ_NAMELEN+1];
		      
#ifndef _DISPGROB_H_INSTALLED
typedef Xamine_gatetype grobj_type ;
#endif

typedef int  unsigned Cardinal;

typedef
enum {
       Gate,
       Button_Press
     } msg_type;




    /* Operation request codes recoginized by Xamine */


typedef enum _operation {
                 /* Gate managemenet operations */

                 Enter,		/* Enter a gate. */
		 Remove,	/* Remove a gate. */
		 Inquire,	/* Inquire a gate list. */

		   /* Button management operations */

		 Button_CreateBox,
		 Button_DefineButton,
		 Button_EnableButton,
		 Button_DisableButton,
		 Button_ModifyButton,
		 Button_InquireButton,
		 Button_DeleteButton,
		 Button_DeleteBox,

		 /* User created graphical objects: */

		 EnterPeakPosition /* Enter a Peak position grobj. */
	       } operation;

    /* Status return codes in the ack mailbox: */

#ifdef Success			/* Defined by motif I think */
#undef Success
#endif
#ifndef __ALPHA
typedef enum _msg_status {
                  Success               =  0,
		  NoSuchSpectrum        = -1,
		  InappropriateGate     = -2,
		  ObjectTableFull       = -3,
		  DuplicateId           = -4,
		  NoSuchObject          = -5,
		  NoProcess             = -6,
                  CheckErrno            = -7,
		  BadCoordinates        = -8,
		  BadType               = -9,
		  BadPrompter           = -10,
		  ButtonCreated         =  1
		} msg_status;
#else
typedef int msg_status;
#define       Success                 0
#define       NoSuchSpectrum         -1
#define	      InappropriateGate      -2
#define       ObjectTableFull        -3
#define	      DuplicateId            -4
#define	      NoSuchObject           -5
#define	      NoProcess              -6
#define       CheckErrno             -7
#define       BadCoordinates         -8
#define       BadType                -9
#define       BadPrompter           -10
#define       ButtonCreated           1
#endif		  

/* Button type definitions. */

typedef enum {
                Push,
		Toggle		/* Type of buttons we can make. */
		} ButtonType;

typedef enum {
               NoPrompt,
	       Confirmation,
	       Text,
	       Points,		/* Types of prompts a button can trigger */
	       Spectrum,
	       Filename
	       } ButtonPromptType;

typedef enum {
               Anytime,
	       InSpectrum,
	       In1dSpectrum,	/* Sensitivity modifiers selected pane is: */
	       In2dSpectrum
	       } ButtonSensitivity;

typedef enum {
              F = 0,	        /* Can never count on Boolean being defined */
	      T = 1		/* Mustj avoid collisions too.              */
	      } Logical;

typedef enum {			/* Type of spectrum a spectrum chooser can */
               Any,		/* Prompt for.                             */
	       Oned,
	       Twod,
	       Compatible
	       } ButtonDialogSpectrumType;

typedef char ButtonTitle[BUTTON_LABELSIZE];
typedef char ButtonDialogPrompt[BUTTON_PROMPTSIZE];
typedef struct {
                 Cardinal column; /* Which button in box. */
		 Cardinal row;
	       } ButtonCoordinate;

typedef struct {		/* Describe a button. */

                 int         button_code; /* Code to return on activation. */
		 ButtonTitle label;       /* Text written in button        */
		 ButtonType  type;        /* Type of button.               */
		 Logical     state;       /* Initial toggle state          */
		 Logical     sensitive;	  /* Initial sensitivity state     */

				/* Fields needed for prompters: */

		 ButtonPromptType prompter; /* Additional data prompter type */
		 ButtonSensitivity whenavailable; /* When button works      */
		 ButtonDialogPrompt promptstr; /* Prompt string for dialog  */
		 ButtonDialogSpectrumType
		                   spectype; /* Type of spectra prompting for */
		 Cardinal   minpts, maxpts; /* Number of desired points.   */
		 } ButtonDescription;

		 
/*
** The types below are used in forming, sending and receiving request
** messages.
*/

    /* A point, as represented by messages. */

typedef struct _msg_point { 
               int x;
	       int y;
	     } msg_point;

    /* A message command structure sent to the request exchange */

struct msg_command {
                          operation command;
		   };

    /* Below is the representation of an object in messages sent around: */

typedef
struct _msg_object  {
                      unsigned int spectrum; /* Number of spectrum of object */
		      unsigned int id;      /* Objecto identification number. */
		      grobj_type   type;     /* Object type code. */
		      int          hasname;  /* Non zero if there's a name. */
		      grobj_name   name;     /* Object name.      */
		      int          npts;     /* Number of points in object */
		      msg_point    points[GROBJ_MAXPTS]; /* the points     */
		    } msg_object;

    /* Below is the representation of a peak position graphical object: */

typedef
struct _msg_peakentry {
                        int nSpectrum;          /* Number of object's spec.  */
                        int nId;                /* Unique object id number   */
                        int          nHasName;  /* True if name exists.      */
                        grobj_name   szName;    /* Name of the item.         */
                        float        fCentroid;
                        float        fWidth;
                      } msg_peakentry;
                       

    /* The acknowledgment message for enter messages: */

struct msg_enterack {
                      msg_status status;
		    };
    /* The data message for remove gate requests. */

struct msg_remove {
                     unsigned int spectrum; /*  Spectrum gate is set on. */
		     unsigned int object;   /*  Identifier of object to rmv */
		     grobj_type   type;     /*  Type of object to rmv.   */
		  };


     /* Acknowledge for remove requests.  */

struct msg_rmvack {
                     msg_status status; /* Status of the attempt. */
		  };

     /* Below asks for the list of objects on a spectrum: */

struct msg_inquiremsg {
                        int spectrum; /* Spectrum id. */
		      };


     /* Below is the response prior to the list of msg_object structures */
     /* that come back from an inquire:                                  */


struct msg_inquireack {
                        msg_status status;    /* Status of the attempt. */
			int        nobjects;  /* number of objects returned */
		      };

     /*
     **   The structures below are used when sending button management
     **   requests to Xamine.
     */



struct msg_CreateButtons {	/* Create a user button box... */
  ButtonCoordinate   size;	/* Number of rows and columns in the box. */
};

struct msg_DefineButton {	/* Define a new button. */
  ButtonCoordinate   which;	/* Which button is being described.  */
  ButtonDescription  button;	/* Describes the button being defined */
   
};


struct msg_EnableButton {	/* Enable a button  */
  ButtonCoordinate which;	/* Defines which button is enabled */
};

struct msg_DisableButton {	/* Disable a button                */
  ButtonCoordinate which;	/* Defines which button is disabled. */
};

struct msg_ModifyButton {	/* Modifies the def of an existing button */
  ButtonCoordinate which;	/* Defines which button is modified */
  ButtonDescription button;	/* New button description.          */
};

struct msg_DeleteButton {	/* Delete a button from the user box. */
  ButtonCoordinate  which;	/* Which button to delete.            */
};


struct msg_InquireButton {	/* Ask for a button's description. */
  ButtonCoordinate which;	/* Which button we're asking about */
};

       /* The structures below are returned by Xamine in response to various
       ** Button management requests.
       */

#ifdef __ALPHA
#pragma member_alignment __save
#pragma nomember_alignment
#endif
struct msg_InquireButtonAck {	/* Button information... */
  ButtonCoordinate  size;	/* Size of the button grid... */
  Logical           valid;	/* Indicates if button queried was valid. */
  ButtonDescription button;	/* Info about the requested button */
};
#ifdef __ALPHA
#pragma member_alignment __restore

#endif

       /* This is the data sent to the 'gates' mailbox by Xamine when a
       ** user button is hit and all prompt data has been accepted.
       */
typedef
struct _msg_ButtonPress {

  /* These fields should always be present and valid  */

  int      buttonid;		/* Number of the button pressed. */
  int      selected;		/* Id. of selected spectrum or 0 if none */
  Logical  togglestate;		/* State of toggle button. if toggle. */

  /* The validity of the following fields depends on the prompt type */

  int      spectrum;		/* Spectrum chosen from spectrum prompt */
  ButtonText text;		/* Text value from either filename or text. */
  int      npts;		/* Number of points received if pt prompt */
  msg_point points[GROBJ_MAXPTS]; /* The array of points if pt prompt.    */

} msg_ButtonPress;

   /* The following union is the type of data structure that is received
   ** from Xamine's gate mailbox.  This can be either a gate message
   ** (msg_object), or a button notification (msg_ButtonPress) at this time
   */

typedef struct  _msg_XamineEvent {
  msg_type               event;	/* Type of event. */
  union {
    msg_object      gate;
    msg_ButtonPress button;
  } data;
} msg_XamineEvent;

/*
**   Define Pipe I/O functions and aliases:
*/

int Xamine_WritePipe(int id, void *buffer, int size);
int Xamine_ReadPipe(int id, int timeout, void *buffer, int size);


#define WritePipe Xamine_WritePipe_
#define ReadPipe  Xamine_ReadPipe_

#ifndef _GATEIO_H
#define requests Xamine_requests
#define acks     Xamine_acks
#define newgates Xamine_newgates
extern int Xamine_requests, Xamine_acks, Xamine_newgates;
#endif

#endif
 
