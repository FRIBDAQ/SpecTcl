Feature: Spectra

SpecTcl 5.0 has a very flexibly internal spectrum model.  This model 
supports spectra of any sort from any histogramming kernel that links into C/C++
such as old style SpecTcl spectra, Root spectra or AIDA histograms. 
This feature set tests the ability to create spectra and for those spectra to
increment themselves when they are asked to by the analysis pipeline.

This feature tests:

  * The ability to construct spectra and see that they are indeed 
    constructed (using old fashioned SpecTcl spectra for now.
  * The ability to add an analysis pipelien element for spectra
    and to see that spectra increment appropriately.

Scenario: Creating a simple spectrum allows me to list it.
	  Given an empty script file named "spectra.tcl"
	  When "spectcl" is loaded
	  And the parameter "george" is created from "-1.0" to "1.0" with "512" bins
	  And a "1d" spectrum "george" is created on "george"
	  And spectra are listed to "stderr"
	  When I run "tclsh" using "spectra.tcl" as input and "test.out" as stderr
	  Then the contents of "test.out" should be
         | {george 1d {george} {{-1.0 1.0 512}}} |

