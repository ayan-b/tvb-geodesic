#include <iostream>
#include <fstream>

#include "geodesic_library/geodesic_algorithm_exact.h"


double* compute_gdist_impl(
    unsigned number_of_vertices,
    unsigned number_of_triangles,
    double *vertices,
    int *triangles,
    unsigned number_of_source_indices,
    unsigned number_of_target_indices,
    unsigned *source_indices_array,
    unsigned *target_indices_array,
    double distance_limit = geodesic::GEODESIC_INF
) {

    std::vector<double> points (vertices, vertices + number_of_vertices);
    std::vector<unsigned> faces (triangles, triangles + number_of_triangles);
    std::vector<unsigned> source_indices (source_indices_array, source_indices_array + number_of_source_indices);
    std::vector<unsigned> target_indices (target_indices_array, target_indices_array + number_of_target_indices);

    geodesic::Mesh mesh;
    mesh.initialize_mesh_data(points, faces); // create internal mesh data structure including edges

    geodesic::GeodesicAlgorithmExact algorithm(&mesh); // create exact algorithm for the mesh
    
    std::vector<geodesic::SurfacePoint> all_sources, stop_points;

    for (unsigned index: source_indices) {
        all_sources.push_back(geodesic::SurfacePoint(&mesh.vertices()[index]));
    }

    for (unsigned index: target_indices) {
        stop_points.push_back(geodesic::SurfacePoint(&mesh.vertices()[index]));
    }

    algorithm.propagate(all_sources, distance_limit, &stop_points);

    double *distance = new double[target_indices.size()];
    for (int i = 0; i < (int)stop_points.size(); ++i) {
        algorithm.best_source(stop_points[i], distance[i]);
    }
    return distance;
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

    std::vector<unsigned> rows_vector, columns_vector;
    std::vector <double> data_vector;

    double distance = 0;

    std::vector<geodesic::SurfacePoint> targets(number_of_vertices), source(1);

    for (int i = 0; i < number_of_vertices; ++i) {
        targets[i] = geodesic::SurfacePoint(&mesh.vertices()[i]);
    }

    for (int i = 0; i < number_of_vertices; ++i) {
        source[0] = geodesic::SurfacePoint(&mesh.vertices()[i]);
        algorithm.propagate(source, max_distance);
        for (int j = 0; j < number_of_vertices; ++j) {
            algorithm.best_source(targets[j], distance);

            if (distance != geodesic::GEODESIC_INF and distance != 0) {
                rows_vector.push_back(i);
                columns_vector.push_back(j);
                data_vector.push_back(distance);
            }
        }
    }

    double *data = new double[data_vector.size() * 3];

    *sparse_matrix_size = data_vector.size();

    std::copy(rows_vector.begin(), rows_vector.end(), data);
    std::copy(columns_vector.begin(), columns_vector.end(), data + data_vector.size());
    std::copy(data_vector.begin(), data_vector.end(), data + 2 * data_vector.size());

    return data;
}


extern "C" {
    double* compute_gdist(
        unsigned number_of_vertices,
        unsigned number_of_triangles,
        double *vertices,
        int *triangles,
        unsigned number_of_source_indices,
        unsigned number_of_target_indices,
        unsigned *source_indices_array,
        unsigned *target_indices_array,
        double distance_limit = geodesic::GEODESIC_INF
    ) {
        return compute_gdist_impl(
            number_of_vertices,
            number_of_triangles,
            vertices,
            triangles,
            number_of_source_indices,
            number_of_target_indices,
            source_indices_array,
            target_indices_array,
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
