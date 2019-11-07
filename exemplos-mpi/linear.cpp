#include <iostream>
#include <mpi.h>

int main(int argc, char *argv[]) {
  int quantos, rank;
  int da_esquerda = -1, da_direita = -1, meu;
  int esquerda, direita;

  MPI_Status status;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &quantos);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Calcula rank do processo da esquerda.
  esquerda = (rank != 0) ? rank - 1 : MPI_PROC_NULL;

  // Calcula rank do processo da direita.
  direita = (rank != quantos - 1) ? rank + 1 : MPI_PROC_NULL;

  // Calcula o valor local. Neste exemplo apenas o quadrado do rank.
  meu = rank * rank;

  MPI_Sendrecv(&meu, 1, MPI_INT, direita, 1, &da_esquerda, 1, MPI_INT, esquerda,
               1, MPI_COMM_WORLD, &status);
  MPI_Sendrecv(&meu, 1, MPI_INT, esquerda, 2, &da_direita, 1, MPI_INT, direita,
               2, MPI_COMM_WORLD, &status);

  std::cout << "Rank: " << rank << ", da esquerda: " << da_esquerda
            << ", meu: " << meu << ", da direita " << da_direita << std::endl;

  MPI_Finalize();
  return 0;
}
