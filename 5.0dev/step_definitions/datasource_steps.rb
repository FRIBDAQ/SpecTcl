Given /^the file "(.*?)"$/ do |filename, table| 
  # table is a Cucumber::Ast::Table
  @file_data = table.raw
  f = File.open(filename, "w") 
  @file_data.each { |line|  
       f.puts line
 }
 f.close
end



When /^I run "(.*?)" using "(.*?)" as input and "(.*?)" as stderr$/ do |program, filename, output|
     `#{program} < #{filename}   2> #{output}`
end

When /^I run "(.*?)" using "(.*?)" as input and "(.*?)" as stdout$/ do |program, filename, output|
     `#{program} < #{filename}   > #{output}`
end



Then /^the contents of "(.*?)" should match those of "(.*?)"$/ do | output, correct| 
     o = File.open(output, "r")
     IO.foreach(correct) {|correct_line|
        output_line = o.gets
	output_line.should == correct_line
     }
     o.close
end

Then /^after skipping "(\d*)" lines the contents of "(.*?)" should be "(.*?)"$/ do |skip, filename, contents|
     contents += "\n"
     o = File.open(filename, "r")
     for i in 1..skip.to_i do
       o.gets
     end
     actual = o.gets
     o.close
     actual.should == contents
end