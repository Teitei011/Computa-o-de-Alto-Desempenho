#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <list>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

class Graph
{
    std::vector<int> *adjLists;

public:
    Graph(int V);
    void addEdge(int src, int dest);
    std::vector<int> find_triangles();
};


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

 std::pair <int , std::vector<int>> read_numbers(std::string filename) {
  std::vector<int> numbers;
  int the_higher_number{0};

  std::ifstream infile;
  infile.open(filename);

  if (infile.is_open()) {
    int num;
    while (infile >> num) {


      if (the_higher_number > num)
        the_higher_number = num;

      numbers.push_back(num);
    }
  }
  return the_higher_number, numbers;
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

  // Colocando os dados nele
  int buffer{-1};
  for (std::vector<int>::iterator it = numbers.begin() ; it != numbers.end(); ++it){
    if (buffer == -1){
      buffer = *it;
    }else{
      g.addEdge(buffer, *it);
      g.addEdge(*it, buffer);
    }
 }
 std::cout << '\n';

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
