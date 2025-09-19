#  Introduction

MVLCSPecTcl is a version of SpecTcl, basedon VMUSBSpecTcl that allows SpecTcl to mechanically unpack
and histogram data taken with the MESYTEC MVLC VME crate contoller using the ```fribdaq-readout```` program.
The slight differences in data format necessiate a separate program.

This document applies to SpecTcl version 7.0-003 and later.

# Supported devices


When VMUSBSpecTcl was written, several devices no longer directly supported by FRIB/NSCLDAQ were, and still are, supported by it.  These devices are not supported by ```fribdaq-readout``` and, therefore, are not supported
by MVLCSpecTcl:

*  The MASE readout system used at Indiana University.
*  The Washington Univerity ASIC readout system, HINP and PSD versions.

Furthermore, the MVLC does not include an internal scaler as the VMUSB does, therefore the VMUSBScaler is not supported.

# Using MVLCSpecTcl

Note that the string ```$SpecTclHome``` refers to the top level directory of a SpecTcl installation that supports MVLCSpecTcl, e.g. ```/usr/opt/spectcl/7.0-003```

The same script infrastructure used to setup VMUSBSpecTcl is used to set up MVLCSpecTcl.  The program itself is in ```$SpecTclHome/bin/MVLCSpecTcl```


## Extending MVLCSpecTcl

The directory ```$SpecTclHome/MVLCSkel``` includes what you need to start extending MVLCSpecTcl.  This includes the following files:

*  CStackUnpacker.{h,cpp} - if you want to add additional unpackers to the software.
*  MySpecTclApp.{h,cpp}  - The SpecTcl main class, in case you want to add additional event processors to the event processing pipeline.
*  Makefile - Makefile (you might need to extend) to build MVLCSpecTcl.
* SpecTclInit.tcl, SpecTclRC.tcl - sample initialization scripts.
