C
C   	Test_program 1/12/93 @(#)tpvolwrite.f	2.2
C
	PROGRAM TPVOLWRITE
	INCLUDE 'MTACCESS.FOR'
	BYTE DATA(256),DATUM
	RECORD /MTITEMLIST/ items(10)
	character *8 label
	CHARACTER *17 FILENAME
	CHARACTER*80 version
     1    /'Test_program 1/12/93 @(#)tpvolwrite.f	2.2 Ron Fox NSCL'/
	integer status, fd
	integer volume
	integer protection
	integer time
C
C		Open unix tape drive zero)
C
	FD = F77MTOPEN(0,.TRUE.)
	IF (fd .LT. 0) THEN
	    print *,'Could not open tape drive'
	    print *,F77MTGETMSG(GETERRNO())
	    STOP
	END IF
	print *,'opened'
C
C		Wait for drive to be mounted:
C
	iok =  F77WTDRIVE(FD)
        if(iok .ne. 0) THEN
           print *,'wtdrive failed'
           stop
        ENDIF
	print *,'ready'
C
C		Init as ANSI volume:
C

	status = F77volinit(fd, 'TEST')
	IF(STATUS .NE. 0) THEN
	  PRINT *,'Unable to init volume'
	  PRINT *,F77MTGETMSG(status)
	  STOP
        ENDIF
	print *,'initted'
C
C		Set up and do the mount.
C
        items(1).code = VOL_LABEL	! Ask for the volume label.
        items(1).buffer = %LOC(label)

	items(2).code = VOL_PROTECT
	protection    = WRITEABLE
	items(2).buffer = %LOC(protection)	! Writeonly volume.

        items(3).code = VOL_ENDLIST	! End of item list.
	volume = F77volmount(fd, items)
        IF(volume .EQ. 0) THEN
          PRINT *,'Unable to mount volume'
          PRINT *,F77mtgetmsg(geterrno())
	  stop
        ENDIF
        print *,'Volume ',label, ' mounted on tape drive'

C		Create a file on the volume

	FILENAME      = 'TEST.DAT'
	items(1).code = FILE_NAMEREQ
	items(1).buffer= %LOC(FILENAME)		! Set filename.
       
        IRECSIZE = 256
	items(2).code = FILE_REQRECLEN
        items(2).buffer = %LOC(irecsize)	! Set recordsize
	

	items(3).code = FILE_ENDLIST
	status = F77volcreate(volume, items)
	IF(STATUS .NE. 0) THEN
           PRINT *,'VOLCREATE FAILED'
	   PRINT *,F77mtgetmsg(status)
	   STOP
	ENDIF
	print *,'file created'

c		Now generate a counting pattern, and write it 100 times
c		on the file.

	do i = 1,256
	  k = i-1
	  datum = IAND(k,'ff'x)
	  data(i) = datum
        end do
	DO i = 1,100
	  status = F77volwrite(volume, data, 256)
          if(status .ne. 0) then
	    print *,'Write number ', i,' failed!!'
	    print *,F77mtgetmsg(status)
	    STOP
	  endif
	end do
	print *,'file written'
C
C		Close file and dismount the volume.
C
	status = F77volclose(volume)
	IF(status .ne. 0) then
	   PRINT *,'VOLCLOSE failed'
	   PRINT *,F77mtgetmsg(status)
           STOP
	ENDIF
	print *,'file closed'

	status = F77voldmount(volume)
	IF(Status .NE. 0) THEN
	   PRINT *,'DISMOUNT failed'
	   PRINT *,F77mtgetmsg(status)
	   STOP
	ENDIF

	stop 'successful completion'
	END 
