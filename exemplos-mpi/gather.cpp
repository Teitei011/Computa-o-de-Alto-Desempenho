#include <cmath>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <vector>

int main(int argc, char *argv[]) {
  int quantos, rank;

  std::vector<float> dados;
  std::vector<float> originais(2);
  std::vector<int> v;

  for (size_t i = 0; i < 5; i++) {
    v.push_back(i);
  }

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &quantos);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    // Processo 0 vai receber 2 dados de cada processo.
    dados.resize(2 * quantos);
  }

  // Cálculos em todos os processos
  originais[0] = pow(2, 2 * rank);
  originais[1] = pow(2, 2 * rank + 1);

  std::cout << "Testando a forma como a memoria é distribuida... " << '\n';
  for (size_t i = 0; i < v.size(); i++) {
    std::cout << v[i] << ' ';
  }
  std::cout << '\n';

  MPI_Gather(&originais[0], 2, MPI_FLOAT, &dados[0], 2, MPI_FLOAT, 0,
             MPI_COMM_WORLD);

  if (rank == 0) { // Rodando no processo 0
    std::ofstream saida("resultados.dat");
    for (int i = 0; i < 2 * quantos; ++i)
      saida << " " << dados[i];
    saida << std::endl;
  }

  MPI_Finalize();
  return 0;
}
