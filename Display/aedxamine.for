C
C Facility:
C   Xamine  - NSCL Display program.
C Abstract:
C   aedxamine.for:
C      This file implements the AEDTSK compatibility bindings to
C      Xamine for unix.  For VMS, see aedxamine_vms.for
C Author:
C    Ron Fox
C    NSCL
C    Michigan State University
C    East Lansing, MI 48824-1321
C
C SCCS:
C   @(#)aedxamine.for	8.1  6/23/95 
C
CCccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
	INTEGER FUNCTION F77XAMINE_INIT(ISIZE)
C
C        Stubby little native function to initialize shared memory
C        for Xamine:
C
	INTEGER ISIZE
	INTEGER ISTAT

	CALL AEDINIT(ISTAT, ISIZE)
	F7XAMINE_INIT = ISTAT
	RETURN
	END
	SUBROUTINE AEDINIT(ISTAT,ISIZE)
 
C+
C 
C FUNCTIONAL DESCRIPTION:	
C 
C    AEDINIT:
C      This subroutine provides AEDTSK compatibility with XAMINE for the
C	AEDTSK AEDINIT function.  The AEDINIT subroutine passes the size
C       of the shared memory region in to the client routines along with
C       a pointer to the front of the shared memory region.
C 
C FORMAL PARAMETERS:
C  
C     integer istat:
C	    Returns the mapping status.
C     integer isize:
C           Number of bytes of spectra (must be <= 8mbytes).
C  
C 
C Author:
C		Ron Fox                                                                         
C		NSCL
C		Michigan State University
C		East Lansing, MI 48824-1321
C		18-OCT-1993
C Modified:
C	1-001	Original version by Ron Fox                                                                         	18-OCT-1993
C--
	IMPLICIT NONE
	INCLUDE 'aedshare.for'
	INTEGER  ISTAT, ISIZE
	INTEGER IS
 
	IS = (ISIZE + 511)/512
	IS = IS * 512
	ISTAT = F77XAMINE_CREATESHAREDMEMORY(IS, XAMINE_XYPTR)
C
C		Set the pointers so that references to the Xamine block
C               will work.
C
	XAMINE_TITLESPTR  = XAMINE_XYPTR +       2*(2*XAMINE_ISMAX)
	XAMINE_OFFSETSPTR = XAMINE_TITLESPTR +   72*XAMINE_ISMAX
	XAMINE_TYPESPTR   = XAMINE_OFFSETSPTR +  4*XAMINE_ISMAX
	XAMINE_DATA1PTR   = XAMINE_TYPESPTR +    4*XAMINE_ISMAX
	XAMINE_DATA2PTR   = XAMINE_DATA1PTR
	XAMINE_DATA4PTR   = XAMINE_DATA1PTR
C
C             Set the AEDTSK compatibility mode pointers:
C
	AED_XYPTR         = XAMINE_XYPTR
	AED_TITLEPTR      = XAMINE_TITLESPTR
	AED_OFFSETPTR     = XAMINE_OFFSETSPTR
	AED_TYPEPTR       = XAMINE_TYPESPTR
	AED_DATA1PTR      = XAMINE_DATA1PTR
	AED_DATA2PTR      = XAMINE_DATA2PTR
	AED_DATA4PTR      = XAMINE_DATA4PTR


	RETURN
	END
	SUBROUTINE AEDSTART(ISTAT)
 
C+
C 
C FUNCTIONAL DESCRIPTION:	
C 
C    AEDSTART:
C	This subroutine provides AEDTSK compatibility functionality with
C       XAMINE.  We emulate the AEDSTART subroutine by calling XAMINE_START.
C 
C FORMAL PARAMETERS:
C  
C     integer istat:
C       Returned status of the attempt to start Xamine.
C  
C 
C-
	IMPLICIT NONE
	INCLUDE 'aedshare.for'
	INTEGER istat
 
              
              
 
	istat = F77XAMINE_START()
	RETURN
	END
	SUBROUTINE AEDSTOP(istat)
 
C+
C 
C FUNCTIONAL DESCRIPTION:	
C 
C    AEDSTOP:
C	This subroutine emulates the AEDTSK AEDSTOP function for XAMINE by
C       calling the Xamine_stop function
C 
C FORMAL PARAMETERS:
C  
C     integer istat:
C	    Status of the attempt.
C  
C 
C-
	IMPLICIT NONE
	INCLUDE 'aedshare.for'
	integer istat
 
              
              
 
	istat = F77XAMINE_STOP()
	RETURN
	END
C
C     The commented functions are only implemented in VMS.
C
C	SUBROUTINE AEDREAD(INPUT, ISIG, ISTATE)
C	ISTAT = 1
C	RETURN
C	END
C
C	SUBROUTINE AEDWAIT(ISIZE, ICODE)
C	INTEGER WAIT
C	ICODE = WAIT(ICODE)
C	RETURN
C	END
C
C	SUBROUTINE AEDCAN(ICAN)
C	ICAN = 1
C	RETURN
C	END

C
C		Since Xamine now uses pipes to move gates back and
C   		forth between the client and itself, the functions which
C		used to deal directly with the AEDBLOCKS data area
C		remain as stubs.
C
	SUBROUTINE AEDBLOCKS
	RETURN
	END

	LOGICAL FUNCTION AEDGETONE(LAST)
	AEDGETONE = .FALSE.
	RETURN
	END

	LOGICAL FUNCTION AEDFREEONE(LAST)
	AEDFREEONE = .TRUE.
	RETURN
	END

	LOGICAL FUNCTION AEDENTER(NSPEC, NAME, ITYPE, NUM, PTS)
C
C Functional Description:
C	AEDENTER:
C		This function enters a gate in Xamine's internal database
C		of gates.  This maps to a call to f77Xamine_EnterGate.
C Formal Parameters:
C	INTEGER NSPEC:
C		The spectrum number on which the gate will be put.
C	INTEGER NAME:
C		Maps the the Xamine gate ident.
C	INTEGER ITYPE:
C		The type of gate being entered.  We have cleverly chosen
C		Xamine gate types to map directly to Xamine_gatetype's.
C	INTEGER NUM:
C		Number of points in the gate.
C	INTEGER PTS(2,*):
C		The points themselves where 1,i is x and 2,i is y.
C Returns:
C	True if worked, and False if Failed.
CCCC
	INCLUDE 'aedshare.for'
	INTEGER NSPEC,NAME, ITYPE, NUM, PTS(2,*)
	INTEGER STATUS
	CHARACTER*80 GateName
	WRITE(GateName, '(A, I5)') 'AEDTSK Gate ID ', name
	status = f77Xamine_EnterGate(nspec, name, itype, GateName, num, pts)
	AEDENTER = (Status .GE. 0)
	RETURN
	END
C
C		AEDTEXT - Lables are not implemented in Xamine at this time.
C
	LOGICAL FUNCTION AEDTEXT(NSPEC, NAME, ITYPE, IX, IY, TEXT)
	PRINT *,'Warning -- AEDTEXT unsupported function called'
	AEDTEXT = .TRUE.
	RETURN 
	END

	LOGICAL FUNCTION AEDDELETE(NSPEC, NAME, ITYPE)
C
C Functional Description:
C   AEDDELETE:
C      This function requests that Xamine remove a gate from the gate
C      table.  We use f77xamine_removegate to do the dirty work.
C Formal Parameters:
C     INTEGER NSPEC:
C	The spectrum nubmer from wich the gate should be deleted.
C     INTEGER NAME:
C	The integer gate identifier of the gate.
C     INTEGER ITYPE:
C	The type of gate to delete.
C Returns:
C	TRUE   - Success
C	FALSE  - Failure.
C
	INCLUDE 'aedshare.for'
	INTEGER NSPEC, NAME, ITYPE
	INTEGER STATUS

	AEDDELETE = (F77Xamine_RemoveGate(nspec, name, itype) .GE. 0)

	RETURN
	END
C
C		AEDLOCATE is not supported by Xamine since the
C		histogrammer only has indirect access to Xamine's gate
C		database.  This stub will claim not to find any
C		requested entries.
C
	LOGICAL FUNCTION AEDLOCATE(NSPEC, NAME, ITYPE, LAST)
	PRINT *,' WARNING -- AEDLOCATE Unsupported function called'
	AEDLOCATE = .FALSE.
	RETURN
	END

	INTEGER FUNCTION AEDSTARTSEARCH(NSPEC)	
C
C Functional Description:
C   AEDSTARTSEARCH:
C      This function invokes f77Xamine_StartSearch to begin listing the
C      set of gates that are defined on a spectrum.
C Formal Parameters:
C   INTEGER NSPEC:
C      The spectrum we're asking about.
C Returns:
C      Non zero search context.  If a zero is returned, then the search could
C      not be started, most likely because the spectrum number was garbage.
C
	INCLUDE 'aedshare.for'
	INTEGER NSPEC

	AEDSTARTSEARCH = F77Xamine_StartSearch(nspec, istat)

	RETURN
	END

	LOGICAL FUNCTION AEDNEXT1(ICTX, NAME, ITYPE)
C
C Functional Description:
C  AEDNEXT1:
C    Returns some limited information about the next gate in the search
C    context returned by AEDSTARTSEARCH.  Note that additional information
C    can be retrieved using the native binding.
C Formal Parameters:
C   INTEGER ICTX:
C     The Context variable returned by AEDSTARTSEARCH or f77Xamine_StartSearch
C   INTEGER NAME:
C      The integer identifier of the gate.
C   INTEGER ITYPE:
C      The type of gate.
C  Returns:
C      True if there is a next gate, False if there isn't.
C
	INCLUDE 'aedshare.for'
	INTEGER ictx, name, itype

	INTEGER  npts, points(2,50)

	AEDNEXT1 = f77Xamine_NextGate(ictx, name, itype, npts, points)	

	RETURN
	END

	SUBROUTINE AEDENDSEARCH(ICTX)
C
C  Functional Description:
C     AEDENDSEARCH:
C       This function completes a search.  Dynamic storage that was allocated
C       by AEDSTARTSEARCH is deleted and the context variable is set in such
C 	a way that the program will fail if it is used prior to being
C	reset via AEDSTARTSEARCH.
C Formal Parameters:
C   INTEGER ICTX:
C	Context value retunred from AEDSTARTSEARCH.
C NOTE:
C   It is not necessary to 'complete' the search prior to calling this function
C
	INCLUDE 'aedshare.for'
	INTEGER ictx

	CALL F77Xamine_EndSearch(ictx)
	ICTX = 0

	RETURN
	END

