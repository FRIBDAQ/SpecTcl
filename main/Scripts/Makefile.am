scriptsdir = @prefix@/Script

scripts_SCRIPTS = dtree.tcl fileall.tcl gui.tcl handle.tcl \
									listall.tcl save.head tkcon.tcl \
									tree.tcl CopyGates.tcl

testScripts = all.tcl dependsondeleted.test formatgatetest.test \
							gateparamcount.test generalgatetype.test \
							getdependencies.test listify.test \
							markwritable.test setstatus.test \
							validparams.test wrap.test

EXTRA_DIST = $(scripts_SCRIPTS) $(testScripts)


check :
	(cd @srcdir@ ; ./fileall_test )
