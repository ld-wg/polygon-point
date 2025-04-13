#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

struct Point {
    int x, y;
};

struct Polygon {
    int id;
    std::vector<Point> vertices;
    std::string classification; // "nao simples", "simples e nao convexo", "simples e convexo"
    bool is_simple = false; // Helper flag
};

int main() {
    int m, n; // Number of polygons and points
    std::cin >> m >> n;

    std::vector<Polygon> polygons(m);
    for (int i = 0; i < m; ++i) {
        polygons[i].id = i + 1;
        int num_vertices;
        std::cin >> num_vertices;
        polygons[i].vertices.resize(num_vertices);
        for (int j = 0; j < num_vertices; ++j) {
            std::cin >> polygons[i].vertices[j].x >> polygons[i].vertices[j].y;
        }
    }

    std::vector<Point> points(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> points[i].x >> points[i].y;
    }

    // --- Placeholder for Algorithm Implementations ---
    // 1. Classify each polygon
    for (int i = 0; i < m; ++i) {
        // TODO: Implement simplicity check
        // TODO: Implement convexity check
        // Assign classification based on checks
        // For now, let's assign a placeholder
        polygons[i].classification = "simples e convexo"; // Placeholder
         polygons[i].is_simple = true; // Placeholder
    }

    // 2. Find containing polygons for each point
    std::vector<std::vector<int>> point_containers(n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            // Only check simple polygons
            if (polygons[j].is_simple) {
                // TODO: Implement point-in-polygon test
                bool contains = false; // Placeholder
                if (/* point i is inside polygon j */ contains) {
                    point_containers[i].push_back(polygons[j].id);
                }
            }
        }
        std::sort(point_containers[i].begin(), point_containers[i].end());
    }

    // --- Output Results ---
    // Part 1: Polygon classifications
    for (int i = 0; i < m; ++i) {
        std::cout << polygons[i].id << " " << polygons[i].classification << std::endl;
    }

    // Part 2: Point containment
    for (int i = 0; i < n; ++i) {
        std::cout << (i + 1) << ":";
        for (size_t k = 0; k < point_containers[i].size(); ++k) {
            std::cout << " " << point_containers[i][k];
        }
        std::cout << std::endl;
    }

    return 0;
}
