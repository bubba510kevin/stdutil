import ctypes

dll = ctypes.CDLL("stdutill.dll")

def DownloadFilee(url , filewrite , outputpath):
    dll.DownloadFile()
    

def free(thing):
    dll.freepy(thing)

def cstmp_execute(command):
    dll.Execute(command)

