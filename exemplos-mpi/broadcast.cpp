/*
 * Criar um arquivo entrada.dat com um numero inteiro
 * antes de rodar este programa
 */

#include <fstream>
#include <iostream>
#include <mpi.h>

int main(int argc, char *argv[]) {
  int rank, dado = -1;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    // Rodando no processo 0: le dado do arquivo.
    std::ifstream entrada("entrada.dat");
    if (!entrada.fail()) {
      entrada >> dado;
    } else {
      // Sem arquivo, usa default.
      dado = 1;
    }
  }

  std::cout << "Processo " << rank << ": meu dado por enquanto e " << dado
            << std::endl;

  MPI_Bcast(&dado, 1, MPI_INT, 0, MPI_COMM_WORLD);

  std::cout << "Processo " << rank << ": calculando com dado " << dado
            << std::endl;

  /* Faz algum cálculo usando o dado */

  MPI_Finalize();
  return 0;
}
