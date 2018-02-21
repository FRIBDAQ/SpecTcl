package provide vmusbconstants 1.0

# Define the device types so the decoder can be selected:

set typeCAEN    0;			# CAEN V775,785,792,862
set typeHYTEC   1;			# Hytec NADC 2530.
set typeMADC32  2;			# Mesytec MADC 32.
set typeTDC1x90 3;                      # CAEN V1x90.
set typeV977    4;                      # CAEN V977 input register.
set typeMase    5;                      # MASE XLM subsystem.
set typeCAENDual 6;			# CAEN dual range modules.
set typeHINP     7;			# HINP XLM module.
set typePSD      8;			# PSD XLM module.
set typeV1729    9;			# CAEN V1729 FADC.
set typeMTDC32  10;                     # Mesytec MTDC 32.
set typeVMUSB    null;                  # Used to flag this makes no data.

set channelCount($typeCAEN)   4096
set channelCount($typeHYTEC)  8192
set channelCount($typeMADC32) 4096;	# Currently only 12 chans.
set channelCount($typeTDC1x90) 16384;   # for now this is the # of channels in a tdc spec
set channelCount($typeV977)    16;      # for a bit mask spec
set channelCount($typeMase)    8192;    # Spectrum channels for MASE.
set channelCount($typeHINP)    16384;	# Num channels in a default HINP spectrum.
set channelCount($typePSD)     8192;	# Num channels in a default PSD spectrum.
set channelCount($typeV1729)   8192;	# Waveform count.
set channelCount($typeMTDC32)  16384;   # MTDC32 default resolution
