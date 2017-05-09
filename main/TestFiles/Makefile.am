#removed 		-I@top_srcdir@/Tape 

commonCPPFLAGS= -I@top_srcdir@/Core \
				-I@top_srcdir@/Utility \
				@LIBTCLPLUS_CFLAGS@	\
				@TCL_CPPFLAGS@


# Removed 		@top_builddir@/Tape/libtape.la 

commonLDADD = @top_builddir@/Core/libTclGrammerApp.la \
		@LIBTCLPLUS_LDFLAGS@ \
		@TCL_LIBS@


noinst_PROGRAMS = testfile # ltwrite

testfile_SOURCES = testfile.cpp
testfile_CPPFLAGS = $(commonCPPFLAGS)
testfile_LDADD= $(commonLDADD)

#ltwrite_SOURCES = ltwrite.cpp
#ltwrite_CPPFLAGS = $(commonCPPFLAGS)
#ltwrite_LDADD = $(commonLDADD)
