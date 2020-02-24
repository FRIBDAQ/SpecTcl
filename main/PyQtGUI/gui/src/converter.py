#!/usr/bin/env python
import re
a = []
x = 0
y = 0
z = 0
with open('Xamine6.ctbl') as color_file:
    for line in color_file:
        #print(line),
        a =re.findall('\d+', str(line))
        #print(a)
        for i in range(len(a)):
            if i == 0:
                x = int(a[i])*255/100
            elif i == 1:
                y = int(a[i])*255/100
            elif i==2:
                z = int(a[i])*255/100
                #            print(x,y,z)
        
        print('#{:02x}{:02x}{:02x}'.format(int(x), int(y), int(z))),
                
