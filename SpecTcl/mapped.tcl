parameter mp1 1 10 {10 100 MeV}
parameter mp2 2 10 {20 200 MeV}
parameter mp3 3 10 {-20 20 cm}
parameter mp4 4 10 {200 800 KeV}

spectrum mspec1.1 1 mp1 {{50 99 100}} long
spectrum mspec1.2 1 mp2 {{100 199 200}} long
spectrum mspec1.3 1 mp3 {{-10 14 250}} long

spectrum mspec2.1 2 {mp1 mp2} {{50 99 100} {20 199 180}} word
spectrum mspec2.2 2 {mp1 mp3} {{50 99 100} {2 6 250}} word
spectrum mspec2.3 2 {mp2 mp3} {{100 199 200} {2 6 250}} word

spectrum mspec3.1 g1 {mp1 mp2} {{10 99 300}} long
spectrum mspec3.2 g1 {mp1 mp2 mp3} {{10 99 300}} long
spectrum mspec3.3 g1 {mp3 mp2 mp1 mp4} {{-10 10 600}} long

spectrum mspec4.1 g2 {mp1 mp2 mp3} {{50 99 512} {80 179 512}} word
spectrum mspec4.2 g2 {mp4 mp1 mp2 mp3} {{210 799 1024} {75 119 1024}} word
spectrum mspec4.3 g2 {mp2 mp3 mp4} {{50 174 1024} {-10 10 1024}} word

spectrum spec1.1 1 mp1 8 long
spectrum spec1.2 1 mp2 10 long
spectrum spec1.3 1 mp3 12 long

spectrum spec2.1 2 {mp2 mp4} {8 8} word
spectrum spec2.2 2 {mp2 mp3} {10 10} word
spectrum spec2.3 2 {mp1 mp2} {12 12} word

spectrum spec3.1 g2 {mp1 mp2 mp3} {9 9} word
spectrum spec3.2 g2 {mp1 mp2 mp3 mp4} {10 10} word
spectrum spec3.3 g2 {mp2 mp3 mp4} {10 10} word

sbind -all
