import numpy as np

import gdist


def test_equality_with_stable():
    res = np.loadtxt('data/outer_skull_642/gdist_matrix.txt')
    vertices = np.loadtxt(
        'data/outer_skull_642/vertices.txt',
        dtype=np.float64,
    )
    triangles = np.loadtxt(
        'data/outer_skull_642/triangles.txt',
        dtype=np.int32,
    )
    ret = gdist.local_gdist_matrix(
        vertices=vertices,
        triangles=triangles,
    )
    ret = ret.toarray()
    np.testing.assert_array_almost_equal(res, ret)
