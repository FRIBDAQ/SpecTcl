C
C  THIS PROGRAM READS 4 STORED 2D SPECTRA FROM 
C  DISK AND DISPLAYS THEM ON THE AED.
	PROGRAM DEMO

	INCLUDE '/daq/include/aedshare.for'
	INCLUDE '/daq/include/aedpublic.for'
	INCLUDE '/daq/include/qtree.for'
	INCLUDE '/daq/include/daqfile.for'
	INCLUDE '/daq/include/daqcvt.for'
C
C  SET UP FOR DISPLAY
C
	INTEGER DAQFILE, QTREE
	CHARACTER*81 NAME
	INTEGER      PTS(2,50)

	CALL AEDINIT(IERR, 8*1024*1024)
	AED_OFFSET(1)=0
	AED_OFFSET(2)=65536
	AED_OFFSET(3)=131072
	AED_OFFSET(4)=196608
	AED_TYPE(1)=3
	AED_TYPE(2)=3
	AED_TYPE(3)=3
	AED_TYPE(4)=3
	AED_XY(1,1)=256
	AED_XY(2,1)=256
	AED_XY(1,2)=256
	AED_XY(2,2)=256
	AED_XY(1,3)=256
	AED_XY(2,3)=256
	AED_XY(1,4)=256
	AED_XY(2,4)=256
	CALL SET_TITLE(1, 'Detector 1')
	CALL SET_TITLE(2, 'Detector 2')
	CALL SET_TITLE(3, 'Detector 3')
	CALL SET_TITLE(4, 'Detector 4')
	DO 10 I=5,AED_ISMAX
	AED_TYPE(I)=0
	AED_XY(1,I)=0
	AED_XY(2,I)=0
10	CONTINUE
C
C  START THE AED TASK
C
	TYPE *,' STARTING AED'
C	CALL AEDBLOCKS
	CALL AEDSTART(IERR)
	print *,'ierr = ',ierr
	IF(IERR .NE. 1)CALL EXIT(IERR)
C
C  GET SPECTRUM FILE 1 AND READ SPEC
	TYPE *,' READING DATA FILE'
	DAQFILE = F77DAQOPEN('demo.qtree')
	IF (DAQFILE .LE. 0) THEN
	   PRINT *,'Could not open datta file demo.qtree'
	   CALL EXIT(DAQFILE)
        ENDIF
	ichan   = 1
C
C          For each spectrum, create/READ a quad tree, 
C          transfer to Xamine holding area and then delete the quadtree:
C
	DO ISP = 1,4
	   QTREE = F77QREAD(DAQFILE)
	   DO J = 0,255
	     DO I = 0,255
	       ic     = F77QGETCHAN(QTREE, I, J)
	       if(ic .GT. '8000'x) ic = IOR(ic, 'ffff0000'x) !SEXT.
               AED_DATA2(ICHAN) = ic
	       ICHAN = ICHAN + 1
             END DO
	   END DO
	   istat =  F77QDELETE(QTREE)
        END DO
20	CONTINUE
C
C         The loop below accepts gates until Xamine exits.
C
	TYPE *,'To Exit, choose Exit from Xamine''s menu.'
	id = 0
100	CONTINUE
	is = f77xamine_pollforgate(1, ispec, itype, name, npts, pts)
	IF(is .LT. 0) THEN
	  TYPE *,'DEMO -- Xamine_pollforgate failed: ', is
	  CALL AEDSTOP(IS)
	  CALL EXIT(IS)
	ENDIF
	IF(is .GT. 0) THEN
	  TYPE *,'DEMO -- got gate: ', name
	  IF(.NOT. AEDENTER(ispec, id, itype, npts, pts)) THEN
	     TYPE *,'DEMO -- Xamine_entergate failed: '
	     CALL AEDSTOP(IS)
	     CALL EXIT(IS)
	  ENDIF
	  id = id + 1
	ENDIF
	IF(f77Xamine_Alive()) GOTO 100
	type *,'DEMO - exiting since Xamine is gone'
	CALL EXIT(0)
	END
	
	SUBROUTINE SET_TITLE(ID, STRING)
C
C          SUBROUTINE TO SET AN AEDTSK/XAMINE TITLE STRING.
C
        INCLUDE '/daq/include/aedshare.for'
	character string*(*)

	ILEN = MIN(LEN(STRING), 72)
	DO I = 1, ILEN
	   AED_TITLE(I, ID) = ICHAR(STRING(I:I))
	END DO
	RETURN
	END
