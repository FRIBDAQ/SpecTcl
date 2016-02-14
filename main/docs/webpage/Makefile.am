DOCDIR = $(srcdir)
DOCDEST = $(prefix)/share/html

SUBDIRS = Commands Programming

install-exec-local:
	$(mkinstalldirs) $(DOCDEST)/Internals
	$(mkinstalldirs) $(DOCDEST)/Internals/_vti_cnf
	$(mkinstalldirs) $(DOCDEST)/_vti_cnf
	$(mkinstalldirs) $(DOCDEST)/_sharedtemplates
	$(mkinstalldirs) $(DOCDEST)/_sharedtemplates/pages
	$(mkinstalldirs) $(DOCDEST)/_sharedtemplates/pages/spectcl.tem
	$(mkinstalldirs) $(DOCDEST)/_sharedtemplates/pages/spectcl.tem/_vti_cnf
	$(mkinstalldirs) $(DOCDEST)/_vti_pvt
	$(mkinstalldirs) $(DOCDEST)/_vti_pvt/_vti_cnf
	$(mkinstalldirs) $(DOCDEST)/images
	$(mkinstalldirs) $(DOCDEST)/images/_vti_cnf
	$(mkinstalldirs) $(DOCDEST)/tailoring
	$(mkinstalldirs) $(DOCDEST)/tailoring/_vti_cnf
	$(mkinstalldirs) $(DOCDEST)/tailoring/examples
	$(mkinstalldirs) $(DOCDEST)/tailoring/examples/_vti_cnf
	$(mkinstalldirs) $(DOCDEST)/tailoring/images
	$(mkinstalldirs) $(DOCDEST)/tailoring/images/_vti_cnf
	$(mkinstalldirs) $(DOCDEST)/tailoring/samplecode
	$(mkinstalldirs) $(DOCDEST)/treeparam
	$(mkinstalldirs) $(DOCDEST)/treeparam/TreeParameter_files
	$(mkinstalldirs) $(DOCDEST)/Plugins
	$(INSTALL_PROGRAM) $(DOCDIR)/Commands/*.* $(DOCDEST)/Commands
	$(INSTALL_PROGRAM) $(DOCDIR)/Commands/_vti_cnf/*.* $(DOCDEST)/Commands/_vti_cnf
	$(INSTALL_PROGRAM) $(DOCDIR)/Internals/*.* $(DOCDEST)/Internals
	$(INSTALL_PROGRAM) $(DOCDIR)/Internals/_vti_cnf/*.* $(DOCDEST)/Internals/_vti_cnf
	$(INSTALL_PROGRAM) $(DOCDIR)/_vti_cnf/*.* $(DOCDEST)/_vti_cnf
	$(INSTALL_PROGRAM) $(DOCDIR)/_sharedtemplates/pages/spectcl.tem/*.* $(DOCDEST)/_sharedtemplates/pages/spectcl.tem
	$(INSTALL_PROGRAM) $(DOCDIR)/_sharedtemplates/pages/spectcl.tem/_vti_cnf/*.* $(DOCDEST)/_sharedtemplates/pages/spectcl.tem/_vti_cnf
	$(INSTALL_PROGRAM) $(DOCDIR)/_vti_pvt/*.* $(DOCDEST)/_vti_pvt
	$(INSTALL_PROGRAM) $(DOCDIR)/_vti_pvt/_vti_cnf/*.* $(DOCDEST)/_vti_pvt/_vti_cnf
	$(INSTALL_PROGRAM) $(DOCDIR)/images/*.* $(DOCDEST)/images
	$(INSTALL_PROGRAM) $(DOCDIR)/images/_vti_cnf/*.* $(DOCDEST)/images/_vti_cnf
	$(INSTALL_PROGRAM) $(DOCDIR)/tailoring/*.* $(DOCDEST)/tailoring
	$(INSTALL_PROGRAM) $(DOCDIR)/tailoring/_vti_cnf/*.* $(DOCDEST)/tailoring/_vti_cnf
	$(INSTALL_PROGRAM) $(DOCDIR)/tailoring/examples/*.* $(DOCDEST)/tailoring/examples
	$(INSTALL_PROGRAM) $(DOCDIR)/tailoring/examples/makefile $(DOCDEST)/tailoring/examples
	$(INSTALL_PROGRAM) $(DOCDIR)/tailoring/examples/_vti_cnf/*.*  $(DOCDEST)/tailoring/examples/_vti_cnf
	$(INSTALL_PROGRAM) $(DOCDIR)/tailoring/examples/_vti_cnf/makefile  $(DOCDEST)/tailoring/examples/_vti_cnf
	$(INSTALL_PROGRAM) $(DOCDIR)/tailoring/images/*.* $(DOCDEST)/tailoring/images
	$(INSTALL_PROGRAM) $(DOCDIR)/tailoring/images/_vti_cnf/*.* $(DOCDEST)/tailoring/images/_vti_cnf
	$(INSTALL_PROGRAM) $(DOCDIR)/tailoring/samplecode/*.* $(DOCDEST)/tailoring/samplecode
	$(INSTALL_PROGRAM) $(DOCDIR)/treeparam/*.* $(DOCDEST)/treeparam
	$(INSTALL_PROGRAM) $(DOCDIR)/treeparam/TreeParameter_files/*.* $(DOCDEST)/treeparam/TreeParameter_files
	$(INSTALL_PROGRAM) $(DOCDIR)/Plugins/*.* $(DOCDEST)/Plugins

htmldir=@prefix@/share/html

html_DATA = parameters.htm HH00706_.jpg programmers_guide.htm gates.htm \
	scripts.htm general_information.htm spectra.htm index.htm users_guide.htm \
	nscl_logo_small.gif obtaining_and_installing.htm  \
	whatsnew.htm whatsnew3.1.htm whatsnew3.2.htm 

EXTRA_DIST =  Internals/chistogrammer.htm \
	Internals/hh00706_.jpg Internals/histog1.gif Internals/histogramming.htm \
	Internals/internals.htm Internals/nscl_logo_small.gif \
	Internals/_vti_cnf/chistogrammer.htm Internals/_vti_cnf/hh00706_.jpg \
	Internals/_vti_cnf/histog1.gif Internals/_vti_cnf/histogramming.htm \
	Internals/_vti_cnf/internals.htm Internals/_vti_cnf/nscl_logo_small.gif \
	_sharedtemplates/pages/spectcl.tem/hh00706_.jpg \
	_sharedtemplates/pages/spectcl.tem/nscl_logo_small.gif \
	_sharedtemplates/pages/spectcl.tem/spectcl.dib \
	_sharedtemplates/pages/spectcl.tem/spectcl.htm \
	_sharedtemplates/pages/spectcl.tem/spectcl.inf \
	_sharedtemplates/pages/spectcl.tem/_vti_cnf/hh00706_.jpg \
	_sharedtemplates/pages/spectcl.tem/_vti_cnf/nscl_logo_small.gif \
	_sharedtemplates/pages/spectcl.tem/_vti_cnf/spectcl.dib \
	_sharedtemplates/pages/spectcl.tem/_vti_cnf/spectcl.htm \
	_sharedtemplates/pages/spectcl.tem/_vti_cnf/spectcl.inf \
	_vti_cnf/HH00706_.jpg _vti_cnf/nscl_logo_small.gif \
	_vti_cnf/scripts.htm _vti_cnf/gates.htm \
	_vti_cnf/obtaining_and_installing.htm  _vti_cnf/spectra.htm \
	_vti_cnf/general_information.htm  _vti_cnf/parameters.htm \
	_vti_cnf/users_guide.htm _vti_cnf/index.htm _vti_cnf/programmers_guide.htm \
	_vti_pvt/botinfs.cnf _vti_pvt/doctodep.btr  _vti_pvt/service.lck \
	_vti_pvt/_x_todo.htm _vti_pvt/bots.cnf _vti_pvt/frontpg.lck \
	_vti_pvt/services.cnf _vti_pvt/_x_todoh.htm _vti_pvt/deptodoc.btr \
	_vti_pvt/linkinfo.cnf _vti_pvt/structure.cnf _vti_pvt/access.cnf \
	_vti_pvt/diskweb.cnf _vti_pvt/service.cnf _vti_pvt/writeto.cnf \
	_vti_pvt/_vti_cnf/_x_todo.htm _vti_pvt/_vti_cnf/_x_todoh.htm \
	images/HH00706_.jpg  images/checkbox51.gif images/hh00706_.jpg \
	images/nscl_logo_small.gif images/gate.h1.gif images/histog2.gif \
	images/undercon.gif images/_vti_cnf/checkbox51.gif \
	images/_vti_cnf/hh00706_.jpg images/_vti_cnf/nscl_logo_small.gif \
	images/_vti_cnf/gate.h1.gif images/_vti_cnf/histog2.gif \
	images/_vti_cnf/undercon.gif tailoring/building.htm tailoring/examples \
	tailoring/modifyin.htm tailoring/samplecode tailoring/supporti.htm \
	tailoring/accessin.htm tailoring/commands.htm tailoring/obtainin.htm \
	tailoring/spectcl.htm tailoring/tailoring.htm \
	tailoring/_vti_cnf/accessin.htm tailoring/_vti_cnf/commands.htm \
	tailoring/_vti_cnf/obtainin.htm tailoring/_vti_cnf/supporti.htm \
	tailoring/_vti_cnf/building.htm tailoring/_vti_cnf/modifyin.htm \
	tailoring/_vti_cnf/spectcl.htm tailoring/_vti_cnf/tailoring.htm \
        tailoring/eventprocessor.htm tailoring/pipeline.htm tailoring/make.htm \
	tailoring/images/spectc1.jpg tailoring/images/spectc2.jpg \
	tailoring/images/_vti_cnf/spectc1.jpg tailoring/images/_vti_cnf/spectc2.jpg \
	treeparam/TreeParameter.html \
	treeparam/TreeParameter_files/image001.jpg \
	treeparam/TreeParameter_files/image003.jpg \
	treeparam/TreeParameter_files/image005.png \
	treeparam/TreeParameter_files/image007.png \
	treeparam/TreeParameter_files/image009.png \
	treeparam/TreeParameter_files/image011.png \
	treeparam/TreeParameter_files/image013.jpg \
	treeparam/TreeParameter_files/image015.jpg \
	treeparam/TreeParameter_files/image017.png \
	Plugins/writing.htm
