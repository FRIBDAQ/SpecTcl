      SUBROUTINE PointerSetup
C
C     This subroutine is intended for use with UNIX f77 clients.
C     The pointers to the subsegments of the shared memory region are setup
C     assuming that XAMINE_XYPTR already points to the front of the shared
C     memory region.
C
      INTEGER XAMINE_SPECBYTES
      PARAMETER (XAMINE_SPECBYTES = 1024*1024)
      INCLUDE '/daq/include/xamineshare.for'

      XAMINE_TITLESPTR = XAMINE_XYPTR + XAMINE_ISMAX*2*2
      XAMINE_OFFSETSPTR = XAMINE_TITLESPTR + 72*XAMINE_ISMAX
      XAMINE_TYPESPTR  = XAMINE_OFFSETSPTR + XAMINE_ISMAX * 4
      XAMINE_DATA1PTR  = XAMINE_TYPESPTR + XAMINE_ISMAX * 4
      XAMINE_DATA2PTR  = XAMINE_DATA1PTR
      XAMINE_DATA4PTR  = XAMINE_DATA1PTR

      
      RETURN
      END


      SUBROUTINE SpectrumSetup
C
C     This subroutine sets up a set of test spectra.
C
      INTEGER XAMINE_SPECBYTES
      PARAMETER (XAMINE_SPECBYTES = 1024*1024)
      INCLUDE '/daq/include/xamineshare.for'
      
C
C     Let the Xamine manage the spectrum memory.
C
      CALL F77Xamine_ManageMemory

C
C     Set up a ramp spectrum.
C
      ioffset = F77Xamine_Allocate1d(isp, 64, 'Ramp Spectrum', .TRUE.)
      if (ioffset .EQ. 0) THEN
         type *,'Failed to allocatd RAMP spectrum'
         CALL EXIT(0)
      ENDIF

      DO i = 0, 63
         XAMINE_DATA2(ioffset + i) = i
      end do
         
C
C     Set up a downward ramp spectrum:
C
      ioffset = F77Xamine_Allocate1d(isp, 64, 'Downward Ramp', .TRUE.)
      if(ioffset .EQ. 0) then
         TYpe *,'Failed to allocate downward ramp spectrum'
         CALL EXIT(0)
      endif
      DO i = 0,63
         XAMINE_DATA2(ioffset+i) = 64-i
      END DO

C
C     Set up a step function spectrum:
C
      ioffset = F77Xamine_Allocate1d(isp, 128, 'Step Function', .FALSE.)
      if(ioffset .EQ. 0) THEN
         TYPE *,'Failed to allocate step function'
         CALL EXIT(0)
      endif
      DO i = 0, 127
         ival = 0
         IF(i .lt. 64) ival = 64
         XAMINE_DATA4(ioffset+i) = ival
      END DO

C
C     Set up a 2-d byte spectrum with values z = X+Y above the X=Y line.
C
      ioffset = F77Xamine_Allocate2d(isp, 128,128, 
     *                              'Z=X+Y above X=Y line', .TRUE.)
      DO i = 0,127
         DO j = 0,127
            XAMINE_DATA1(ioffset) = 0
            IF(i .GT. j) XAMINE_Data1(ioffset) = i+j
            ioffset = ioffset+1
         END DO
      EndDo
C
C     Set up a 2-d word spectrum which is a bi-variate gaussian
C        Centroid will be at 60, 100
C        Sigma  is           20, 60
C        Height at peak is   1000
C
      ioffset = F77Xamine_Allocate2d(isp, 256,256,
     1                          'Gaussian C = (60,100), sig = (20,60)',
     2                           .FALSE.)
      DO j = 0, 255
         y = j - 100
         DO i = 0, 255
            x = i  - 60
            value = 1000.0*exp(-x*x/(2*20.0*20.0)) *
     1                     exp(-y*y/(2*60.0*60.0))
            XAMINE_DATA2(ioffset) = VALUE
            ioffset = ioffset + 1
         end do
      end do
      END

      PROGRAM XAMINETEST
      INTEGER XAMINE_SPECBYTES
      PARAMETER (XAMINE_SPECBYTES = 1024*1024)
      INCLUDE '/daq/include/xamineshare.for'
C
C     Gate points
C
      INTEGER ipts(2,50)      

      RECORD /XAMINE_BUTTONDESCRIPTION/ button
      RECORD /XAMINE_EVENT/             event
      
C
C      Gate name.
C
      CHARACTER*80 Gatename

C
C     Create the shared memory region
C
      istat = f77Xamine_CreateSharedMemory(XAMINE_SPECBYTES, 
     *                                     XAMINE_XYPTR)
      IF(istat .NE. 1) THEN
         type *,istat, 'after create shared memory'
         CALL EXIT(0)
      ENDIF

         
C
C     Make the correspondence between the shared memory and f77 defs.
C     then set up the test spectra
C

      CALL PointerSetup
      CALL SpectrumSetup

C
C     Start Xamine
C

      istat = f77Xamine_Start()
      IF(istat .NE. 1) THEN
         TYPE *,istat, 'After Xamine_Start'
         CALL EXIT(0)
      ENDIF
C
C      Set up the button box. (5x5).
C

      istat = f77Xamine_DefineButtonBox(8,8)
      IF(istat .LT. 0) THEN
         TYPE *,istat, ' Could not create the button box.'
         CALL EXIT(0)
      endif
C
C         Exit button
C

      button.button_code = 1
      button.label       = 'Exit'
      button.type        = XAMINE_PUSH
      button.sensitive   = .TRUE.
      button.prompter    = XAMINE_CONFIRMATION
      button.whenavailable = XAMINE_ANYTIME
      button.promptstr   = 'Really exit? '
      istat = f77Xamine_DefineButton(0, 0, button)
      IF(istat .LT. 0) THEN
         type *,istat, ' Could not create the exit button'
         CALL EXIT(0)
      endif

C
C        Toggle button to disable the exit button.
C
      button.button_code = 2
      button.type        = XAMINE_TOGGLE
      button.label       = 'Enable Exit'
      button.prompter    = XAMINE_NOPROMPT
      button.state       = .TRUE.
      istat = f77Xamine_DefineButton(1,0, button)
      if(istat .LT. 0) THEN
         type *,istat,'Could not create enable exit toggle button'
         CALL EXIT(0)
      endif

C
C        Push button to delete selected pane... spectrum must be
C        displayed.
C
      button.button_code = 3
      button.type = XAMINE_PUSH
      button.label       = 'Delete'
      button.whenavailable = XAMINE_INSPECTRUM
      button.promptstr = 'Delete spectrum -- are you sure?'
      istat = f77Xamine_DefineButton(0,1, button)
      IF(istat .LT. 0) THEN
         TYPE *,istat, 'Could not Create DELETE button'
         CALL EXIT(0)
      ENDIF
C
C        Push button to accept some text string.   This will require
C        being in a 1-d spectrum.
C
      button.whenavailable = XAMINE_IN1D
      button.button_code   = 4
      button.label         = 'Text-Input'
      button.prompter      = XAMINE_TEXT
      button.promptstr     = 'Enter some text'
      istat = f77Xamine_DefineButton(0,2,button)
      if(istat .LT. 0) THEN
         TYPE *,istat, 'Could not create Text-input button'
         call EXIT(0)
      ENDIF

C         Push button active in a 2-d spectrum only... choose a spectrum.

      button.whenavailable = XAMINE_IN2D
      button.button_code   = 5
      button.label         = 'Spectrum'
      button.promptstr     = 'Choose spectrum'
      button.prompter      = XAMINE_SPECTRUM
      button.spectype      = XAMINE_COMPATIBLE
      istat = f77Xamine_DefineButton(0,4,button)
      if(istat .Lt. 0) then
         TYPE *,istat, 'Could not create spectrum prompter button'
         CALL EXIT(0)
      ENDIF


C     Push button which prompts for a filename:

      button.whenavailable = XAMINE_ANYTIME
      button.button_code   = 6
      button.label         = 'File'
      button.promptstr     = 'Choose file'
      button.prompter      = XAMINE_FILENAME
      button.spectype      = XAMINE_ANY
      istat = f77Xamine_DefineButton(1, 2, button)
      if(istat .LT. 0) then
         TYPE *, istat, ' Could not create file prompter button'
         CALL EXIT(0)
      ENDIF
      button.maxpts = 10
      button.minpts = 1

C     Push button to get points list.

      button.whenavailable = XAMINE_INSPECTRUM
      button.button_code   = 7
      button.label = 'Points'
      button.promptstr = 'Click in points'
      button.prompter  = XAMINE_POINTS
      istat = f77Xamine_DefineBUtton(1, 4, button)
      IF(istat .lt. 0) Then
         type *,istat, 'Could not create button prompter'
         CALL EXIT(0)
      ENDIF
C
C     Process events until Xamine exits.
C
      igate = 0
      DO WHILE (F77Xamine_Alive())
         istat = f77Xamine_PollForEvent(2, event)
         IF(istat .LT. 0) THEN
            TYPE *,istat, 'Failed to read an event'
            CALL EXIT(0)
         endif
	 IF(istat .EQ. 0) GOTO 2000
C
C        Process gates
C
         IF(event.event .EQ. XAMINE_ISGATE) THEN
            istat = f77Xamine_EnterGate(event.object.spectrum,
     *                                  event.object.id,
     *                                  event.object.object_type,
     *                                  event.object.name,
     *                                  event.object.npts,
     *                                  event.object.points)
            IF(istat .lt. 0) THEN
               TYPE *,istat, ' Failed to enter gate'
               CALL EXIT(0)
            ENDIF


C        Process button presses.
C
C
         ELSE IF(event.event .EQ. XAMINE_ISBUTTON) THEN
            GOTO(100, 200, 300, 400, 500, 
     *           600, 700) event.button.buttonid
            TYPE *,event.button.buttonid,
     *             ' Unrecognized button code.'
            CALL EXIT(0)

C
C             Exit button.
C
 100        CONTINUE
               istat =  f77Xamine_Stop
               CALL EXIT(0)
C
C             Toggle button to enable/Disable EXIT(0)
C
 200        CONTINUE
               IF(event.button.togglestate) THEN
                  istat =  f77Xamine_EnableButton(0,0)
               ELSE
                  istat =  f77Xamine_DisableButton(0,0)
               ENDIF

C             
            GOTO 1000
C
C     Delete spectrum in currently selected pane.
C
 300        CONTINUE
               TYPE *,' Deleting spectrum in current pane'
               ispec = event.button.selected_spectrum
               ioff  = Xamine_Offsets(ispec)
               itype = Xamine_types(ispec)
C                   
C                    Figure out the byte offset of the data:
C
               IF(itype .EQ. XAMINE_ONEDLONG) ioff = ioff * 4
               IF((itype .EQ. XAMINE_ONEDWORD) .OR. 
     *            (itype .EQ. XAMINE_TWODWORD)) ioff = ioff * 2
               CALL F77XAMINE_FREESPECTRUM(ispec)
               CALL F77XAMINE_FREEMEMORY(ioff)
            GOTO 1000
C
C               Got some text.
C          
 400        CONTINUE
               TYPE *,' Received text string "',event.button.text,'"'
            GOTO 1000
C
C              Spectrum choice:
C
 500        CONTINUE
               type *,' Recieved spectrum ', event.button.spectrum
            GOTO 1000

 600        CONTINUE
              TYPE *,'FILE "', event.button.text, '" accepted'
            GOTO 1000
 700        CONTINUE
              TYPE *,'NPTS = ', event.button.npts
              DO j = 1, event.button.npts
                 type *,'Pt ',j , event.button.points(j).x,
     *                            event.button.points(j).y
              enddo
            GOTO 1000
C
C             Exit point of the computed goto.
C
 1000       CONTINUE
C
C        Unrecognized event.
C
         ELSE
            TYPE *,event.event, ' Unrecognized event code'
         ENDIF

2000  CONTINUE
      END DO


      END





