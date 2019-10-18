import os
import sipconfig

basename = "CPyConverter"

# The name of the SIP build file generated by SIP and used by the build
# system.
build_file = basename + ".sbf"

# Get the SIP configuration information.
config = sipconfig.Configuration()

# Run SIP to generate the code.
os.system(" ".join([config.sip_bin, "-c", ".", "-b", build_file, basename + ".sip"]))

# Create the Makefile.
makefile = sipconfig.SIPModuleMakefile(config, build_file)

# Add the library we are wrapping.  The name doesn't include any platform
# specific prefixes or extensions (e.g. the "lib" prefix on UNIX, or the
# ".dll" extension on Windows).
makefile.extra_libs = [basename]

# Search libraries in current directory
makefile.extra_lflags= ['-L.']

# Add extra cxxflags to be passed to C++ compiler
makefile.extra_cxxflags = [ "-std=c++11" ]

# Generate the Makefile itself.
makefile.generate()