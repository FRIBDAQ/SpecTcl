Given /^an empty script file named "(.*?)"$/ do |filename|
  @script = File.open(filename, "w")
  @skip_lines = 1;
end

When /^"(.*?)" is loaded$/ do |package|
  @script.puts("package require #{package}")
end

Given /^"(.*?)" is appended to the event pipeline$/ do |element|
 @script.puts("::spectcl::pipeline add #{element} to event")
 @skip_lines += 1
end

And /^the event pipeline is listed$/ do
  @script.puts("::spectcl::pipeline list in event")
end

Given /^"(.*?)" is deleted from the event pipeline$/ do |element|
      @script.puts("::spectcl::pipeline remove #{element} in event")
end

Given /^"(.*?)" is added to the event pipeline before "(.*?)"$/ do |element, before|
   @script.puts("::spectcl::pipeline add #{element} before #{before} to event")
   @skip_lines += 1
end

Then /^running "(.*?)" gives "(.*?)"$/ do |command, expectation|
  @script.puts("exit")
  @script.flush
  @script.close
  i = 0
  outputPipe = IO.popen(command, "r") 
  for i in 1..@skip_lines do
     l = outputPipe.gets
  end
  l = outputPipe.gets
  outputPipe.close
  l.should == "% #{expectation}\n"
end


Given /^the data source "(.*?)" is attached$/ do | filename |
  @script.puts("::spectcl::pipeline setdecoder lines")
  @script.puts("::spectcl::attach file://#{filename}")
end

And /^after it exits$/ do 
  @script.puts("after 1000 exit")
  @script.close
end


Given /^"(.*?)" is appended to the analysis pipeline$/ do |element|
      @script.puts("::spectcl::pipeline add #{element} to analysis")
      @skip_lines += 1
end

Then /^the contents of "(.*?)" should be$/ do |filename, table|
     should = table.raw     # Convert table to array.
     f = File.open(filename, "r")
     should.each { | sb_line |
       sb  = sb_line[0] + "\n"
       actual = f.gets
       actual.should == sb
     }
end