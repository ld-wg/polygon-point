#ifndef DESENHA_H
#define DESENHA_H

#include <vector>
#include "geometry.h"

// Funcao para desenhar poligonos e pontos usando gnuplot
// Recebe ponteiros para os vetores de poligonos e pontos
void draw(const std::vector<Polygon>* polygons, const std::vector<Point>* points);

#endif // DESENHA_H 