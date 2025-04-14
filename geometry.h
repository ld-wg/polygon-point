#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <string>

enum class PolygonType {
    NOT_SIMPLE,         // "nao simples"
    SIMPLE_CONVEX,      // "simples e convexo"
    SIMPLE_NON_CONVEX   // "simples e nao convexo"
};

// converter o enum para string para a saida
inline std::string polygon_type_to_string(PolygonType type) {
    switch (type) {
        case PolygonType::NOT_SIMPLE:
            return "nao simples";
        case PolygonType::SIMPLE_CONVEX:
            return "simples e convexo";
        case PolygonType::SIMPLE_NON_CONVEX:
            return "simples e nao convexo";
        default:
            return "desconhecido";
    }
}

enum class Orientation {
    COLINEAR,    // pontos são colineares
    HORARIO,     // sentido horário (clockwise)
    ANTIHORARIO  // sentido anti-horário (counter-clockwise)
};

struct Point {
    long long x, y;
};

struct Polygon {
    int id;
    std::vector<Point> vertices;
    PolygonType type;
    bool is_simple;
};

#endif // GEOMETRY_H 