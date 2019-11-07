#include <iostream>
#include <mpi.h>
#include <vector>

int main(int argc, char *argv[]) {
  int quantos, rank;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &quantos);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  std::vector<int> dims(1), periodico(1);
  int reordena;

  MPI_Comm linear_comm;

  dims[0] = quantos; // Todos os processos em uma dimensao
  periodico[0] = 0;  // Nao eh periodico
  reordena = 1;      // Pode reordenar

  // Cria uma topologia linear sem periodicidade
  MPI_Cart_create(MPI_COMM_WORLD, 1, &dims[0], &periodico[0], reordena,
                  &linear_comm);

  int da_esquerda = -1, da_direita = -1, meu;
  int fonte, destino;

  meu = rank * rank;

  MPI_Status status;

  // Comunicacao no sentido positivo (direita)
  MPI_Cart_shift(linear_comm, 0, 1, &fonte, &destino);
  MPI_Sendrecv(&meu, 1, MPI_INT, destino, 1, &da_esquerda, 1, MPI_INT, fonte, 1,
               linear_comm, &status);

  // Comunicacao no sentido negativo (esquerda)
  MPI_Cart_shift(linear_comm, 0, -1, &fonte, &destino);
  MPI_Sendrecv(&meu, 1, MPI_INT, destino, 2, &da_direita, 1, MPI_INT, fonte, 2,
               linear_comm, &status);

  std::cout << "Rank: " << rank << ", da esquerda: " << da_esquerda
            << ", meu: " << meu << ", da direita " << da_direita << std::endl;

  MPI_Finalize();
  return 0;
}
