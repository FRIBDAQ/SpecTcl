puts "Creating parameters"

parameter Gaussian 0 10
parameter Dist2 1 10
parameter Another 2  10
parameter Dist4 3 10
parameter Lastone 4 10

puts "listing by name"

puts [parameter -list]

puts  "listing by id"

puts [parameter -list -byid]

puts "listing 'Another'"

puts [parameter -list Another]

puts "listing id=2"

puts [parameter -list -id 2]

puts "deleting Dist4, and id = 1"

parameter -delete Dist4
parameter -delete -id 1
puts [parameter -list]

puts "Use standard 'ParList' function to get formatted list:"
ParList [parameter -list]