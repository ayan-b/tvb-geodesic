import ctypes
import glob
import os
import sys

import numpy as np
import scipy.sparse


if sys.platform == 'win32':
    libfile = glob.glob('build/*/gdist_c_api.dll')[0]
    libfile = os.path.abspath(libfile)
    lib = ctypes.CDLL(libfile)
elif sys.platform == 'darwin':
    libfile = glob.glob('build/*/gdist*.dylib')[0]
    lib = ctypes.CDLL(libfile)
else:
    libfile = glob.glob('build/*/gdist*.so')[0]
    lib = ctypes.CDLL(libfile)

lib.local_gdist_matrix.argtypes = [
    ctypes.c_uint,
    ctypes.c_uint,
    np.ctypeslib.ndpointer(dtype=np.float64),
    np.ctypeslib.ndpointer(dtype=np.uint32),
    ctypes.POINTER(ctypes.c_uint),
    ctypes.c_double,
]
lib.local_gdist_matrix.restype = ctypes.POINTER(ctypes.c_double)

lib.free_memory.argtypes = [
    ctypes.POINTER(ctypes.c_double),
]
lib.free_memory.restype = None


def local_gdist_matrix(
    vertices,
    triangles,
    max_distance=1e100,
):
    vertices = vertices.ravel()
    triangles = triangles.ravel()
    sparse_matrix_size = ctypes.c_uint(0)
    data = lib.local_gdist_matrix(
        vertices.size,
        triangles.size,
        vertices,
        triangles,
        ctypes.byref(sparse_matrix_size),
        max_distance,
    )
    np_data = np.fromiter(
        data,
        dtype=np.float64,
        count=3 * sparse_matrix_size.value,
    )
    lib.free_memory(data)
    assert np_data.size % 3 == 0
    sizes = np_data.size // 3
    rows = np_data[:sizes]
    columns = np_data[sizes: 2 * sizes]
    np_data = np_data[2 * sizes:]
    return scipy.sparse.csc_matrix(
        (np_data, (rows, columns)), shape=(vertices.size // 3, vertices.size // 3)
    )
