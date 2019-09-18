#include <algorithm>
#include <chrono>
#include <iostream>
#include <list>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace chrono = std::chrono;
std::string read_argument(int argc, char *argv[]);

class Graph {
  std::vector<int> *adj;
  int max_number{1};

public:
  Graph();
  void addEdge(int origem, int destino);
  void show_graph();
};

Graph::Graph() { std::vector<int> *adj = new std::vector<int>[5]; }

void Graph::addEdge(int origem, int destino) {
  adj[origem].push_back(destino);

  if (adj.back() >
      destino) { // Caso o valor de acesso ultrapasse o que tem no vector
    // Verifando agora qual é o maior numero que contém na lista
  } else {
    adj[destino].push_back(origem);
  }
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

int main(int argc, char *argv[]) {

  std::string filename = read_argument(argc, argv);
  std::cout << "The filename is: " << filename << '\n';

  std::vector<int> numbers;
  int higher_number{0};

  fstream input(
      filename); // put your program together with thsi file in the same folder.
  if (input.is_open()) {
    while (!input.eof()) {
      // if ()
      getline(input, number);      // read number
      data = atoi(number.c_str()); // convert to integer
      cout << data << endl;        // print it out
    }
  }

  // Graph g();
  // g.addEdge(0, 1);
  // g.addEdge(0, 2);
  // g.addEdge(1, 2);
  // g.addEdge(2, 3);
  //
  // g.show_graph();

  return 0;
}
