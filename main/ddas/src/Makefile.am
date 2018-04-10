
FILES =MyParameters.h \
				MyParameters.cpp \
				MyParameterMapper.h \
				MyParameterMapper.cpp \
				MySpecTclApp.h \
				MySpecTclApp.cpp

DESTDIR = @prefix@/DDASSkel


install-data-local:
	${mkinstalldirs} ${DESTDIR}
	for file in ${FILES} ; do ${INSTALL_DATA} $$file ${DESTDIR} ; done 
	${INSTALL_DATA} Makefile_user ${DESTDIR}/Makefile

