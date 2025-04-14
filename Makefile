CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

TARGET = poligonos
SOURCES = main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = geometry.h

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS) temp_gnuplot_script.gp desenho.png

.PHONY: all clean
