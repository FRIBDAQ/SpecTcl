Feature: User can attach data sources

  SpecTcl 5.0 and later take data from some data source
  and feed them into an analysis pipeline.  The analysis pipeline
  is then responsible for doing the remainder of the work on the
  data.
 
  This feature focuses on the acceptance tests for attaching data sources.



Scenario: Attach to existing file
   Given the file "test.evt"
    | First line of text  |
    | Second line of text |
    | last line of text   |
   And the file "attach.tcl"
   | package require spectcl           |
   | package require catdecoder        |
   | spectcl::pipeline setdecoder cat  |
   | spectcl::attach file://test.evt   |
   | after 1 exit                    |
   When I run "tclsh" using "attach.tcl" as input and "test.out" as stderr
   Then the contents of "test.out" should match those of "test.evt"

Scenario: Attach to nonexistent file
   Given the file "attach.tcl"
   | package require spectcl                     |
   | package require catdecoder                  |
   | spectcl::pipeline setdecoder cat            |
   | spectcl::attach file:///no/such/file/at/all |
   | exit                                        |
   When I run "tclsh" using "attach.tcl" as input and "test.output" as stdout
   Then after skipping "2" lines the contents of "test.output" should be "% CFileDataSource::onAttach open failed for: ///no/such/file/at/all : No such file or directory"

   



   