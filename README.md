FRIB SpecTcl is a histogramming program with a low learning curve.  The
embedded Tcl/Tk scripting language makes layering graphical user interfaces
a simple process. An alternative based on Python bindings for C++ is offered. 
It adopts common Python libraries for basic and more advanced numerical algorithms. The GUI is written on PyQt5 for modularity and extensibility.

### News ####
* 5/11/2023 - SpecTcl has won a SourceForge community award.


### About releases:

   FRIB releases are versioned using version names of the form ```M.m-eee``` where *M* is a major version number, *m* a minor version number and *eee* is an edit or patch level.  This version numbering system is similar in nature to the semantic versioning and you can think of ```M.m-eee``` as corresponding to a semantic version number of ```M.m.eee```
   
   Releases will, in general, have several files.  The files labeled 'Source Files' are generated by github.  The files named something like spectcl-M.m-eee-dist.tar.gz are source tarballs generated by us using:
   
   ```make dist```

Files named spectcl-M.m-eee-distro.tar.gz are binary tarballs that can be rolled into the /usr/opt filesystem of containerized environment using the FRIB containers for the 'distro'  environment e.g. ```spectcl-5.13-003-bullseye.tar.gz``` is a binary distribution for the Debian bullseye container image for version 5.13-003.

