FILES = MyCalibrator.cpp MyCalibrator.h MyParameterMapper.cpp MyParameterMapper.h MyParameters.cpp MyParameters.h MySpecTclApp.cpp MySpecTclApp.h SpecTclInit.tcl SpecTclRC.tcl

DESTDIR = @prefix@/DDASSkel

install-data-local:
	${mkinstalldirs} ${DESTDIR}
	for file in ${FILES} ; do ${INSTALL_DATA} @srcdir@/$$file ${DESTDIR} ; done 
	${INSTALL_DATA} Makefile_user ${DESTDIR}/Makefile

EXTRA_DIST = $(FILES)

