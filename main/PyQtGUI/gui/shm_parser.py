#!/usr/bin/python3
import os, sys, re
import json
import httplib2

import errno
from socket import error as socket_error

def cleaning(args):
    lst = []
    args = (str(args)).split("'")
    for i, value in enumerate(args):
        if (i%2):
            lst.append(args[i])
    return lst
            
try:
    args = cleaning(sys.argv)
    '''
    try:
        h = os.environ['RESThost']
        p = os.environ['RESTport']
    except:
    '''
    h = args[1]
    p = args[2]
    key_address = "http://"+h+":"+p+"/spectcl/shmem/key"    
    key = httplib2.Http().request(key_address)[1]    
    var = json.loads(key.decode())
    size_address = "http://"+h+":"+p+"/spectcl/shmem/size" 
    size = httplib2.Http().request(size_address)[1]    
    var2 = json.loads(size.decode())
    print(var['detail'], var2['detail'])

except socket_error as serr:
    print("Inside exception socket_error")
    if serr.errno != errno.ECONNREFUSED:
        raise serr

