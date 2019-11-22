#include <iostream>
#include <mpi.h>

int main(int argc, char *argv[]) {
  int quantos, rank;

  MPI_Init(&argc, &argv);


  MPI_Comm_size(MPI_COMM_WORLD, &quantos);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  std::cout << "Processo " << rank << " de " << quantos << " rodando."
            << std::endl;

  MPI_Finalize();
  return 0;
}
