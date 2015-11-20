package require spectcl
package require catdecoder
package require wc
package require tpdump
::spectcl::pipeline add wc to event
::spectcl::pipeline add tpdump to analysis
::spectcl::pipeline setdecoder lines
::spectcl::attach file://test.evt
after 1000 exit
