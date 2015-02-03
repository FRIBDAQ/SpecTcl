set ConfigDir /home/fox/see/config

puts "Reading hardware/parameter definitions"
source $ConfigDir/hardware.tcl

puts "Reading spectrum definitions."
source $ConfigDir/spectra.tcl

puts "Reading operational parameters "
source $ConfigDir/opparams.tcl

puts "Reading SpecTcl SEE interface code"
source $ConfigDir/interface.tcl

sbind -all

puts "Configured and ready to go"

