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
	ISTAT = F77XAMINE_CREATESHAREDMEMORY(IS, %LOC(XAMINE_XY))
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
	INCLUDE 'AEDSHARE.FOR'
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
	INCLUDE 'AEDSHARE.FOR'
	integer istat
 
              
              
 
	istat = F77XAMINE_STOP()
	RETURN
	END
C
C     The commented functions are only implemented in VMS.
C
	SUBROUTINE AEDREAD(INPUT, ISIG, ISTATE)
	INCLUDE 'AEDSHARE.FOR'
	CHARACTER *(*) INPUT

	ISTAT = f77xamine_read(input, isig)
	ISTATE = istat

	RETURN
	END

	SUBROUTINE AEDWAIT(ISIZE, ICODE)
	INCLUDE 'AEDSHARE.FOR'
	
	ICODE = f77xamine_wait(isize)

	RETURN
	END

	SUBROUTINE AEDCAN(ICAN)
	INCLUDE 'AEDSHARE.FOR'
	CALL f77xamine_cancel(ican)
	RETURN
	END

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
C  Functional Description:
C     AEDLOCATE:
CC       This function determines if a particular spectrum/gateid/gatetype
C        triplet is present in Xamine's gate database.
C        The functionality is somewhat different than that of 
C        the AEDtsk function of that name.  Instead of returning a pointer
C        to the gate storage information, we give the relative gate number
C        if found.
C  Formal Parameters:
C     INTEGER NSPEC:
C       The spectrum number to check.
C     INTEGER NAME:
C       The gate ID to look for.
C     INTEGER ITYPE:
C       The gate type to look for.
C     INTEGER LAST [returned]:
C       If the gate is found, this is the relative gate number
C       (number of AEDNEXT1's needed to locate it in a search scan.
C  Returns:
C     TRUE   - Gate found.
C     FALSE  - Gate not found.
	LOGICAL FUNCTION AEDLOCATE(NSPEC, NAME, ITYPE, LAST)
	LOGICAL AEDSTARTSEARCH, AEDNEXT1
	INTEGER CONTEXT
	LOGICAL firstcall/.TRUE./
C
C         Since functionality is different, indicate the call.
C
	IF (firstcall) THEN
	    firstcall = .FALSE.
	    TYPE *, 'XAMINE''s AEDLOCATE function has been called.'
	    TYPE *, '   Note that there are slight differences between the'
	    TYPE *,'    definition of this function for Xamine and AEDTSK.'
	    TYPE *,'    If you are involved in porting this program to'
	    TYPE *,'    Xamine, please carefully evaluate the use of'
	    TYPE *,'    AEDLOCATE to determine if it still does what you need'
	    TYPE *,'    NOTE that calls to AEDLOCATE can be replaced by'
	    TYPE *,'    Calls to AEDSTARTSEARCH, AEDNEXT1 and AEDENDSEARCH'
	    END IF
C
C	    Get the search context.  If NULL, return FALSE.
C
	context = AEDSTARTSEARCH(NSPEC)
	IF (context .EQ. 0) THEN
	    AEDLOCATE = .FALSE.
	    RETURN
	    END IF
C
C	    Iterate through the search until located or done.
C
	last = 0
	DO WHILE (AEDNEXT1(context, n, it))
	    last = last+1
	    IF ((n .EQ. name) .AND. (it .EQ. itype)) THEN
		CALL AEDENDSEARCH(context)
		AEDLOCATE = .TRUE.
		RETURN
		END IF
	    END DO
C
C	    Control passes here if the search failed.  end search and
C           return .false.
C
	CALL AEDENDSEARCH(context)
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

