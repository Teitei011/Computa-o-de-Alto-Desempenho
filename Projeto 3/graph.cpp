#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include <list>

namespace chrono = std::chrono;

class Graph
  std::vector<int> *adj;
  int max_number{1};
  {
public:
    Graph();
    void addEdge(int origem, int destino);
    void show_graph():
};

Graph:Graph(){
  std::vector<int> *adj = new std::vector<int>[5];
}

void Graph::addEdge(int origem, int destino)
{
    adj[origem].push_back(destino);

    if(destino ){
      adj[destino].push_back(origem);
    }
}

void Graph::show_graph(){
}

int read_data(){

}

int main()
{
    Graph g();
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 2);
    g.addEdge(2, 3);

    g.show_graph();

    return 0;
}
