#!/usr/bin/python3
import json
import httplib2

key = httplib2.Http().request("http://localhost:8080/spectcl/shmem/key")[1]
var = json.loads(key.decode())
size = httplib2.Http().request("http://localhost:8080/spectcl/shmem/size")[1]
var2 = json.loads(size.decode())
print(var['detail'], var2['detail'])
