#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <list>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include <typeinfo>

namespace chrono = std::chrono;

class Graph {
  int _size;
  int **adjLists;

public:
  Graph(int V);
  void addEdge(int origem, int destino);
  std::vector<int> find_triangles();
  void show_graph();
};

void Graph::show_graph() {
  for (int i = 0; i < this->_size; i++) {
    for (int j = 0; j < this->_size; j++) {
      std::cout << this->adjLists[i][j] << ' ';
    }
    std::cout << '\n';
  }
}

Graph::Graph(int N) {
  _size = N;

  // Creating the matrix
  int **adjLists = new int *[N];
  int *SA = new int[N * N];

  // linking pointers
  for (int i = 0; i < N; i++) {
    adjLists[i] = &SA[i * N];
  }

  // Putting zero in all the matrix
  for (int i = 0; i < N; i++) {
    for (int k = 0; k < N; k++) {
      for (int j = 0; j < N; j++) {
        adjLists[i][j] = 0;
      }
    }
  }
}

void Graph::addEdge(int origem, int destino) {
  this->adjLists[origem][destino] = 1;
  this->adjLists[destino][origem] = 1;
}

// std::vector<int> Graph::find_triangles() {
//
// }

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

std::pair<int, std::vector<int>> read_numbers(std::string filename) {
  std::pair<int, std::vector<int>> data;

  std::vector<int> numbers;
  int the_highest_number{0};

  std::ifstream infile;
  infile.open(filename);

  if (infile.is_open()) {
    int num;
    while (infile >> num) {

      if (the_highest_number < num)
        the_highest_number = num;

      numbers.push_back(num);
    }
  }
  data.first = the_highest_number;
  data.second = numbers;

  infile.close();
  return data;
}

int main(int argc, char *argv[]) {

  std::string filename = read_argument(argc, argv);

  std::vector<int> numbers;
  int the_highest_number;
  std::pair<int, std::vector<int>> data;

  data = read_numbers(filename);
  the_highest_number = std::get<0>(data);
  numbers = std::get<1>(data);

  // The time monitor
  double elapsed = 0;
  chrono::high_resolution_clock::time_point t1, t2;

  t1 = chrono::high_resolution_clock::now();

  Graph g(the_highest_number); // Inicializando o grapho

  // Colocando os dados nele
  int buffer{-1};
  int contador{0};
  int inserir;

  for (unsigned i = 0; i < numbers.size(); i++) {
    inserir = numbers[i];
    if (contador == 1) {
      g.addEdge(buffer, inserir);
      contador = 0;
    } else {
      contador += 1;
    }
    buffer = inserir;
  }

  t2 = chrono::high_resolution_clock::now();

  auto dt = chrono::duration_cast<chrono::microseconds>(t2 - t1);
  elapsed += dt.count();

  // Creating a file with the .trg instead of the .edgelist
  std::string toReplace(".edgelist");
  size_t pos = filename.find(toReplace);
  filename.replace(pos, toReplace.length(), ".trg");

  std::ofstream output_file;
  output_file.open(filename);

  // Passando por todos os pontos dos vetor e escrevendo no output text

  // TODO: Lembrar de mudar isso para o vetor do find_triangles
  for (std::vector<int>::iterator it = numbers.begin(); it != numbers.end();
       ++it) {
    output_file << *it << " ";
  }

  output_file.close();

  // Show timing results
  std::cout << "Time Taken: " << elapsed / 1.0 / 1e6 << std::endl;

  g.show_graph();

  return 0;
}