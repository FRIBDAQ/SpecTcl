Feature: Event processing pipeline

  SpecTcl uses an event processing pipeline to map data from some raw 
  representation to tree parameters. The concept is that each event 
  processor has available to it the raw data representation as well 
  as some subset of the tree parameters it needs ether as input or 
  output or both.   Since tree parameters bind many to one to 
  underyling SpecTcl parameters,  event processors just declare 
  the tree paramter structs that make sense for them and use them 
  directly.

   This feature contains acceptance tests for
   * attaching event processors to the event processing pipeline
   * editing the event processing pipeline.
   * the ability of those event processors to receive data from the 
     pipeline manager.
   * the ability of pipeline elements to share data through tree 
     parameters
   * the ability of pipline elements to establish datastore 
     elements that are shared amongst pipeline elements.

Background:
    Given the file "test.evt" 
    | First line of text  |
    | Second line of text |
    | last line           |
    And an empty script file named "pipetest.tcl"
    When "spectcl" is loaded

Scenario:  Adding an event processor to the pipeline
	   Given "null" is loaded
	   And "null" is appended to the event pipeline
	   And the event pipeline is listed
	   Then running "tclsh <pipetest.tcl 2>&1" gives "null"
	   

Scenario: Removing an event processor from the pipeline
	   Given "null" is loaded
	   And "null" is appended to the event pipeline
	   And "null" is deleted from the event pipeline
	   And the event pipeline is listed
	   Then running "tclsh <pipetest.tcl 2>&1" gives ""


Scenario: Adding an event processor at a specified location in the pipeline
	  Given "null" is loaded
	  And "null" is appended to the event pipeline
          And "null1" is added to the event pipeline before "null"
	  And the event pipeline is listed
	  Then running "tclsh <pipetest.tcl 2>&1" gives "null1 null"

Scenario: Event processors receive data from the source
	   Given "catdecoder" is loaded
	   And "cat" is appended to the event pipeline
	   And the data source "test.evt" is attached
	   And after it exits
	   When I run "tclsh" using "pipetest.tcl" as input and "test.out" as stderr
	   Then the contents of "test.out" should match those of "test.evt"

Scenario: Event processors can fill tree parameters
          Given "catdecoder" is loaded
	  And "wc" is loaded
          And "tpdump" is loaded
	  And "wc" is appended to the event pipeline
	  And "tpdump" is appended to the analysis pipeline
          And the data source "test.evt" is attached
          And after it exits
	  When I run "tclsh" using "pipetest.tcl" as input and "test.out" as stderr
	  Then the contents of "test.out" should be
          | wordcount: 4 |
          | wordcount: 4 |
          | wordcount: 2 |

          

#Scenario: Event processors share data via the data registry 3