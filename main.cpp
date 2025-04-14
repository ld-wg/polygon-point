#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>
#include "geometry.h" // structs polygon e point
#include "desenha.h"  // script gnuplot

/**
 * calcula a orientacao entre 3 pontos (p, q, r)
 *
 * @param p, q, r  pontos
 * @return COLINEAR || ANTIHORARIO || HORARIO
 */
Orientation orientation(Point p, Point q, Point r) {
    // cross product 2D
    long long val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

    if (val == 0) return Orientation::COLINEAR; // colinear

    return (val > 0) ? Orientation::ANTIHORARIO : Orientation::HORARIO;
}

// verifica se o ponto q esta no segmento pr (assumindo colinearidade)
bool on_segment(Point p, Point q, Point r) {
    return (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
            q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y));
}

/**
 * verifica se dois segmentos de reta se intersectam.
 *
 * @param p1 primeiro ponto do primeiro segmento
 * @param q1 segundo ponto do primeiro segmento
 * @param p2 primeiro ponto do segundo segmento
 * @param q2 segundo ponto do segundo segmento
 * @return boolean << se os segmentos se intersectam
 */
bool do_intersect(const Point& p1, const Point& q1, const Point& p2, const Point& q2) {
    // verificar as 4 orientacoes necessarias
    Orientation o1 = orientation(p1, q1, p2);
    Orientation o2 = orientation(p1, q1, q2);
    Orientation o3 = orientation(p2, q2, p1);
    Orientation o4 = orientation(p2, q2, q1);
    
    // caso geral: se para cada segmento os dois pontos do outro segmento estao em sentidos opostos os segmentos se cruzam
    if (o1 != o2 && o3 != o4) {
        return true;
    }
    
    // casos especiais: se um ponto tem orientacao colinear com o segmento, checamos se esta dentro dos "limites" do segmento 
    if (o1 == Orientation::COLINEAR && on_segment(p1, p2, q1)) return true;
    if (o2 == Orientation::COLINEAR && on_segment(p1, q2, q1)) return true;
    if (o3 == Orientation::COLINEAR && on_segment(p2, p1, q2)) return true;
    if (o4 == Orientation::COLINEAR && on_segment(p2, q1, q2)) return true;
    
    return false;
}


/**
 * verifica se um poligono e simples (sem auto-intersecoes)
 * um poligono e considerado simples se:
 * 1. tem pelo menos 3 vertices
 * 2. nao possui arestas nao-adjacentes que se interceptam
 */
bool is_simple(const Polygon& poly) {
    int n = poly.vertices.size();
    
    if (n < 3) {
        return false; // poligonos com menos de 3 vertices nao sao simples por definicao
    }

    // verifica se ha intersecoes entre arestas nao-adjacentes
    for (int i = 0; i < n; ++i) {
        Point p1 = poly.vertices[i];
        Point q1 = poly.vertices[(i + 1) % n]; // proximo vertice (% usa vetor como anel)

        // verificar contra todas as outras arestas nao-adjacentes
        for (int j = i + 2; j < n; ++j) {

            if ((j + 1) % n == i) {
                continue; // arestas adjacentes (ultima e primeira)
            }

            Point p2 = poly.vertices[j];
            Point q2 = poly.vertices[(j + 1) % n];

            // verificar interseccao
            if (do_intersect(p1, q1, p2, q2)) {
                return false; // encontrada interseccao entre arestas nao-adjacentes
            }
        }
    }

    return true; // nenhuma interseccao encontrada
}

/**
 * verifica se um poligono simples e convexo
 * um poligono e convexo se todos os angulos internos sao menores ou iguais a 180 graus.
 * matematicamente, isso significa que todas as "viradas" devem ser na mesma direcao.
 */
bool is_convex(const Polygon& poly) {
    int n = poly.vertices.size();
    
    // verificacoes preliminares
    if (n < 3) {
        return false; // nao e um poligono valido
    }
    
    // para um poligono ser convexo, todas as orientacoes entre 3 pontos sequenciais devem ser a mesma
    // primeiro, encontra a primeira orientacao nao-colinear para usar como referencia
    Orientation reference_orientation = Orientation::COLINEAR;
    bool has_orientation = false;
    
    // encontrar a primeira orientacao nao-colinear
    for (int i = 0; i < n && !has_orientation; ++i) {
        Point p1 = poly.vertices[i];
        Point p2 = poly.vertices[(i + 1) % n];
        Point p3 = poly.vertices[(i + 2) % n];
        
        Orientation orient = orientation(p1, p2, p3);
        if (orient != Orientation::COLINEAR) {
            reference_orientation = orient;
            has_orientation = true;
        }
    }
    
    // se todos os pontos sao colineares, consideramos convexo (e um segmento de reta)
    if (!has_orientation) {
        return true;
    }
    
    // agora verificamos se todas as orientacoes sao iguais a referencia ou colineares
    for (int i = 0; i < n; ++i) {
        Point p1 = poly.vertices[i];
        Point p2 = poly.vertices[(i + 1) % n];
        Point p3 = poly.vertices[(i + 2) % n];
        
        Orientation orient = orientation(p1, p2, p3);
        
        // se encontrarmos uma orientacao diferente da referencia e nao-colinear, nao e convexo
        if (orient != Orientation::COLINEAR && orient != reference_orientation) {
            return false;
        }
    }
    
    return true; // todas as orientacoes sao consistentes, o poligono e convexo
}


/**
 * verifica se um ponto esta dentro de um poligono simples
 * 
 * implementa o algoritmo de ray casting (parity method):
 * - traca um raio horizontal a partir do ponto
 * - conta as intersecoes com as arestas do poligono
 * - numero impar de intersecoes: ponto esta dentro
 * - numero par de intersecoes: ponto esta fora
 * - pontos sobre arestas ou vertices sao considerados dentro
 *
 * @param point o ponto a ser verificado
 * @param polygon o poligono a ser testado
 * @return true se o ponto estiver dentro ou sobre o poligono, false caso contrario
 */
bool is_inside(const Point& point, const Polygon& polygon) {
    // poligonos nao-simples ou com menos de 3 vertices nao contem pontos
    if (!polygon.is_simple || polygon.vertices.size() < 3) {
        return false;
    }

    const int n = polygon.vertices.size();

    // comecamos com false e vamos invertendo a variavel a cada interseccao encontrada (%2)
    bool inside = false;
    
    // primeiro, verificar se o ponto esta sobre alguma aresta ou vertice
    for (int i = 0; i < n; i++) {
        const Point& current = polygon.vertices[i];
        const Point& next = polygon.vertices[(i + 1) % n];
        
        // verificar se o ponto esta sobre um vertice
        if (point.x == current.x && point.y == current.y) {
            return true;
        }
        
        // verificar se o ponto esta sobre uma aresta
        if (orientation(current, point, next) == Orientation::COLINEAR && 
            on_segment(current, point, next)) {
            return true;
        }
    }
    
    // implementacao do algoritmo ray casting
    for (int i = 0, j = n - 1; i < n; j = i++) {
        const Point& vi = polygon.vertices[i];
        const Point& vj = polygon.vertices[j];
        
        // condicao para verificar se a aresta cruza o raio horizontal
        bool cross_x_ray = (vi.y > point.y) != (vj.y > point.y);
        
        // se a aresta nao cruza horizontalmente, pule para a proxima aresta
        if (!cross_x_ray) continue;

        // a partir daqui, sabemos que o ponto esta no range vertical do segmento
        
        // verificar se a interseccao esta a direita do ponto
        // caso especial: aresta vertical
        if (vi.x == vj.x) {
            if (vi.x > point.x) {
                inside = !inside;
            }
            continue;
        }
        
        // caso geral: calcular a interseccao
        // evitar divisao por zero (embora ja deveria estar garantido por cross_x_ray)
        if (vj.y - vi.y == 0) continue;
        
        // calcular o ponto x onde a aresta cruza o raio horizontal
        double x_intersect = static_cast<double>(vj.x - vi.x) * (point.y - vi.y) / (vj.y - vi.y) + vi.x;
        
        // se a interseccao esta a direita do ponto, inverte o estado
        if (x_intersect > point.x) {
            inside = !inside;
        }
    }
    
    return inside;
}

/**
 * encontra quais poligonos simples contem cada ponto
 *
 * @param polygons lista de poligonos a serem verificados
 * @param points lista de pontos a serem testados
 * @return vetor de vetores (indexado por ponto) com os ids dos poligonos que contem cada ponto
 */
std::vector<std::vector<int>> find_containing_polygons(
    const std::vector<Polygon>& polygons,
    const std::vector<Point>& points) {
    
    int num_points = points.size();
    std::vector<std::vector<int>> result(num_points);
    
    // para cada ponto, testar inclusao em cada poligono
    for (int i = 0; i < num_points; ++i) {
        const Point& point = points[i];
        
        for (const Polygon& polygon : polygons) {
            // testar apenas em poligonos simples (conforme especificacao)
            if (polygon.is_simple && is_inside(point, polygon)) {
                // adicionar o id do poligono (1-indexed)
                result[i].push_back(polygon.id);
            }
        }        
    }
    
    return result;
}

/**
 * le os dados dos poligonos da entrada padrao
 *
 * @param count numero de poligonos a serem lidos
 * @return um vetor de poligonos lidos da entrada
 */
std::vector<Polygon> read_polygons(int count) {
    std::vector<Polygon> polygons(count);
    
    for (int i = 0; i < count; ++i) {
        polygons[i].id = i + 1;  // indexado em 1 para corresponder a especificacao
        int num_vertices;
        std::cin >> num_vertices;
        
        // le vertices do poligono
        polygons[i].vertices.resize(num_vertices);
        for (int j = 0; j < num_vertices; ++j) {
            std::cin >> polygons[i].vertices[j].x >> polygons[i].vertices[j].y;
        }
        
        // valor padrao, sera atualizado na classificacao
        polygons[i].is_simple = false;
        polygons[i].type = PolygonType::NOT_SIMPLE;
    }
    
    return polygons;
}

/**
 * le os dados dos pontos da entrada padrao
 *
 * @param count numero de pontos a serem lidos
 * @return um vetor de pontos lidos da entrada
 */
std::vector<Point> read_points(int count) {
    std::vector<Point> points(count);
    
    for (int i = 0; i < count; ++i) {
        std::cin >> points[i].x >> points[i].y;
    }
    
    return points;
}

/**
 * classifica os poligonos como simples/nao simples e convexo/nao convexo
 */
void classify_polygons(std::vector<Polygon>& polygons) {
    for (auto& polygon : polygons) {
        // processamento especial para poligonos com menos de 3 vertices
        if (polygon.vertices.size() < 3) {
            polygon.type = PolygonType::NOT_SIMPLE;
            polygon.is_simple = false;
            continue;
        }

        // verificar se o poligono e simples (sem auto-intersecoes)
        polygon.is_simple = is_simple(polygon);
        
        if (!polygon.is_simple) {
            polygon.type = PolygonType::NOT_SIMPLE;
        } else {
            if (is_convex(polygon)) {
                polygon.type = PolygonType::SIMPLE_CONVEX;
            } else {
                polygon.type = PolygonType::SIMPLE_NON_CONVEX;
            }
        }
    }
}

/**
 * imprime os resultados da classificacao de poligonos e contencao de pontos
 *
 * @param polygons vetor de poligonos classificados
 * @param point_containers vetor de vetores com ids dos poligonos que contem cada ponto
 */
void print_results(const std::vector<Polygon>& polygons, 
                   const std::vector<std::vector<int>>& point_containers) {
    // classificacao de poligonos
    for (const auto& polygon : polygons) {
        std::cout << polygon.id << " " << polygon_type_to_string(polygon.type) << std::endl;
    }

    // pontos
    for (size_t i = 0; i < point_containers.size(); ++i) {
        std::cout << (i + 1) << ":";  // Ponto ID comeca em 1
        
        for (int polygon_id : point_containers[i]) {
            std::cout << " " << polygon_id;
        }
        
        std::cout << std::endl;
    }
}

int main() {
    // otimizar entrada/saida
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int m, n; // numero de poligonos e pontos
    std::cin >> m >> n;

    // ler os poligonos da entrada
    std::vector<Polygon> polygons = read_polygons(m);

    // ler os pontos da entrada
    std::vector<Point> points = read_points(n);

    // 1. classificar cada poligono
    classify_polygons(polygons);

    // 2. encontrar poligonos que contem cada ponto
    std::vector<std::vector<int>> point_containers = find_containing_polygons(polygons, points);

    // 3. imprimir resultados
    print_results(polygons, point_containers);

    // 4. desenhar os poligonos e pontos (se houver)
    if (!polygons.empty() || !points.empty()) {
        draw(&polygons, &points);
    }

    return 0;
}
