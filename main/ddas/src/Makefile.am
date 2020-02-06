
FILES =MyParameters.h \
				MyParameters.cpp \
				MyParameters2.h \
				MyParameters2.cpp \
				MyParameterMapper.h \
				MyParameterMapper.cpp \
				MyPipelineData.cpp \
				MyPipelineData.h \
				MyCalibrator.cpp \
				MyCalibrator.h \	
				MySpecTclApp.h \	
				MySpecTclApp.cpp \
				MyVariables.h \
				MyVariables.cpp


DESTDIR = @prefix@/DDASSkel


install-data-local:
	${mkinstalldirs} ${DESTDIR}
	for file in ${FILES} ; do ${INSTALL_DATA} @srcdir@/$$file ${DESTDIR} ; done 
	${INSTALL_DATA} Makefile_user ${DESTDIR}/Makefile

EXTRA_DIST=$(FILES)

