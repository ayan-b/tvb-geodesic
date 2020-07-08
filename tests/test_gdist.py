import numpy as np

import gdist


def test_equality_with_stable():
    surface_data = 'inner_skull_642'
    expected = np.loadtxt(f'data/{surface_data}/gdist_matrix.txt')
    vertices = np.loadtxt(
        f'data/{surface_data}/vertices.txt',
        dtype=np.float64,
    )
    triangles = np.loadtxt(
        f'data/{surface_data}/triangles.txt',
        dtype=np.uint32,
    )
    actual = gdist.local_gdist_matrix(
        vertices=vertices,
        triangles=triangles,
    )
    actual = actual.toarray()
    np.testing.assert_array_almost_equal(actual, expected)

def test_flat_triangular_mesh():
    data = np.loadtxt("data/flat_triangular_mesh.txt", skiprows=1)
    vertices = data[0:121].astype(np.float64)
    triangles = data[121:].astype(np.uint32)
    distances = gdist.local_gdist_matrix(vertices, triangles)
    epsilon = 1e-6  # the default value used in `assert_array_almost_equal`
    # test if the obtained matrix is symmetric
    assert (abs(distances - distances.T) > epsilon).nnz == 0
    np.testing.assert_array_almost_equal(distances.toarray()[1][0], 0.2)
    # set max distance as 0.3
    distances = gdist.local_gdist_matrix(vertices, triangles, 0.3)
    # test if the obtained matrix is symmetric
    assert (abs(distances - distances.T) > epsilon).nnz == 0
    assert np.max(distances) <= 0.3

def test_hedgehog_mesh():
    data = np.loadtxt("data/hedgehog_mesh.txt", skiprows=1)
    vertices = data[0:300].astype(np.float64)
    triangles = data[300:].astype(np.uint32)
    distances = gdist.local_gdist_matrix(vertices, triangles)
    epsilon = 1e-6  # the default value used in `assert_array_almost_equal`
    # test if the obtained matrix is symmetric
    assert (abs(distances - distances.T) > epsilon).nnz == 0
    np.testing.assert_array_almost_equal(
        distances.toarray()[1][0], 1.40522
    )
    # set max distance as 1.45
    distances = gdist.local_gdist_matrix(vertices, triangles, 1.45)
    # test if the obtained matrix is symmetric
    assert (abs(distances - distances.T) > epsilon).nnz == 0
    assert np.max(distances) <= 1.45
