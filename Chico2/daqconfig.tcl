for {set i 0} {$i < 32} {incr i} {
    lappend anodeChargeThresholds 0
}

caenv792 create anodeCharge 0x30000000
caenv792 config anodeCharge -thresholds $anodeChargeThresholds \
    -supressrange false -geo 1

tdc1x90 create anodeTimes -base 0x10000000 
tdc1x90 config anodeTimes -vsn 2 -termination on -tagtime on
tdc1x90 config anodeTimes -refchannel 24 -window 5 -offset -4 -channelcount 128
tdc1x90 config anodeTimes -edgeresolution 100ps -leresolution 100ps -deadtime 100ns 
tdc1x90 config anodeTimes  -encapsulatechip true -maxhits infinite -depth 1
tdc1x90 config anodeTimes -triggerrelative true

tdc1x90 create cathodeTimes -base 0x20000000 
tdc1x90 config cathodeTimes -vsn 3 -termination on -tagtime on
tdc1x90 config cathodeTimes -refchannel 80 -window 12 -offset -3  -channelcount 128
tdc1x90 config cathodeTimes -edgeresolution 100ps -leresolution 100ps -deadtime 100ns 
tdc1x90 config cathodeTimes -encapsulatechip true -maxhits infinite -depth 1
tdc1x90 config cathodeTimes -triggerrelative true

v1495sc create scaler 0x40000000

stack create event
stack config event -trigger nim1 -delay  20
stack config event -modules [list anodeCharge anodeTimes cathodeTimes]
 
# stack config event -modules [list anodeCharge  cathodeTimes] 


stack create scalers
stack config scalers -trigger scaler -period 2 -modules [list scaler]


##
#  Name the anode charge  parameters: anode.nn
#  There are only 20 channels in use.
#
for {set i 0} {$i < 20} {incr i} {
    set name [format anodeQ.%02d $i]
    lappend adcChannels(anodeCharge) $name
}

##
#  There are 20 anode times in the anodeTimes tdc:
#  Channel 20 (numbered from 0) is going to be the reference time.
#  We're only keeping the first hit on each channel.
#
#
for {set i 0} {$i < 21} {incr i} {
    set name [format anodeT.%02d $i]
    lappend adcChannels(anodeTimes) $name
}

##
#  There are 80 channels of cathode time + 1 ref channel. Again we're only keeping
#  the first hit on each channel.  We use the same naming
#  System.
#
for {set i 0} {$i < 81} {incr i} {
    set name [format cathodeT.%02d $i]
    lappend adcChannels(cathodeTimes) $name
}
