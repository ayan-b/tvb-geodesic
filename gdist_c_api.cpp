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

    // double distance[target_indices.size()];
    double *distance = (double *)malloc(sizeof(double) * target_indices.size());
    for (int i = 0; i < (int)stop_points.size(); ++i) {
        algorithm.best_source(stop_points[i], distance[i]);
    }
    std::cout << "I am>" << distance[0] << "\n";
    return distance;
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
};
