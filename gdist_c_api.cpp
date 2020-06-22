#include "gdist_c_api.h"


void compute_gdist_impl(
    unsigned number_of_vertices,
    unsigned number_of_triangles,
    double *vertices,
    int *triangles,
    unsigned number_of_source_indices,
    unsigned number_of_target_indices,
    unsigned *source_indices_array,
    unsigned *target_indices_array,
    double *distance,
    double distance_limit
) {

    std::vector<double> points (vertices, vertices + number_of_vertices);
    std::vector<unsigned> faces (triangles, triangles + number_of_triangles);
    std::vector<unsigned> source_indices (source_indices_array, source_indices_array + number_of_source_indices);
    std::vector<unsigned> target_indices (target_indices_array, target_indices_array + number_of_target_indices);

    geodesic::Mesh mesh;
    mesh.initialize_mesh_data(points, faces); // create internal mesh data structure including edges

    geodesic::GeodesicAlgorithmExact algorithm(&mesh); // create exact algorithm for the mesh
    
    std::vector<geodesic::SurfacePoint> all_sources, stop_points;

    for (unsigned i = 0; i < number_of_source_indices; ++i) {
        all_sources.push_back(geodesic::SurfacePoint(&mesh.vertices()[source_indices[i]]));
    }

    for (unsigned i = 0; i < number_of_target_indices; ++i) {
        stop_points.push_back(geodesic::SurfacePoint(&mesh.vertices()[target_indices[i]]));
    }

    algorithm.propagate(all_sources, distance_limit, &stop_points);

    for (unsigned i = 0; i < stop_points.size(); ++i) {
        algorithm.best_source(stop_points[i], distance[i]);
    }
}

double* local_gdist_matrix_impl(
    unsigned number_of_vertices,
    unsigned number_of_triangles,
    double *vertices,
    unsigned *triangles,
    unsigned *sparse_matrix_size,
    double max_distance
) {
    std::vector<double> points (vertices, vertices + number_of_vertices);
    std::vector<unsigned> faces (triangles, triangles + number_of_triangles);
    
    geodesic::Mesh mesh;
    mesh.initialize_mesh_data(points, faces); // create internal mesh data structure including edges
    geodesic::GeodesicAlgorithmExact algorithm(&mesh); // create exact algorithm for the mesh
    std::vector <unsigned> rows_vector, columns_vector;
    std::vector <double> data_vector;

    double distance = 0;

    std::vector<geodesic::SurfacePoint> targets(number_of_vertices), source;

    for (unsigned i = 0; i < number_of_vertices; ++i) {
        targets[i] = geodesic::SurfacePoint(&mesh.vertices()[i]);
    }
    for (unsigned i = 0; i < number_of_vertices / 3; ++i) {
        source.push_back(geodesic::SurfacePoint(&mesh.vertices()[i]));
        algorithm.propagate(source, max_distance, NULL);
        source.pop_back();
        for (unsigned j = 0; j < number_of_vertices / 3; ++j) {
            algorithm.best_source(targets[j], distance);
            if (distance != geodesic::GEODESIC_INF && distance != 0 && distance <= max_distance) {
                rows_vector.push_back(i);
                columns_vector.push_back(j);
                data_vector.push_back(distance);
            }
        }
    }

    double *data;
    data = new double[3 * rows_vector.size()];

    *sparse_matrix_size = rows_vector.size();

    std::copy(rows_vector.begin(), rows_vector.end(), data);
    std::copy(columns_vector.begin(), columns_vector.end(), data + data_vector.size());
    std::copy(data_vector.begin(), data_vector.end(), data + 2 * data_vector.size());

    return data;
}


extern "C" {
    void compute_gdist(
        unsigned number_of_vertices,
        unsigned number_of_triangles,
        double *vertices,
        int *triangles,
        unsigned number_of_source_indices,
        unsigned number_of_target_indices,
        unsigned *source_indices_array,
        unsigned *target_indices_array,
        double *distance,
        double distance_limit
    ) {
        compute_gdist_impl(
            number_of_vertices,
            number_of_triangles,
            vertices,
            triangles,
            number_of_source_indices,
            number_of_target_indices,
            source_indices_array,
            target_indices_array,
            distance,
            distance_limit
        );
    }

    double* local_gdist_matrix(
        unsigned number_of_vertices,
        unsigned number_of_triangles,
        double *vertices,
        unsigned *triangles,
        unsigned *sparse_matrix_size,
        double max_distance
    ) {
        return local_gdist_matrix_impl(
            number_of_vertices,
            number_of_triangles,
            vertices,
            triangles,
            sparse_matrix_size,
            max_distance
        );
    }
};


// The wrapper to the underlying C function
static PyObject *py_local_gdist_matrix(PyObject *self, PyObject *args) {
	unsigned number_of_vertices;
    unsigned number_of_triangles;
    double *vertices;
    unsigned *triangles;
    unsigned *sparse_matrix_size;
    double max_distance;

	// The ':compute_gdist' is for error messages
	if (!PyArg_ParseTuple(
        args, "dd|i:local_gdist_matrix", &number_of_vertices, &number_of_triangles,
        &vertices, &triangles, &sparse_matrix_size, &max_distance))
		
        return NULL;

	// Call the C function
	local_gdist_matrix(number_of_vertices, number_of_triangles,
        vertices, triangles, sparse_matrix_size, max_distance);

	Py_RETURN_NONE;
}

// The wrapper to the underlying C function
static PyObject *py_compute_gdist(PyObject *self, PyObject *args) {
    printf("I am herecpp");
	unsigned number_of_vertices;
    unsigned number_of_triangles;
    double *vertices;
    int *triangles;
    unsigned number_of_source_indices;
    unsigned number_of_target_indices;
    unsigned *source_indices_array;
    unsigned *target_indices_array;
    double *distance;
    double distance_limit;

	// The ':compute_gdist' is for error messages
	if (!PyArg_ParseTuple(
        args, "dd|i:compute_gdist", &number_of_vertices, &number_of_triangles,
        &vertices, &triangles, &number_of_source_indices, &number_of_target_indices,
        &source_indices_array, &target_indices_array, &distance, &distance_limit))
		
        return NULL;

	// Call the C function
	compute_gdist(number_of_vertices, number_of_triangles,
        vertices, triangles, number_of_source_indices, number_of_target_indices,
        source_indices_array, target_indices_array, distance, distance_limit);

	Py_RETURN_NONE;
}



// Method definition object for this extension, these arguments mean:
// ml_name: The name of the method
// ml_meth: Function pointer to the method implementation
// ml_flags: Flags indicating special features of this method, such as
//          accepting arguments, accepting keyword arguments, being a
//          class method, or being a static method of a class.
// ml_doc:  Contents of this method's docstring
static PyMethodDef gdist_module_methods[] = {
    {   
        "compute_gdist",
        py_compute_gdist,
        METH_VARARGS,
        "Method to calculate geodesic distances."
    },
    {
        "local_gdist_matrix",
        py_local_gdist_matrix,
        METH_VARARGS,
        "Method to calculate local geodesic distances."
    },
    {NULL, NULL, 0, NULL}
};

// Module definition
// The arguments of this structure tell Python what to call your extension,
// what it's methods are and where to look for it's method definitions
static struct PyModuleDef gdist_module_definition = { 
    PyModuleDef_HEAD_INIT,
    "gdist2",
    "Method to calculate geodesic distances.",
    -1, 
    gdist_module_methods
};


PyMODINIT_FUNC PyInit_gdist(void) {
    Py_Initialize();
    return PyModule_Create(&gdist_module_definition);
}
