import os
os.chdir("E:/Development/tvb/tvb-geodesic/test_ctypes")
libname = os.path.abspath(
    os.path.join(os.path.dirname(__file__), "libfoo.so"))
 
from ctypes import CDLL
   
lib = CDLL(libname)

class Foo(object):
    def __init__(self):
        self.obj = lib.Foo_new()

    def bar(self):
        lib.Foo_bar(self.obj)

f = Foo()
f.bar()
