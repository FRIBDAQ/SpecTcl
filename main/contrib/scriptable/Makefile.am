DEFS = @WCHAR_T_DEF@

bin_PROGRAMS=ScriptedSpecTcl
lib_LTLIBRARIES=libScripted.la

ScriptedSpecTcl_SOURCES = MySpecTclApp.cpp

libScripted_la_LDFLAGS = -version-info $(SOVERSION):0 @ROOT_LDFLAGS@

MainSources=CConfigurableObject.cpp \
	CSegmentUnpacker.cpp \
	CConfigurationParameter.cpp \
	CIntConfigParam.cpp \
	CBoolConfigParam.cpp \
	CStringConfigParam.cpp \
	CPacket.cpp \
	CModuleDictionary.cpp \
	CModule.cpp \
	CModuleCommand.cpp \
	CPacketCreator.cpp \
	CModuleCreator.cpp \
	CScriptableUnpacker.cpp \
	CCAENDigitizerCreator.cpp \
	CCAENV830Creator.cpp \
	CCAENDigitizerModule.cpp \
	CCAENV830Module.cpp CIntArrayParam.cpp \
	CStringArrayparam.cpp 

Main_Objects = CConfigurableObject.o   \
	CSegmentUnpacker.o   \
	CConfigurationParameter.o   \
	CIntConfigParam.o   \
	CBoolConfigParam.o   \
	CStringConfigParam.o   \
	CPacket.o   \
	CModuleDictionary.o   \
	CModule.o   \
	CModuleCommand.o   \
	CPacketCreator.o   \
	CModuleCreator.o   \
	CScriptableUnpacker.o   \
	CCAENDigitizerCreator.o   \
	CCAENV830Creator.o   \
	CCAENDigitizerModule.o   \
	CCAENV830Module.o   CIntArrayParam.o   \
	CStringArrayparam.o 

libScripted_la_SOURCES =  $(MainSources)

# Removed 			    -I@top_srcdir@/Tape


libScripted_la_CPPFLAGS =   -I@top_srcdir@/Utility \
			    -I@top_srcdir@/Display \
			    -I@top_srcdir@/Core \
			    @TCL_CPPFLAGS@ \
			    @TK_CPPFLAGS@ \
			    -DINSTALLED_IN='"$(prefix)"' \
			    @XINCLUDES@ \
			    @LIBTCLPLUS_CFLAGS@ @ROOT_CFLAGS@

noinst_HEADERS =  CBoolConfigParam.h \
	CCAENDigitizerCreator.h \
	CCAENDigitizerModule.h \
	CCAENV830Creator.h \
	CCAENV830Module.h \
	CConfigurableObject.h \
	CConfigurationParameter.h \
	CIntArrayParam.h \
	CIntConfigParam.h \
	CModule.h \
	CModuleCommand.h \
	CModuleCreator.h \
	CModuleDictionary.h \
	CPacket.h CPacketCreator.h \
	CScriptableUnpacker.h \
	CSegmentUnpacker.h \
	CStringArrayparam.h \
	CStringConfigParam.h \
	MySpecTclApp.h \
	CConfigurableObject.h \
	CCAENV1x90Data.h


ScriptedSpecTcl_DEPENDENCIES = libScripted.la

# Removed 	@top_builddir@/Tape/libtape.la 


ScriptedSpecTcl_LDFLAGS =   @top_builddir@/contrib/scriptable/libScripted.la \
	@top_builddir@/Core/libTclGrammerApp.la \
	@top_builddir@/Display/libXamine.la  \
	-lXt -lX11 $(X11EXTRA_LDFLAGS)  \
	$(LD_EXTRA_FLAGS) @ROOT_LDFLAGS@ \
	-lgsl \
	-lgslcblas \
	@LIBTCLPLUS_LDFLAGS@ \
	 @TK_LIBS@ \
	 @TCL_LIBS@ \
	 @XLIBSW@ \
	 -g -Wl,"-rpath=($prefix)/lib"


#removed 			    -I@top_srcdir@/Tape


ScriptedSpecTcl_CPPFLAGS =  -I@top_srcdir@/Utility \
			    -I@top_srcdir@/Display \
			    -I@top_srcdir@/Core \
			    @TCL_CPPFLAGS@ \
			    @TK_CPPFLAGS@ \
			    -DINSTALLED_IN='"$(prefix)"' \
			    @XINCLUDES@ \
			    @LIBTCLPLUS_CFLAGS@ @ROOT_CFLAGS@


#
#  At install time, in order to make this open ended we:
#   Create $(prefix)/contrib/scripted
#          $(prefix)/contrib/scripted/src
#          $(prefix)/contrib/scripted/include
#          $(prefix)/contrib/scripted/skel
#
#    The headers all go in ...include.
#    The .cpp    all go in ...src
#    skel gets:
#                 MySpecTclApp.cpp
#                 MySpecTclApp.h
#        and an extensible  Makefile that's built from Makefile.skel
#        that will, given MySpecTclApp.cpp build the 
#        Scripted SpecTcl.  Extensible means that the user can
#        easily add additional modules to the build.
#

install-exec-local:
	$(mkinstalldirs) $(prefix)/contrib
	$(mkinstalldirs) $(prefix)/contrib/scripted
	$(mkinstalldirs) $(prefix)/contrib/scripted/src
	$(mkinstalldirs) $(prefix)/contrib/scripted/include
	$(mkinstalldirs) $(prefix)/contrib/scripted/skel
	$(INSTALL_DATA) @srcdir@/*.cpp $(prefix)/contrib/scripted/src
	$(INSTALL_DATA) @srcdir@/*.h   $(prefix)/contrib/scripted/include
	$(INSTALL_DATA) @srcdir@/MySpecTclApp.cpp @srcdir@/MySpecTclApp.h \
			      $(prefix)/contrib/scripted/skel
	echo "#-------- Begin auto generated stuff " \
	                                  > Makefile.user
	echo INSTDIR=$(prefix)           >> Makefile.user
	cat @srcdir@/Makefile.skel                >> Makefile.user
	$(INSTALL_DATA) @builddir@/Makefile.user $(prefix)/contrib/scripted/skel/Makefile
	$(INSTALL_DATA) @srcdir@/scriptable_Makefile.include $(sysconfdir)


EXTRA_DIST=Makefile.skel scriptable_Makefile.include 


if CPPUNIT

noinst_PROGRAMS = unittest

unittest_SOURCES = SegUnpackerTest.cpp \
		    ModuleTest.cpp \
		    PacketTest.cpp \
		    CreatorTest.cpp \
		    UnpackerTest.cpp \
		    TestRunner.cpp

unittest_CXXFLAGS= -I@top_srcdir@/Core \
		   -I@top_srcdir@/Utility \
		    @CPPUNIT_CFLAGS@ \
		    @TCL_CPPFLAGS@ \
		    @LIBTCLPLUS_CFLAGS@ @ROOT_CFLAGS@


unittest_DEPENDENCIES = @builddir@/libScripted.la

# Removed 		    @top_builddir@/Tape/libtape.la


unittest_LDADD =   @builddir@/libScripted.la \
		    @top_builddir@/Core/libTclGrammerApp.la \
		    @top_builddir@/Display/libXamine.la @ROOT_LDFLAGS@

unittest_LDFLAGS = \
	@LIBTCLPLUS_LDFLAGS@ \
	@CPPUNIT_LIBS@ \
	$(TCL_LIBS) $(MOTIF_LIBSW) $(XLIBSW)  -lXt -lX11 -lm \
	-lgsl -lgslcblas


#TESTS = ./unittest		#Needs Xamine to fire up :-(
endif
