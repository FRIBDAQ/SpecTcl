#
#   Define the parameters:
#
parameter Distribution1 1 10
parameter Distribution2 2 10
parameter Distribution3 3 10
parameter Distribution4 4 10
parameter Distribution5 5 10
parameter dist1+dist2   0 11
#
#  Ordinary spectra:
#
spectrum d1 1 Distribution1 10
spectrum d2 1 Distribution2 10
spectrum d3 1 Distribution3 10
spectrum d4 1 Distribution4 10
spectrum d5 1 Distribution5 10
spectrum gam1 g1 {Distribution1 Distribution2 Distribution3} 10 word
spectrum gam2 g1 {Distribution1 Distribution2 Distribution4} 10 word
spectrum sum 1 dist1+dist2  11
spectrum sumw 1 dist1+dist2 11 word       ;# Word 1-d spectrum.
spectrum d3-vs-sum 2 {Distribution3 dist1+dist2} {8 8}
spectrum d1-vs-sum 2 {Distribution1 dist1+dist2} {8 8}
spectrum d2-vs-d3  2 {Distribution1 Distribution3} {8 8} byte ;# byte 2d spec.
spectrum ad1vsd2    2 {Distribution1 Distribution2} {8 8}
spectrum gam3 g2 {Distribution1 Distribution2 Distribution3 Distribution4} {8 8} word
spectrum gam4 g2 {Distribution1 Distribution2 Distribution3 Distribution4 Distribution5} {8 8} word
spectrum gam5 g2 {Distribution1 Distribution3 dist1+dist2} {8 8} word

#
#  Bit Spectra:
#
spectrum d1bl b Distribution1 4	       ;# 16 bits of bit register (long).
spectrum d1bw b Distribution1 5 word   ;# 32 bits of bit register (word).
spectrum d1sumw s {Distribution1
                  Distribution1
                  Distribution1
                  Distribution2
                  Distribution1
                  Distribution1
                  Distribution1
                  Distribution1 } 10  ;# Summary spectrum of mostly distrib1 (w)

spectrum d1sumb s {Distribution1
                  Distribution1
                  Distribution1
                  Distribution2
                  Distribution1
                  Distribution1
                  Distribution1
                  Distribution1 } 10 byte ;# Summary  mostly distrib1 (b)
#
#   Give spectra a slot in the display
#
sbind -all         ;# bind all spectra.
#
#   Put in some gates:
#

gate Band1  b {Distribution1 Distribution2 { {128 10} {512 64}}}
gate Band2  b {Distribution1 Distribution2 { {128 64} {512 255}}}
gate NotBand1 - Band1
gate Band  * {NotBand1 Band2}
gate Slice s {Distribution1 {400 500}}
gate D2Gate s {Distribution2 {0 200}}
gate AndGate * { Slice D2Gate }
gate NotGate - Slice
gate OrGate  + { Slice NotGate }
gate TRUE    T {}
gate FALSE   F {}
apply Band ad1vsd2
gate Contour c {Distribution1 Distribution2 { {200 200} {300 300} {200 300} {300 200}}}
gate Contour2 c2band { Band1 Band2 }
gate RightEye gc { { {188 712} {192 668} {236 660} {228 716}} gam3}
gate LeftEye gc { { {600 712} {604 648} {640 664} {656 708}} gam3}
gate Nose gc { { {380 560} {328 448} {380 444}} gam3}
gate Mouth gb { { {176 328} {208 224} {256 172} {340 132} {436 144} {516 164} {576 200} {608 276} {624 372}} gam3}
gate REyeBrow gb { { {148 752} {240 784} {272 760}} gam3}
gate LEyeBrow gb { { {568 752} {604 764} {716 736}} gam3}
gate GSlice gs { {400 500} gam1}
apply GSlice gam1
