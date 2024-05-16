## INTRODUCTION

NSCLSpecTcl is a low overhead dynamic histograming framework for
Nuclear Sciecne experimental data.   It was written for the 
National Superconducting Cyclotron Laboratory (NSCL) at Michigan State University.
When the NSCL experimental program finished and the NSCL was succeeded by
the Facility for Rare Isotope Beams (FRIB), the FRIB adopted NSCLSpecTcl
for use in online and early offline analysis.

Note that NSCLSpecTcl should not be confised with the Tcl GUI interface
generated named SpecTcl.  In this document the word ```SpecTcl`` will
be used for brevity to mean NSCLSpecTcl 

NSCLSpecTcl development is now hosted at:

http://github.com/FRIBDAQ/SpecTcl

The documentation for NSCLSpecTcl is at:

http://docs.nscl.msu.edu/daq

Various documents describing SpecTcl are in the section labeled
"What is SpecTcl?" on that web page.

SpecTcl is developed under Debian Linux and has several
dependencies, some are built in, some are expected to be installed
by the system owner.  As the FRIB actively develops NSCLSPecTcl,
it is possible that the list of dependencies below are incomplete.

One can install NSCLSpecTcl in three ways.  These are listed
in decreasing order of our preference for you;

*  By using one of the FRIB supported singularity containers and filesystems
(see https://docs.nscl.msu.edu/daq/newsite/DAQFromContainers.pdf for
a desscription of our container systems and how to install them).
*  By building from a distribution tarball
*  By building from a clone of the repository.

https://github.com/FRIBDAQ/SpecTcl/releases

Provides a set of the releases we would prefer you to use.  Each of those
releases  has several 'assets' or files;  If a release is named
x.y-zzz;  it will have the files:

*  spectcl-x.y-zzz-{container}.tar.gz  Where {container} is the
Debian release for one of our containers.  These are binary distributions
which should be installed in the directory tree that is bound to /usr/opt
from our containers and filesystems  in general it will have a single directory
tree named x.y.zzz which should be unrolled into a directory that will  be
bound to /usr/opt/spectcl (e.g. /usr/opt/spectcl/x.y.zzz).
*  spectcl-x.y-zzz-dist.tar.gz   This is a source distribution generated
by the ```make dist```, see however  DEPENDENCIES below. if you want
to build using this.
*  spectcl-x.y-zzz.{zip,tar.gz}  these are labeled as "Source code (zip): and
"Source code (.tar.gz)" respectivel in the release asset list.   These  cotain
clones of the git repository tag that generated the release.

## Installing for a container:

This installation requires the least knowledge -- once you have your
containerized system up and running.  Suppose you downloaded version 5.13-010's
binary tarball for e.g. the bullseye container.  You would have gottan a file named:

```spectcl-5.13-010-bullseye.tar.gz```

Hand that file to a user that is allowed to write into the directory tree that is bound
into the bullseye container's /usr/opt directory tree.  Suppose they put that file
in their home directory and a script or alias named ```bullseye``` has been defined
to start the container with that directory tree bound to /usr/opt.  Here's how to 
install that new SpecTcl Version:

```bash
cd /usr/opt/spectcl   # if necessary make this directory.
tar xzf ~/spectcl-5.13-010-bullseye.tar.gz
```

Done, you now have NSCLSpecTcl version 5.13-010 installed for use in your
containerized buster system.


## Installing from sources - the git clone.

Unwrap the git clones and cd into the main subdirectory of the resulting tree.

### Installing the 'built in' dependencies.  
SpecTcl depends on a few other FRIB/NSCL products.  Rather than make you build
and install them, we have three scripts that allow you to incorporate them
into the SpecTcl build.  Each script is quite short but you'll want to read the
comments that descdribe the most recently used version/tag of that product.


You will need to have git installed on your system to incorpoprate these.


* The Unified format library provides code that can decode NSCLDAQ data (NSCLDAQ
is the data acquisition system adopted by the FRIB it too was developed at the
NSCL).  Suppose you see the command in the script unifiedfmt-incorp.sh
```
# Most recently used version 1.2
```
run the script as:

```bash
./unifiedfmt-incorp.sh 1.2
```
to incorporate that version of the unified format verison.

* tcl++ Is a C++ wrapping of the Tcl API libraries. SpecTcl uses this
library extensively.  It's incorporation script is tcl++incorp  Suppose you
see the comment:
```
#  e.g. ./tcl++incorp libtclplus-v4.3-001
#  last used in this branch.
#
```
towards the top of the script.  In that case, invoke it as:

```bash
./tcl++incorp libtclplus-v4.3-001
```

* CutiePie is a displayer which allows you to view the spectra created by SpecTcl
during its analysis of your data.  It's incorporation script is qtpi-incorp.sh
If you see a comment like:
```
#  last tag was v1.4
```

Run this script as folllows:
```bash
./qtpi-incorp.sh v1.4
```

Now all of the internal dependencies are incorporated and you can build
SpecTcl as if you got a spectcl-5.13-010-dist tarball and had unwrapped it
into the 'main' directory that is now your working directory.

### Building SpecTcl from a source tarball.

#### External dependencies

These must either be installed via your system's package manager or built 
and installed somewhere on your system.  For each dependency you must 
build I'll describe where the default configuration of SpecTcl expects 
it.  By default I mean that if you install there you don't need
to specify any special options to the configuration script.


*   A version of NSCLDAQ is required to connect SpecTcl to the online system.
There is no default use the configure ```--with-nscldaq-root``` option to 
specify the location of the version of NSCLDAQ you with to use.  Note that
this is only used to locate the helper that SpecTcl uses to get online data.
*   OpenMotif or lesstif.  This is an X11 toolk it used by Xamine the internally 
integrated displayer.  Note that we recommend CutiePie now but Xamine is still
built and provided.  ```--with-motif-header-dir``` and ```--with-motif-lib``` can be used
to describe where the libXm .h file are and th efull path to libXm.
*   libgd is used by Xamine to perfrom hardcopy ```--with-gd-headerdir``` and ```--with-gd-libdir``` 
can be used to specify where this is installed if you choose to build it.
* We also realy on imagemaagick for image conversions the ```imagemagick``` package on
Debian is suittable for this.
* gri is used to produce publication quality plots.  If it is not present, the Xamine just
won't print.  If you build and install it yourself, specify the path to
the executable to configure using ```--with-gri-dir```
* SpecTcl depends on the CERN Root product at http://root.cern.ch  It must be built by you (or
someone).  Specify the top level of the installation directory tree with
```--with-rootsys```
* Some of the SpecTcl utilities interact with the SpecTcl REST server plugin.  They use
restclient-cpp at https://github.com/mrtazz/restclient-cpp  which you must build.  Specify where you
installed it via ```--with-restclient-cpp```
*  We send some REST responses zipped so we need ```zlib1g-dev``` to do this compression.
*  The REST rersponses are parsed using jsoncpp which, in general you can install with your
system's package manager.  For Debian, e.g. you'll want ```libjsoncpp1``` for the libraries and
```libjsoncpp-dev``` for the headers.
* Xamine configuration file processors rely on flex and bison which, again, can ge 
installed with your package manager.
*  Tcl/Tck versions 8.x are needed.  On Debian we use. ```libtcl8.6-dev``` ```libtk8.6-dev``` to get
everything pulled in via apt.
*  Several modules from the Tcl standard library *tcllib* are used and can be pulled in with your
package manager.   On debian this is the ```tcllib``` package.
*  Some of the standard UI scripts make use of itcl/itk and iwidgets on Debian these packages are
```itcl3```, ```itk3``` and ```iwidgets4```
*  We use sqlite for some configuration things so you'll want ```libsqlite3-0``` ```libsqlite3-dev```
and ```libsqlite3-tcl``` on Debian.
*  Documentation is generated with Docbook on Debian you'll want ```xmlto``` on Debian.
*  SpecTcl includes a module that allows it to be programmed in Python. For Debian you need
```python3``` and ```python3-dev```
*  SpecTcl is written in C++ so either g++ or an llvm based C++ compiler are needed.


#### Configuring SpecTcl

If you have all of the external dependencies installed in their normal places you should be able to:

```
./configure --prefix=/where/to/install --with-rootsys=/where/cernroot/is/installed
```

This will trundle along (out of tree builds are also supported).

Once configure worked:

```
make
make all -jn  #n like the number of processors you system has.
make install
VERBOSE=1 make check    # If you want to run tests.
```

and you are done.