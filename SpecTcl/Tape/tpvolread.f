	PROGRAM TPVOLREAD
c
c		Test of volume level read routines.
C		We expect a volume with a single file
C		named test.dat which is fixed length 256
C		byte records.
C
C--------------
C
C		As this is an informal test program only major
C		variables are declared:
C	Test_program 1/27/92 1/27/92

	INCLUDE 'MTACCESS.FOR'
	INTEGER FD		! File descriptor from mtopen
	INTEGER VOLUME		! Volume descriptor from volmount
	RECORD /MTITEMLIST/ items(10) ! Item list for mount/open.
	BYTE   data(256)	! Array for data
	CHARACTER*8 vollabel
	CHARACTER*18 filename
	CHARACTER*80 sccsinfo
     1   /' Test_program 1/27/92 @(#)tpvolread.f	2.1 R. Fox NSCL '/
C
C		First open the tape:
C
	FD = F77MTOPEN(0, .FALSE.)	! Open unit 0 for readonly access.
	IF(FD .LT. 0) THEN
	   PRINT *,'Open failed on tape'
	   PRINT *,F77MTGETMSG(geterrno())
	   STOP
	ENDIF
	PRINT *,'Opened'
C		Now mount it.  We'll ask for the volume name to be
C		returned, but we don't require a particular label.
C		Volume is mounted for read access only.
C
	items(1).code 	= VOL_LABEL
	items(1).buffer = %LOC(vollabel)

 	iprotect  = READABLE	
	items(2).code 	= VOL_PROTECT
	items(2).buffer	= %LOC(iprotect)

	items(3).code = VOL_ENDLIST

	volume = F77volmount(fd, items)	 
	IF(volume .EQ. 0) THEN
	   PRINT *,'Mount of volume failed'
	   PRINT *,F77MTGETMSG(geterrno())
	   STOP
	ENDIF
	PRINT *,'Volume ', vollabel, ' mounted '
C
C		Open the next file on tape.
C		Note that we could use FILE_NAMEREQ to request a particular
C		file.
C
	items(1).code 	= FILE_NAME
	items(1).buffer	= %LOC(filename)

	items(2).code   = FILE_RECLEN
	items(2).buffer = %LOC(ireclen)

	items(3).code   = FILE_BLOCK
	items(3).buffer = %LOC(iblocklen)

	items(4).code   =  FILE_ENDLIST
	istat = F77volopen(volume, items)
	IF(istat .NE. 0) THEN
	  PRINT *,'Open failed'
	  PRINT *,F77mtgetmsg(istat)
	ENDIF
	TYPE *,'Opened file: ', filename
	TYPE *,'Recordsize = ', ireclen
	TYPE *,'Blocksize  = ', iblocklen
C
C		Now we read the file checking the contents.
C		If there is an error other than MTEOF, then we
C		abort, other wise we exit the loop cleanly.
C
	i = 0
100	CONTINUE
	   DO j = 1,256			! Zero data array each time.
	      data(j) = 0
	   END DO
C
	   istat = F77VOLREAD(volume, data, 256, ibytes)
	   IF(istat .EQ. MTEOF) THEN	! End file check.
	      PRINT *,i,' records processed'
	      GOTO 200
	   ENDIF
	   IF(istat .NE. 0) THEN	! Read fail check.
	      PRINT *,'Read failed'
	      PRINT *,F77mtgetmsg(istat)
	      STOP
	   endif
	   i = i + 1
           DO j = 1, 256		! Check data validity.
	     k = j - 1
	     k = IAND(k,'ff'x)
	     l = data(j)
	     l = IAND(l, 'ff'x)
	     IF(k .ne. l) THEN
		TYPE *,' Byte ', j, ' mismatch sb ', k, ' is ', l
	     ENDIF
	   END DO
	GOTO 100
200	CONTINUE			! eof
C
C		Now we close the file and dismount the tape.
C		Nothing to stop us from closing the file and
C		then doing another volopen to process another file,
C		but for the test tape there's only one file.
C
	istat = F77VOLCLOSE(volume)
	IF (istat .NE. 0) THEN
	   print *,'Close failed'
	   PRINT *,F77MTGETMSG(istat)
	   STOP
	ENDIF
C
C		Dismount the tape:
C
	istat = F77VOLDMOUNT(volume)
	IF(istat .NE. 0) THEN
	  print *,'Close failed'
	  PRINT *,F77MTGETMSG(istat)
	  STOP
	ENDIF
	stop 'successful completion'
	END
