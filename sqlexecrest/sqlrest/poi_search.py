# -*- encoding: utf-8 -*-

import ctypes,json
from flask import abort, make_response


class LibConfig(ctypes.Structure):
    _fields_=[('response_count', ctypes.c_int)]
    def getdict(self):
        return dict((f, getattr(self, f)) for f, _ in self._fields_)


class TimoLibrary(object):
    def __init__(self, config):
        sql_library = ctypes.cdll.LoadLibrary(config['SQL_LIB_PATH'])
        if sql_library == None:
            raise Exception('TIMO_LIB_PATH library is not found.') 
        # init api
        self.InitSqlExec = sql_library.InitSqlExec
        self.InitSqlExec.restype = ctypes.c_void_p
        self.InitSqlExec.argtypes = [ctypes.POINTER(LibConfig), ctypes.c_char_p]
        self.ProcessSqlExec = sql_library.ProcessSqlExec
        self.ProcessSqlExec.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
        self.ProcessSqlExec.restype = ctypes.c_char_p
        self.CloseSqlExec = sql_library.CloseSqlExec
        self.CloseSqlExec.argtypes = [ctypes.c_void_p]
        self.CloseSqlExec.restype = None
        self._sqllib = ctypes.c_void_p()
        # initialize config
        libconfig = LibConfig()
        libconfig.response_count = config['SQL_LIB_RESPONSE_COUNT']
        self._sqllib = self.InitSqlExec(ctypes.byref(libconfig), config['SQL_LIB_DATA_PATH'])
        if self._sqllib == None:
            raise Exception('initial failed.')
    
    def __enter__(self):
        return self
    
    def __exit__(self):
        self.CloseSqlExec(ctypes.c_void_p(self._sqllib))  
        
    def close(self):
        self.CloseSqlExec(ctypes.c_void_p(self._sqllib))
    
    def decoding2json(self, data):
        #process
        response = ''
        try:
            if self._sqllib is not None:
                response = self.ProcessSqlExec(ctypes.c_void_p(self._sqllib), data)
        except:
            abort(make_response("Process Error", 500))         

        #print response
        return response
    
    
    
    