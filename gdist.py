from ctypes import POINTER, addressof, CDLL, c_int, c_uint, c_double
import glob

import numpy as np
import scipy

libfile = glob.glob('build/*/gdist*.so')[0]

lib = CDLL(libfile)

lib.compute_gdist.argtypes = [
    c_uint,
    c_uint,
    np.ctypeslib.ndpointer(dtype=np.float64),
    np.ctypeslib.ndpointer(dtype=np.int32),
    c_uint,
    c_uint,
    np.ctypeslib.ndpointer(dtype=np.int32),
    np.ctypeslib.ndpointer(dtype=np.int32),
    c_double,
]

lib.compute_gdist.restype = POINTER(c_double)

lib.local_gdist_matrix.argtypes = [
    c_uint,
    c_uint,
    np.ctypeslib.ndpointer(dtype=np.float64),
    np.ctypeslib.ndpointer(dtype=np.int32),
    POINTER(c_uint),
    c_double,
]
lib.local_gdist_matrix.restype = POINTER(c_double)


class Gdist(object):
    def compute_gdist(
        self,
        number_of_vertices,
        number_of_triangles,
        vertices,
        triangles,
        number_of_source_indices,
        number_of_target_indices,
        source_indices_array,
        target_indices_array,
        distance_limit
    ):
        return lib.compute_gdist(
            number_of_vertices,
            number_of_triangles,
            vertices,
            triangles,
            number_of_source_indices,
            number_of_target_indices,
            source_indices_array,
            target_indices_array,
            distance_limit
        )

    def local_gdist_matrix(
        self,
        number_of_vertices,
        number_of_triangles,
        vertices,
        triangles,
        max_distance,
    ):
        sparse_matrix_size = 0
        data_size = lib.local_gdist_matrix(
            number_of_vertices,
            number_of_triangles,
            vertices,
            triangles,
            addressof(c_int(sparse_matrix_size)),
            max_distance
        )
        
        data = np.fromiter(data, dtype=np.float64, count=sparse_matrix_size)
        return data


def compute_gdist(
    vertices,
    triangles,
    source_indices=None,
    target_indices=None,
    max_distance=10**100,
):
    vertices = vertices.ravel()
    triangles = triangles.ravel()
    source_indices = source_indices.ravel()
    target_indices = target_indices.ravel()

    g = Gdist()
    distance = g.compute_gdist(
        number_of_vertices=vertices.size,
        number_of_triangles=triangles.size,
        vertices=vertices,
        triangles=triangles,
        number_of_source_indices=source_indices.size,
        number_of_target_indices=target_indices.size,
        source_indices_array=source_indices,
        target_indices_array=target_indices,
        distance_limit=max_distance
    )
    return np.fromiter(distance, dtype=np.float64, count=target_indices.size)


def local_gdist_matrix(
    vertices,
    triangles,
    max_distance = 10**100
):
    vertices = vertices.ravel()
    triangles = triangles.ravel()
    rows = POINTER(c_uint)()
    columns = POINTER(c_uint)()
    data = POINTER(c_double)()

    g = Gdist()
    data = g.local_gdist_matrix(
        vertices.size,
        triangles.size,
        vertices,
        triangles,
        max_distance
    )
    print(data)

    # return scipy.sparse.csc_matrix(
    #     (data, (rows, columns)), shape=(vertices.size, vertices.size)
    # )
