

lib_LTLIBRARIES = libTclGrammerApp.la

libTclGrammerApp_la_CPPFLAGS = -DINSTALLED_IN='"@prefix@"' -DVERSION='"@VERSION@"' @ROOT_CFLAGS@


Analysis_SRC =  CBufferCallbackList.cpp \
	CBufferProcessor.cpp         \
	CLanguageCallback.cpp        \
	CStandardControlCallback.cpp \
	CStandardScalerCallback.cpp  \
	BufferAnalysis.cpp		\
	CAnalysisBase.cpp \
	CAnalysisEventProcessor.cpp

Analysis_INCLUDES =  CBufferCallback.h \
	CBufferCallbackList.h       \
	CBufferProcessor.h          \
	CLanguageCallback.h         \
	CStandardControlCallback.h  \
	CStandardScalerCallback.h   \
	SRunContext.h	            \
	BufferAnalysis.h	\
	CAnalysisBase.h \
	CAnalysisEventProcessor.h 


DecoderRing_SRC = CRingBufferDecoder.cpp \
											RingFormatHelper.cpp \
											RingFormatHelper10.cpp \
											RingFormatHelper11.cpp \
											RingFormatHelperFactory.cpp \
											RingFormatHelperCreator.cpp \
											RingFormatHelper10Creator.cpp \
											RingFormatHelper11Creator.cpp

DecoderRing_INCLUDES = CRingBufferDecoder.h \
											RingFormatHelper.h \
											RingFormatHelper10.h \
											RingFormatHelper11.h \
											RingFormatHelperFactory.h \
											RingFormatHelperCreator.h \
											RingFormatHelper10Creator.h \
											RingFormatHelper11Creator.h

DecoderRing_noinst_INCLUDES = DataFormatPre11.h DataFormat.h

DisplayInterface_SRC = Display.cpp \
			DisplayFactory.cpp \
			DisplayCollection.cpp \
			DisplayInterface.cpp \
			NullDisplay.cpp \
			TestDisplay.cpp \
			CHistogrammerFitObserver.cpp \
			GateBinderObserver.cpp \
			GatingDisplayObserver.cpp

DisplayInterface_INCLUDES =  Display.h \
		    DisplayFactory.h \
		    DisplayCollection.h \
		    DisplayInterface.h \
		    NullDisplay.h \
		    TestDisplay.h \
		    CHistogrammerFitObserver.h \
		    GateBinderObserver.h \
		    GatingDisplayObserver.h \
		    Subprocess.h


Events_SRC = Analyzer.cpp BufferDecoder.cpp BufferTranslator.cpp \
	Event.cpp EventFormatError.cpp EventList.cpp NSCLBufferDecoder.cpp \
	RunControl.cpp TCLRunFileHandler.cpp TKRunControl.cpp EventSink.cpp \
	EventSinkPipeline.cpp NSCLJumboBufferDecoder.cpp DopeVector.cpp

Events_INCLUDES = Analyzer.h BufferDecoder.h BufferTranslator.h Event.h \
	EventFormatError.h EventList.h EventSink.h EventUnpacker.h \
	NSCLBufferDecoder.h RunControl.h TCLRunFileHandler.h TKRunControl.h \
	TranslatorPointer.h ValidValue.h EventSinkPipeline.h \
	NSCLJumboBufferDecoder.h DopeVector.h

# Removed TapeException.cpp, Tapefile

EventSource_SRC = File.cpp GaussianDistribution.cpp \
	TestFile.cpp PipeFile.cpp  \
	MultiTestSource.cpp CalibrationDistribution.cpp

# Removed TapeFile.h TapeException.h

EventSource_INCLUDES =  DiskFile.h Distribution.h File.h GaussianDistribution.h \
	PipeFile.h  TestFile.h  MultiTestSource.h \
	CalibrationDistribution.h

Filter_SRC =  EventFilter.cpp \
	FilterDictionary.cpp \
	FilterBufferDecoder.cpp \
	FilterEventProcessor.cpp \
	GatedEventFilter.cpp \
	CXdrOutputStream.cpp CXdrInputStream.cpp \
	CXdrMemInputStream.cpp	\
	CFilterOutputStage.cpp CXdrFilterOutputStage.cpp \
	CFilterOutputStageFactory.cpp \
	CXdrFilterOutputStageCreator.cpp

Filter_INCLUDES = EventFilter.h \
								 FilterDictionary.h  \
								 FilterEventProcessor.h \
								 FilterBufferDecoder.h \
								 GatedEventFilter.h    \
								 CXdrOutputStream.h CXdrInputStream.h \
								 CXdrMemInputStream.h	\
								 CFilterOutputStage.h CXdrFilterOutputStage.h \
								 CFilterOutputStageFactory.h \
								 CFilterOutputStageCreator.h \
								 CXdrFilterOutputStageCreator.h

Fits_SRC = CFit.cpp		\
			CFitCreator.cpp		\
			CFitFactory.cpp		\
			CLinearFit.cpp		\
			CLinearFitCreator.cpp	\
			CSpectrumFit.cpp	\
			CFitDictionary.cpp	\
			CGaussianFit.cpp	\
			CGaussianFitCreator.cpp \
			SpectrumDictionaryFitObserver.cpp \
			fitgsl.c

Fits_INCLUDES = CFit.h			\
			CFitCreator.h		\
			CFitFactory.h		\
			CLinearFit.h		\
			CLinearFitCreator.h	\
			CSpectrumFit.h		\
			CFitDictionary.h	\
			fitgsl.h		\
			CGaussianFit.h		\
			CGaussianFitCreator.h	\
			SpectrumDictionaryFitObserver.h

Gates_SRC =  ActualIterator.cpp AndGate.cpp Band.cpp \
	CompoundGate.cpp ConstituentIterator.cpp Contour.cpp Cut.cpp \
	DeletedGate.cpp FalseGate.cpp Gate.cpp GateContainer.cpp \
	GateListIterator.cpp Interior.cpp InteriorRow.cpp Not.cpp OrGate.cpp \
	PointlistGate.cpp PointlistIterator.cpp SingleItemIterator.cpp \
	TrueGate.cpp C2Bands.cpp \
	CGammaCut.cpp CGammaBand.cpp CGammaContour.cpp \
	MaskGates.cpp MaskEqualGate.cpp MaskAndGate.cpp MaskNotGate.cpp \
	CFold.cpp \
	CROI.cpp CEverywhereROI.cpp CContourROI.cpp

Gates_INCLUDES = ActualIterator.h AndGate.h Band.h C2Bands.h CompoundGate.h \
	ConstituentIterator.h Contour.h Cut.h DeletedGate.h FalseGate.h \
	CGammaBand.h CGammaContour.h CGammaCut.h Gate.h GateContainer.h \
	GateListIterator.h Interior.h InteriorRow.h Not.h OrGate.h \
	PointlistGate.h PointlistIterator.h SingleItemIterator.h TrueGate.h \
	MaskGates.h MaskEqualGate.h MaskAndGate.h MaskNotGate.h \
	CFold.h \
	CROI.h CEverywhereROI.h CContourROI.h

Sorter_SRC =  DictionaryException.cpp Histogrammer.cpp Spectrum.cpp \
	Spectrum1DL.cpp Spectrum2DW.cpp Spectrum1DW.cpp Spectrum2DB.cpp \
	BitSpectrumW.cpp BitSpectrumL.cpp SummarySpectrumW.cpp \
	SummarySpectrumB.cpp SpectrumFactory.cpp SpectrumFactoryException.cpp \
	SnapshotSpectrum.cpp Gamma1DW.cpp Gamma1DL.cpp Gamma2DW.cpp \
	CGammaSpectrum.cpp \
	Gamma2DB.cpp GateMediator.cpp CAxis.cpp CParameterMapping.cpp  Parameter.cpp \
	SpectrumS.cpp \
	Spectrum2DL.cpp Gamma2DL.cpp  SummarySpectrumL.cpp \
	CSpectrum2Dm.cpp CSpectrum2DmL.cpp CSpectrum2DmW.cpp CSpectrum2DmB.cpp \
	Gamma2DD.cpp CFlattenedGateList.cpp CSpectrumByParameter.cpp \
	CGammaSummarySpectrum.cpp \
	GatingObserver.cpp

Sorter_INCLUDES = BitSpectrumL.h BitSpectrumW.h Dictionary.h \
	DictionaryException.h Gamma1DL.h Gamma1DW.h Gamma2DB.h Gamma2DW.h \
	GateMediator.h Histogrammer.h \
	Parameter.h SnapshotSpectrum.h Spectrum.h \
	CGammaSpectrum.h \
	Spectrum1DL.h Spectrum1DW.h Spectrum2DB.h Spectrum2DW.h \
	SpectrumFactory.h SpectrumFactoryException.h SummarySpectrumB.h \
	SummarySpectrumW.h CAxis.h CParameterMapping.h SpectrumS.h \
	Spectrum2DL.h  Gamma2DL.h SummarySpectrumL.h \
	CSpectrum2Dm.h CSpectrum2DmL.h CSpectrum2DmW.h CSpectrum2DmB.h \
	Gamma2DD.h DynamicList.h DynamicList.cpp \
	CFlattenedGateList.h CSpectrumByParameter.h CGammaSummarySpectrum.h \
	GatingObserver.h

# TapeCommand.cc removed.

TclGrammerApp_SRC =  StopRun.cpp StartRun.cpp \
	RunControlPackage.cpp ParameterPackage.cpp ParameterCommand.cpp \
	SpectrumPackage.cpp ClearCommand.cpp BindCommand.cpp \
	UnbindCommand.cpp SpectrumCommand.cpp DataSourcePackage.cpp \
	AttachCommand.cpp GateCommand.cpp ApplyCommand.cpp \
	UngateCommand.cpp GatePackage.cpp GateFactory.cpp \
	GateFactoryException.cpp TCLHistogrammer.cpp \
	PseudoCommand.cpp PseudoScript.cpp ChanCommand.cpp WriteCommand.cpp \
	ReadCommand.cpp CRootExec.cpp 	\
	TCLAnalyzer.cpp EventProcessor.cpp TclGrammerApp.cpp \
	Globals.cpp FilterCommand.cpp \
	ScalerProcessor.cpp ScalerProcessorCallbacks.cpp \
	DocumentationException.cpp DocumentationCallback.cpp \
	SpecTcl.cpp CFoldCommand.cpp CProjectionCommand.cpp CFitCommand.cpp \
	IntegrateCommand.cpp RingFormatCommand.cpp VersionCommand.cpp \
	SContentsCommand.cpp CSpectrumStatsCommand.cpp SpecTclDisplayManager.cpp \
	XamineEventHandler.cpp SharedMemoryKeyCommand.cpp SharedMemorySizeCommand.cpp

# TapeCommand.h removed

TclGrammerApp_INCLUDES =  AppInit.h ApplyCommand.h AttachCommand.h BindCommand.h \
	ChanCommand.h ClearCommand.h DataSourcePackage.h EventProcessor.h \
	GateCommand.h GateFactory.h GateFactoryException.h GatePackage.h \
	Globals.h ParameterCommand.h ParameterPackage.h \
	PseudoCommand.h PseudoScript.h ReadCommand.h RunControlPackage.h \
	SpectrumCommand.h SpectrumPackage.h StartRun.h StopRun.h \
	TCLAnalyzer.h TCLHistogrammer.h TclGrammerApp.h \
	UnbindCommand.h UngateCommand.h WriteCommand.h \
	FilterCommand.h \
	ScalerProcessor.h ScalerProcessorCallbacks.h \
	DocumentationException.h DocumentationCallback.h \
	SpecTcl.h CFoldCommand.h CProjectionCommand.h CFitCommand.h \
	IntegrateCommand.h RingFormatCommand.h VersionCommand.h SContentsCommand.h \
	CSpectrumStatsCommand.h SpecTclDisplayManager.h \
	XamineEventHandler.h SharedMemoryKeyCommand.h SharedMemorySizeCommand.h


# TapeCommand.h removed

SpecTcl_SRC =  MySpecTclApp.cpp \
	AppInit.h ApplyCommand.h AttachCommand.h BindCommand.h \
	ChanCommand.h ClearCommand.h DataSourcePackage.h EventProcessor.h \
	GateCommand.h GateFactory.h GateFactoryException.h GatePackage.h \
	Globals.h MySpecTclApp.h ParameterCommand.h ParameterPackage.h \
	PseudoCommand.h PseudoScript.h ReadCommand.h RunControlPackage.h \
	SpectrumCommand.h SpectrumPackage.h StartRun.h StopRun.h \
	TCLAnalyzer.h TCLHistogrammer.h  TclGrammerApp.h \
	UnbindCommand.h UngateCommand.h WriteCommand.h \
	CFoldCommand.h CRootExec.h

Spectra_SRC = SpectraLocalDisplay.cpp SpectraProcess.cpp

Spectra_INCLUDES = SpectraLocalDisplay.h SpectraProcess.h


SpectrumIO_SRC =  SpectrumFormatter.cpp \
	SpectrumFormatterFactory.cpp \
	SpectrumStandardFormatters.cpp \
	NSCLBinarySpectrumFormatter.cpp \
	NSCLAsciiSpectrumFormatter.cpp  \
	SpectrumFormatError.cpp \
	nsclbin.cpp nsclbinerror.cpp nscldata.cpp tclstreams.cpp

SpectrumIO_INCLUDES = NSCLAsciiSpectrumFormatter.h NSCLBinarySpectrumFormatter.h \
	SpectrumFormatError.h SpectrumFormatter.h SpectrumFormatterFactory.h \
	nsclbin.h nsclbinerror.h nscldata.h nscltime.h tclstreams.h

TreeParam_SRC = CMatchingVisitor.cpp \
	CTreeException.cpp \
	CTreeParameter.cpp \
	CTreeParameterArray.cpp \
	CTreeParameterCommand.cpp \
	CTreeVariable.cpp \
	CTreeVariableArray.cpp \
	CTreeVariableCommand.cpp \
	CTreeVariableProperties.cpp \
	ListVisitor.cpp \
	SetAllVisitor.cpp \
	SetChannelsVisitor.cpp \
	SetLimitsVisitor.cpp \
	SetUnitsVisitor.cpp \
	SetWidthVisitor.cpp \
	UncheckVisitor.cpp

TreeParam_INCLUDES =  BindVisitor.h \
	CMatchingVisitor.h \
	CTreeException.h \
	CTreeParameter.h \
	CTreeParameterArray.h \
	CTreeParameterCommand.h \
	CTreeVariable.h \
	CTreeVariableArray.h \
	CTreeVariableCommand.h \
	CTreeVariableProperties.h \
	ListVisitor.h \
	MatchTreeParameter.h \
	ResetVisitor.h \
	SetAllVisitor.h \
	SetChannelsVisitor.h \
	SetLimitsVisitor.h \
	SetUnitsVisitor.h \
	SetWidthVisitor.h \
	UncheckVisitor.h \
	TreeParameter.h

Xamine_SRC =  XamineGate.cpp Xamine1D.cpp Xamine2D.cpp \
	XamineButton.cpp XamineButtonException.cpp XamineButtonPrompt.cpp \
	XamineConfirmPrompt.cpp XamineEvent.cpp XamineFilePrompt.cpp \
	XamineGateException.cpp XamineGates.cpp \
	XamineNoPrompt.cpp XaminePointsPrompt.cpp \
	XamineSpectrumPrompt.cpp  XaminePushButton.cpp \
	XamineSpectrum.cpp XamineSpectrumIterator.cpp \
	XamineTextPrompt.cpp XamineToggleButton.cpp Xamineplus.cpp \
	ProductionXamineShMem.cpp XamineGateFactory.cpp \
	XamineShMemDisplayImpl.cpp

Xamine_INCLUDES = ButtonEvent.h XamineGate.h Point.h Xamine1D.h Xamine2D.h \
	XamineButton.h XamineButtonException.h XamineButtonPrompt.h \
	XamineConfirmPrompt.h XamineEvent.h XamineFilePrompt.h \
	XamineGateException.h XamineGates.h XamineMap1D.h XamineMap2D.h \
	XamineNoPrompt.h XaminePointsPrompt.h XaminePushButton.h \
	XamineSpectrum.h XamineSpectrumIterator.h XamineSpectrumPrompt.h \
	XamineTextPrompt.h XamineToggleButton.h Xamineplus.h \
	XamineSharedMemory.h ProductionXamineShMem.h XamineGateFactory.h \
	XamineShMemDisplayImpl.h


libTclGrammerApp_la_SOURCES = $(Analysis_SRC) \
												$(DecoderRing_SRC) \
												$(DisplayInterface_SRC) \
												$(Events_SRC) \
												$(EventSource_SRC) \
												$(Filter_SRC) \
												$(Fits_SRC) \
												$(Gates_SRC) \
												$(Sorter_SRC) \
												$(TclGrammerApp_SRC) \
												$(Spectra_SRC) \
												$(SpectrumIO_SRC) \
												$(TreeParam_SRC) \
												$(Xamine_SRC)

# Removed -I@top_srcdir@/Tape

libTclGrammerApp_la_CXXFLAGS = -I@top_srcdir@/Utility \
												 -I@top_srcdir@/Display \
												 -I@top_srcdir@/factories \
												 -I@top_srcdir@/Tape \
												 @LIBTCLPLUS_CFLAGS@ \
												 @TCL_CPPFLAGS@
# Removed @top_builddir@/libntape.la

libTclGrammerApp_la_LIBADD = @top_builddir@/Display/libXamine.la \
			 @LIBTCLPLUS_LDFLAGS@ \
			@TCL_LIBS@ @ROOT_LDFLAGS@

libTclGrammerApp_la_LDFLAGS = -version-info $(SOVERSION):0 -lgsl -lgslcblas -lm


include_HEADERS = $(Analysis_INCLUDES) \
												$(DecoderRing_INCLUDES) \
												$(DisplayInterface_INCLUDES) \
												$(Events_INCLUDES) \
												$(EventSource_INCLUDES) \
												$(Filter_INCLUDES) \
												$(Fits_INCLUDES) \
												$(Gates_INCLUDES) \
												$(Sorter_INCLUDES) \
												$(Spectra_INCLUDES) \
												$(TclGrammerApp_INCLUDES) \
												$(SpectrumIO_INCLUDES) \
												$(TreeParam_INCLUDES) \
												$(Xamine_INCLUDES)

noinst_HEADERS = $(DecoderRing_noinst_INCLUDES)


noinst_PROGRAMS =  scalertests docbuftests spectrumPkgTests \
					displayInterfaceTests \
					sorterTests treeParamTests fitTests \
					xamineTests spectraDisplayTests \
					spectclCmdTests

commonCXXFLAGS = -I@top_srcdir@/Core \
				  -I@top_srcdir@/Utility \
				  -I@top_srcdir@/factories \
				  -I@top_srcdir@/Display \
									@LIBTCLPLUS_CFLAGS@ \
									@TCL_CPPFLAGS@ \
									@CPPUNIT_CFLAGS@ \
				@ROOT_CFLAGS@

# Removed @top_builddir@/Tape/libtape.la

commonTestLdFlags = @builddir@/libTclGrammerApp.la \
		@LIBTCLPLUS_LDFLAGS@ \
		@CPPUNIT_LIBS@ \
		@TK_LIBS@ \
		@TCL_LIBS@ @ROOT_LDFLAGS@


scalertests_CXXFLAGS = $(commonCXXFLAGS)
scalertests_LDADD    = $(commonTestLdFlags)
scalertests_SOURCES  = TestRunner.cpp \
											 scalercallbacktests.cpp 	\
											 controlcallbacktests.cpp


docbuftests_CXXFLAGS = $(commonCXXFLAGS)
docbuftests_LDADD    = $(commonTestLdFlags)
docbuftests_SOURCES  = TestRunner.cpp \
											docexceptiontests.cpp \
											callbacktests.cpp

spectrumPkgTests_CXXFLAGS = $(commonCXXFLAGS)
spectrumPkgTests_LDADD    = $(commonTestLdFlags)
spectrumPkgTests_SOURCES  = TestRunner.cpp SpectrumPackageTests.cpp

displayInterfaceTests_CXXFLAGS = $(commonCXXFLAGS)
displayInterfaceTests_LDADD = $(commonTestLdFlags)
displayInterfaceTests_SOURCES = TestRunner.cpp \
								DisplayCollectionTests.cpp \
								DisplayFactoryTests.cpp \
								DisplayInterfaceTests.cpp

sorterTests_CXXFLAGS = $(commonCXXFLAGS)
sorterTests_LDADD = $(commonTestLdFlags)
sorterTests_SOURCES = TestRunner.cpp TestDictObservers.cpp \
					  Test1d.cpp  Test2d.cpp TestSummary.cpp TestBitSpec.cpp  \
					  TestGammaSummary.cpp TestG2dDeluxe.cpp TestGamma1.cpp   \
					  TestGamma2.cpp TestGammaDeluxe.cpp TestHistogrammer.cpp \
						CParameterMappingTest.cpp CAxisTests.cpp \
						CParameterMappingTest.h CAxisTests.h
treeParamTests_CXXFLAGS  = $(commonCXXFLAGS)
treeParamTests_LDADD     = $(commonTestLdFlags)
treeParamTests_SOURCES   = ListTest.cpp       SetWidthTest.cpp          TreeParameterTest.cpp \
	MatchTest.cpp      TestRunner.cpp            TreeVariableArrayTests.cpp \
	SetAllVTest.cpp    TestSuite.cpp             TreeVariableCommandTests.cpp \
	SetChansTest.cpp   TreeExceptionTest.cpp     TreeVariablePropertyTests.cpp \
	SetLimitsTest.cpp  TreeParamArrayTest.cpp    TreeVariableTests.cpp \
	SetUnitsTest.cpp   TreeParamCommandTest.cpp  UncheckTest.cpp  testStubs.cpp \
	TreeTestSupport.h


fitTests_CXXFLAGS = $(commonCXXFLAGS)
fitTests_LDADD = $(commonTestLdFlags)
fitTests_SOURCES = TestRunner.cpp SpectrumDictionaryFitObserverTests.cpp

xamineTests_CXXFLAGS = $(commonCXXFLAGS)
xamineTests_LDADD = $(commonTestLdFlags)
xamineTests_SOURCES = TestRunner.cpp TestXamineShMem.cpp TestXamineShMem.h \
												XamineShMemDisplayImplTests.cpp

spectraDisplayTests_CXXFLAGS = $(commonCXXFLAGS)
spectraDisplayTests_LDADD = $(commonTestLdFlags)
spectraDisplayTests_SOURCES = TestRunner.cpp \
								TestXamineShMem.cpp TestXamineShMem.h \
								SpectraLocalDisplayTests.cpp SpectraProcessTests.cpp

spectclCmdTests_CXXFLAGS = $(commonCXXFLAGS)
spectclCmdTests_LDADD    = $(commonTestLdFlags)
spectclCmdTests_SOURCES  = TestRunner.cpp \
							SharedMemoryKeyCommandTest.cpp \
							SharedMemorySizeCommandTest.cpp

TESTS = scalertests docbuftests spectrumPkgTests displayInterfaceTests \
		sorterTests treeParamTests fitTests xamineTests spectraDisplayTests \
		spectclCmdTests


