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

class Graph {
  std::vector<std::vector<int>> _adjLists;

public:
  Graph(int V);
  void addEdge(int origem, int destino);
  std::vector<int> find_triangles();
  void show_graph();
};

void Graph::show_graph() {

  for (unsigned i = 0; i < _adjLists.size(); i++) {
    // std::cout << i << " --> ";
    for (unsigned j = 0; j < _adjLists[i].size(); j++) {
      // std::cout << _adjLists[i][j] << ' ';
    }
    std::cout << '\n';
  }

  return;
}

Graph::Graph(int V) {
  _adjLists.resize(V + 1);
  // std::cout << "Tamanho do vetor igual a: " << _adjLists.size() << '\n';
}

void Graph::addEdge(int origem, int destino) {
  _adjLists[origem].push_back(destino);
  _adjLists[destino].push_back(origem);
}

std::vector<int> Graph::find_triangles() { // TODO: The problem lays here
  std::vector<int> answer;
  int buffer, triangles;
  int contador{0};

  for (unsigned i = 0; i < _adjLists.size() - 1; i++) {
    triangles = 0;
    if (_adjLists[i].size() < 2){ // Não há n de vertices o suficiente para fazer um triangulo
      answer.push_back(0);
      // std::cout << "Muito pequeno, próximo!" << '\n';
      }
    else {
      // std::cout << "Tem um tamanho o suficiente" << '\n';
      for (unsigned j = 0; j < _adjLists[i].size(); j++) {
        if (contador == 0) {
          buffer = _adjLists[i][j];
          contador += 1;
        } else {
          for (unsigned k = 0; k < _adjLists[buffer].size(); k++) {
            if (_adjLists[buffer][k] == buffer) {
              triangles += 1;
              break;
            }
          }
          contador = 0;
        }
      }
      answer.push_back(triangles);
    }
  }

  return answer;
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

  int buffer{-1};
  int contador{0};
  int contador1{0};
  int inserir;

  double elapsed = 0;
  chrono::high_resolution_clock::time_point t1, t2;

  std::vector<int> numbers;
  int the_highest_number;
  std::pair<int, std::vector<int>> data;

  std::vector<int> answer;

  std::cout << "The filename is: " << filename << '\n';

  data = read_numbers(filename);
  the_highest_number = std::get<0>(data);
  numbers = std::get<1>(data);

  // The time monitor

  t1 = chrono::high_resolution_clock::now();

  // std::cout << "The highest number is: " << the_highest_number << '\n';

  Graph g(the_highest_number); // Inicializando o grapho
  // Colocando os dados nele

  for (unsigned i = 0; i < numbers.size(); i++) {
    inserir = numbers[i];
    if (contador == 1) {
      g.addEdge(buffer, inserir);
      contador = 0;
      // std::cout << buffer << " " << inserir << '\n';
    } else {
      contador += 1;
      contador1 += 1;
    }
    buffer = inserir;
  }

  answer = g.find_triangles();

  std::cout << "Resposta do exercicio: " << '\n';
  for (size_t i = 0; i < answer.size(); i++) {
    std::cout << answer[i] << " ";
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
  for (std::vector<int>::iterator it = answer.begin(); it != answer.end();
       ++it) {
    output_file << *it << " ";
  }

  output_file.close();

  g.show_graph();

  // Show timing results
  std::cout << "Time Taken: " << elapsed / 1.0 / 1e6 << std::endl;

  return 0;
}
