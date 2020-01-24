#!/usr/bin/python3
import json
import httplib2

class ShmParser:
        def __init__(self, *args, **kwargs):
            self.key = httplib2.Http().request("http://localhost:8080/spectcl/shmem/key")[1]
            self.size = httplib2.Http().request("http://localhost:8080/spectcl/shmem/size")[1]
            self.parameterList = httplib2.Http().request("http://localhost:8080/spectcl/parameter/list")[1]
            self.spectrumList = httplib2.Http().request("http://localhost:8080/spectcl/spectrum/list")[1]            
            self.gateList = httplib2.Http().request("http://localhost:8080/spectcl/gate/list")[1]            

        def parse(self, variable):
            var = json.loads(variable.decode())
            return var['detail']
            
        def get_key(self):
                return ShmParser.parse(self, self.key)

        def get_size(self):
                return ShmParser.parse(self, self.size)
        
        def get_params(self):
                return ShmParser.parse(self, self.parameterList)

        def get_spectra(self):
                return ShmParser.parse(self, self.spectrumList)

        def get_gates(self):
                return ShmParser.parse(self, self.gateList)
        
        def list(self, variable):
                for i in variable:
                        print(i)
                        for key in i:
                                print(key, '->', i[key])
        
