#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <list>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace chrono = std::chrono;
std::string read_argument(int argc, char *argv[]);

class Graph {
  std::list<int> adj;

public:
  Graph();
  void show_graph();
  void find_triangle();
  void addEdge(int origem, int destino);
};

void Graph::Graph() {}

void Graph::show_graph() {
  std::cout << "Graph contains: ";
  for (auto it = std::begin(this.adj); it != std::end(this.adj); ++it)
    std::cout << ' ' << *it;
  std::cout << '\n';
}

void Graph::find_triangle() {}

void Graph::addEdge(int origem, int destino) {
  adj.insert(origem, destino);
  adj.insert(destino, origem);
}

std::string read_argument(int argc, char *argv[]) {
  std::string filename;

  // We need exactly three arguments (considering program name).
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <filename>\n";
    exit(1);
  }

  filename = argv[1];
  return filename;
}

std::vector<int> read_numbers(std::string filename) {
  std::vector<int> numbers;
  std::ifstream infile;
  infile.open(filename);

  if (infile.is_open()) {
    int num;
    while (infile >> num) {
      numbers.push_back(num);
    }
  }
  return numbers;
}

int main(int argc, char *argv[]) {

  std::string filename = read_argument(argc, argv);
  std::cout << "The filename is: " << filename << '\n';

  std::vector<int> numbers;
  numbers = read_numbers(filename);

  // The time monitor
  double elapsed = 0;
  chrono::high_resolution_clock::time_point t1, t2;

  t1 = chrono::high_resolution_clock::now();

  Graph g(); // Inicializando o grapho

  // g.addEdge(0, 1);
  // g.addEdge(0, 2);
  // g.addEdge(1, 2);
  // g.addEdge(2, 3);
  // //

  t2 = chrono::high_resolution_clock::now();

  auto dt = chrono::duration_cast<chrono::microseconds>(t2 - t1);
  elapsed += dt.count();

  // Show timing results
  std::cout << "Time Taken: " << elapsed / 1.0 / 1e6 << std::endl;

  // g.show_graph();

  return 0;
}
