import sys
import os
import sipconfig

basename = "CPyConverter"

# The name of the SIP build file generated by SIP and used by the build system.
build_file = basename + ".sbf"

_file = basename + ".sip"

pwdfile = sys.argv[1]+"/"+_file
pwdlib = "-L"+sys.argv[2]+" -lMirrorClient -lXamine -lCPyConverter -Wl,-rpath="+sys.argv[2]
installdir = sys.argv[2]

# Get the SIP configuration information.
config = sipconfig.Configuration()

# Run SIP to generate the code.
os.system(" ".join([config.sip_bin, "-c", ".", "-b", build_file, pwdfile]))
config.default_bin_dir = installdir

# Create the Makefile.
makefile = sipconfig.SIPModuleMakefile(config, build_file,makefile="Makefile.sip")

# Add the library we are wrapping.  The name doesn't include any platform
# specific prefixes or extensions (e.g. the "lib" prefix on UNIX, or the
# ".dll" extension on Windows).
makefile.extra_libs = [basename]

includedir = sys.argv[1]
incdir2 = sys.argv[3]
makefile.extra_include_dirs = [includedir, incdir2]

# Search libraries in ../src directory
makefile.extra_lflags= [pwdlib]

# Add extra cxxflags to be passed to C++ compiler
makefile.extra_cxxflags = [ "-std=c++11" ]

# Generate the Makefile itself.
makefile.generate()
