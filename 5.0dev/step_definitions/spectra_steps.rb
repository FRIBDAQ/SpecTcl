
When /^a "(.*?)" spectrum "(.*?)" is created on "(.*?)"$/ do |type, name, parameter|
     @script.puts("::spectcl::spectrum create #{name} #{type} #{parameter}")
end

When /^spectra are listed to "(.*?)"$/ do |arg1|
     @script.puts("puts stderr [::spectcl::spectrum list]")

end

When /^the parameter "(.*?)" is created from "(.*?)" to "(.*?)" with "(.*?)" bins$/ do |name, low, high, bins|
     @script.puts("::spectcl::treeparameter create #{name} #{low} #{high} #{bins} arbitrary")
end

