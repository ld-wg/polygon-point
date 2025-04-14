#include "desenha.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdio> // Para popen, pclose, remove
#include <cstdlib> // Para system()
#include <limits> // Para min/max
#include <cmath>  // Para floor, ceil
#include <algorithm>
#include <array>
#include <memory>

// Estrutura para representar limites de dados
struct DataBounds {
    long long min_x = std::numeric_limits<long long>::max();
    long long max_x = std::numeric_limits<long long>::min();
    long long min_y = std::numeric_limits<long long>::max();
    long long max_y = std::numeric_limits<long long>::min();
    
    // Atualiza os limites com um novo ponto
    void update(const Point& p) {
        min_x = std::min(min_x, p.x);
        max_x = std::max(max_x, p.x);
        min_y = std::min(min_y, p.y);
        max_y = std::max(max_y, p.y);
    }
    
    // Verifica se os limites são válidos
    bool isValid() const {
        return min_x <= max_x && min_y <= max_y;
    }
    
    // Aplica uma margem aos limites
    void applyMargin(double marginPercent = 0.1, double minMargin = 5.0) {
        double width = max_x - min_x;
        double height = max_y - min_y;
        
        double margin_x = std::max(width * marginPercent, minMargin);
        double margin_y = std::max(height * marginPercent, minMargin);
        
        min_x -= margin_x;
        max_x += margin_x;
        min_y -= margin_y;
        max_y += margin_y;
    }
    
    // Configura limites padrão caso os atuais sejam inválidos
    void setDefaultIfInvalid() {
        if (!isValid()) {
            min_x = 0;
            max_x = 100;
            min_y = 0;
            max_y = 100;
        }
    }
    
    // Calcula as dimensões da imagem baseadas nos limites
    std::pair<int, int> calculateImageSize(int baseWidth = 800, int minHeight = 400, int maxHeight = 1200) const {
        double width = max_x - min_x;
        double height = max_y - min_y;
        
        // Evitar divisão por zero
        if (width <= 0) width = 1;
        
        int imgWidth = baseWidth;
        int imgHeight = static_cast<int>(imgWidth * (height / width));
        
        // Limitar altura para evitar imagens muito alongadas
        imgHeight = std::max(minHeight, std::min(imgHeight, maxHeight));
        
        return std::make_pair(imgWidth, imgHeight);
    }
};

// Calcula o centroide de um conjunto de pontos
Point calculateCentroid(const std::vector<Point>& points) {
    if (points.empty()) return {0, 0};
    
    long long sum_x = 0, sum_y = 0;
    for (const auto& p : points) {
        sum_x += p.x;
        sum_y += p.y;
    }
    
    return {sum_x / static_cast<long long>(points.size()), 
            sum_y / static_cast<long long>(points.size())};
}

// Classe para gerenciar a geração do script Gnuplot
class GnuplotScript {
private:
    std::ofstream file;
    std::string filename;
    bool first_plot = true;
    
    // Paleta de cores para os polígonos
    const std::array<const char*, 24> colors = {
        "#FF0000", "#0000FF", "#00FF00", "#FF00FF", "#00FFFF", "#FFFF00", 
        "#800000", "#008000", "#000080", "#800080", "#008080", "#808000",
        "#FF8000", "#8000FF", "#00FF80", "#FF0080", "#80FF00", "#0080FF",
        "#FF8080", "#8080FF", "#80FF80", "#FF80FF", "#80FFFF", "#FFFF80"
    };
    
public:
    GnuplotScript(const std::string& fname) : filename(fname) {
        file.open(filename);
    }
    
    ~GnuplotScript() {
        if (file.is_open()) {
            file.close();
        }
        // O script temporário é removido pela função que chama este objeto
    }
    
    bool isOpen() const {
        return file.is_open();
    }
    
    // Configura o terminal e outras configurações básicas
    void setupTerminal(int width, int height, const std::string& outputFile, 
                       const DataBounds& bounds) {
        file << "set terminal pngcairo size " << width << "," << height 
             << " enhanced font 'Verdana,10'" << std::endl;
        file << "set output '" << outputFile << "'" << std::endl;
        file << "set title 'Visualizacao de Poligonos e Pontos'" << std::endl;
        file << "set xlabel 'Eixo X'" << std::endl;
        file << "set ylabel 'Eixo Y'" << std::endl;
        file << "set grid" << std::endl;
        file << "set key off" << std::endl;
        
        // Definir os limites do gráfico
        file << "set xrange [" << bounds.min_x << ":" << bounds.max_x << "]" << std::endl;
        file << "set yrange [" << bounds.min_y << ":" << bounds.max_y << "]" << std::endl;
        
        // Estilo para polígonos e linhas
        file << "set style fill transparent solid 0.3" << std::endl;
        file << "set style line 1 lc rgb 'black' lw 1.5" << std::endl;
    }
    
    // Inicia a seção de plot
    void startPlot() {
        file << "plot";
        first_plot = true;
    }
    
    // Adiciona um item ao plot
    void addPlotItem(const std::string& plotSpec) {
        if (!first_plot) {
            file << ",";
        }
        file << " " << plotSpec;
        first_plot = false;
    }
    
    // Finaliza a seção de plot
    void endPlot() {
        file << std::endl;
    }
    
    // Adiciona um polígono ao plot com a cor especificada
    void addPolygon(int polyId, int colorIndex, int vertexCount) {
        if (vertexCount <= 1) {
            // Ponto único
            addPlotItem("'-' with points pt 7 ps 1.5 lc rgb '" + 
                         std::string(colors[colorIndex % colors.size()]) + 
                         "' title 'Poly " + std::to_string(polyId) + "'");
        } else if (vertexCount == 2) {
            // Linha (polígono aberto com 2 vértices)
            addPlotItem("'-' with lines lw 2 lc rgb '" + 
                         std::string(colors[colorIndex % colors.size()]) + 
                         "' title 'Poly " + std::to_string(polyId) + "'");
        } else {
            // Polígono fechado (3+ vértices)
            addPlotItem("'-' with filledcurves closed lc rgb '" + 
                         std::string(colors[colorIndex % colors.size()]) + 
                         "' title 'Poly " + std::to_string(polyId) + "'");
            addPlotItem("'-' with lines lw 2 lc rgb 'black' notitle");
        }
    }
    
    // Adiciona rótulos ao plot
    void addLabels(const std::string& style) {
        addPlotItem("'-' with labels " + style + " notitle");
    }
    
    // Escreve pontos para um polígono
    void writePolygonPoints(const std::vector<Point>& vertices) {
        for (const auto& p : vertices) {
            file << p.x << " " << p.y << std::endl;
        }
        
        // Fechar o polígono se tiver 3 ou mais vértices
        if (vertices.size() >= 3) {
            file << vertices[0].x << " " << vertices[0].y << std::endl;
        }
        
        file << "e" << std::endl;
    }
    
    // Escreve dados de pontos
    void writePoints(const std::vector<Point>& points) {
        for (const auto& p : points) {
            file << p.x << " " << p.y << std::endl;
        }
        file << "e" << std::endl;
    }
    
    // Escreve rótulos para pontos
    void writePointLabels(const std::vector<Point>& points) {
        for (size_t i = 0; i < points.size(); ++i) {
            file << points[i].x << " " << points[i].y << " \"" << (i+1) << "\"" << std::endl;
        }
        file << "e" << std::endl;
    }
    
    // Escreve um rótulo no centroide
    void writeCentroidLabel(const Point& centroid, const std::string& label) {
        file << centroid.x << " " << centroid.y << " \"" << label << "\"" << std::endl;
        file << "e" << std::endl;
    }
};

// Função principal para desenhar polígonos e pontos
void draw(const std::vector<Polygon>* polygons, const std::vector<Point>* points) {

    
    // Nomes de arquivos
    const std::string script_filename = "temp_gnuplot_script.gp";
    const std::string output_filename = "desenho.png";
    
    // Calcular limites dos dados
    DataBounds bounds;
    
    // Processar polígonos
    if (polygons) {
        for (const auto& poly : *polygons) {
            for (const auto& p : poly.vertices) {
                bounds.update(p);
            }
        }
    }
    
    // Processar pontos
    if (points) {
        for (const auto& p : *points) {
            bounds.update(p);
        }
    }
    
    // Garantir limites válidos
    bounds.setDefaultIfInvalid();
    bounds.applyMargin(0.1, 5.0);
    
    // Calcular dimensões da imagem
    std::pair<int, int> img_size = bounds.calculateImageSize();
    int img_width = img_size.first;
    int img_height = img_size.second;
    
    // Criar e configurar o script Gnuplot
    GnuplotScript script(script_filename);
    if (!script.isOpen()) {
        std::cerr << "Erro: Não foi possível criar o arquivo de script gnuplot." << std::endl;
        return;
    }
    
    script.setupTerminal(img_width, img_height, output_filename, bounds);
    script.startPlot();
    
    // Adicionar polígonos ao plot
    if (polygons && !polygons->empty()) {
        int idx = 0;
        for (const auto& poly : *polygons) {
            int vertexCount = poly.vertices.size();
            if (vertexCount > 0) { // Desenhar mesmo polígonos "abertos" com poucos vértices
                script.addPolygon(poly.id, idx, vertexCount);
                idx++;
            }
        }
    }
    
    // Adicionar pontos ao plot
    if (points && !points->empty()) {
        script.addPlotItem("'-' with points pt 7 ps 1 lc rgb 'black' title 'Pontos'");
    }
    
    // Adicionar rótulos para polígonos
    if (polygons && !polygons->empty()) {
        for (const auto& poly : *polygons) {
            if (!poly.vertices.empty()) {
                script.addLabels("font ',8' offset 0,0 point pt 7 ps 0");
            }
        }
    }
    
    // Adicionar rótulos para pontos
    if (points && !points->empty()) {
        script.addLabels("font ',8' offset 1,1");
    }
    
    script.endPlot();
    
    // Escrever dados dos polígonos
    if (polygons && !polygons->empty()) {
        for (const auto& poly : *polygons) {
            int vertexCount = poly.vertices.size();
            if (vertexCount > 0) {
                script.writePolygonPoints(poly.vertices);
                
                // Adicionar borda preta para polígonos com 3+ vértices
                if (vertexCount >= 3) {
                    script.writePolygonPoints(poly.vertices);
                }
            }
        }
    }
    
    // Escrever dados dos pontos
    if (points && !points->empty()) {
        script.writePoints(*points);
    }
    
    // Escrever rótulos para polígonos
    if (polygons && !polygons->empty()) {
        int poly_idx = 1;
        for (const auto& poly : *polygons) {
            if (!poly.vertices.empty()) {
                // Calcular centroide ou usar o primeiro ponto se polígono "aberto"
                Point labelPos = (poly.vertices.size() >= 3) ? 
                                 calculateCentroid(poly.vertices) : 
                                 poly.vertices[0];
                
                script.writeCentroidLabel(labelPos, "P" + std::to_string(poly_idx));
                poly_idx++;
            }
        }
    }
    
    // Escrever rótulos para pontos
    if (points && !points->empty()) {
        script.writePointLabels(*points);
    }
    
    // Executar o gnuplot
    std::string command = "gnuplot " + script_filename;
    int result = system(command.c_str());
    
    if (result != 0) {
        std::cerr << "Erro: Falha ao executar gnuplot. Verifique se ele está instalado e no PATH." << std::endl;
    } else {
        std::cout << "Imagem gerada com sucesso: " << output_filename << std::endl;
    }
    
    // Remover o arquivo de script temporário
    std::remove(script_filename.c_str());
} 