package require spectcl
::spectcl::treeparameter create george -1.0 1.0 512 arbitrary
::spectcl::spectrum create george 1d george
puts stderr [::spectcl::spectrum list]
