FILES = MyParameters.h \
	MyParameters.cpp \
	MyParameterMapper.h \
	MyParameterMapper.cpp \
	MyPipelineData.h \
	MyPipelineData.cpp \
	MyParameters2.h \
	MyParameters2.cpp \
	MyVariables.h \
	MyVariables.cpp \
	MyCalibrator.h \
	MyCalibrator.cpp \
	MySpecTclApp.h \
	MySpecTclApp.cpp

DESTDIR = @prefix@/DDASSkel


install-data-local:
	${mkinstalldirs} ${DESTDIR}
	for file in ${FILES} ; do ${INSTALL_DATA} @srcdir@/$$file ${DESTDIR} ; done 
	${INSTALL_DATA} Makefile_user ${DESTDIR}/Makefile

EXTRA_DIST=$(FILES)

