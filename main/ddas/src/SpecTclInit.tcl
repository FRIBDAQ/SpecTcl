set DisplayMegabytes 200

# Use QtPy GUI. Comment out for Xamine.
set DisplayType qtpy

# REST interface configuration:
set NonDAQHTTPDPort 6675
set NonDAQMirrorPort 5565
 
lappend auto_path [file join $SpecTclHome TclLibs]
if {[array names env DAQTCLLIBS]  ne ""} {
    lappend auto_path $env(DAQTCLLIBS)
    package require DAQService
    set HTTPDPort [SpecTcl::getServicePort SpecTcl_REST]
    set MirrorPort [SpecTcl::getServicePort SpecTcl_MIRROR]
} else {
    set HTTPDPort $NonDAQHTTPDPort
    set MirrorPort $NonDAQMirrorPort
}
