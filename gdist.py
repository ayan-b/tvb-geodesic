import ctypes

import numpy as np
import scipy

lib = ctypes.CDLL('./gdist_c_api.so')


c_double_p = ctypes.POINTER(ctypes.c_double)
c_int_p = ctypes.POINTER(ctypes.c_int)
c_uint_p = ctypes.POINTER(ctypes.c_uint)

class Gdist(object):
    def __init__(self):
        lib.compute_gdist.argtypes = [
            ctypes.c_uint,
            ctypes.c_uint,
            c_double_p,
            c_int_p,
            ctypes.c_uint,
            ctypes.c_uint,
            c_uint_p,
            c_uint_p,
            ctypes.c_double,
        ]
        lib.compute_gdist.restype = c_double_p

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


def compute_gdist(
    vertices,
    triangles,
    source_indices = None,
    target_indices = None,
    max_distance = 1**100,
):
    vertices = vertices.ravel()
    triangles = triangles.ravel()
    source_indices = source_indices.ravel()
    target_indices = target_indices.ravel()
    vertices_size = vertices.size
    triangles_size = triangles.size
    source_indices_size = source_indices.size
    target_indices_size = target_indices.size
    vertices_p = vertices.ctypes.data_as(c_double_p)
    triangles_p = triangles.ctypes.data_as(c_int_p)
    source_indices_p = source_indices.ctypes.data_as(c_uint_p)
    target_indices_p = target_indices.ctypes.data_as(c_uint_p)

    g = Gdist()
    return g.compute_gdist(
        number_of_vertices=vertices_size,
        number_of_triangles=triangles_size,
        vertices=vertices_p,
        triangles=triangles_p,
        number_of_source_indices=source_indices_size,
        number_of_target_indices=target_indices_size,
        source_indices_array=source_indices_p,
        target_indices_array=target_indices_p,
        distance_limit=max_distance
    )


if __name__ == "__main__":
    temp = np.loadtxt("../data/flat_triangular_mesh.txt", skiprows=1)
    vertices = temp[0:121].astype(np.float64)
    triangles = temp[121:321].astype(np.int32)
    src = np.array([1], dtype=np.int32)
    trg = np.array([2], dtype=np.int32)
    distance = compute_gdist(vertices, triangles, source_indices = src, target_indices = trg)
    distance = np.array(np.fromiter(distance, dtype=np.float64, count=trg.size))
    np.testing.assert_array_almost_equal(distance, [0.2])
