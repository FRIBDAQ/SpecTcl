#!/usr/bin/python3
import os
import json
import httplib2

import errno
from socket import error as socket_error

try:
    h = os.environ['RESThost']
    p = os.environ['RESTport']
    key_address = "http://"+h+":"+p+"/spectcl/shmem/key"    
    key = httplib2.Http().request(key_address)[1]    
    var = json.loads(key.decode())
    size_address = "http://"+h+":"+p+"/spectcl/shmem/size" 
    size = httplib2.Http().request(size_address)[1]    
    var2 = json.loads(size.decode())
    print(var['detail'], var2['detail'])
except socket_error as serr:
    if serr.errno != errno.ECONNREFUSED:
        raise serr

