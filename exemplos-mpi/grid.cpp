#include <iostream>
#include <mpi.h>
#include <vector>

int main(int argc, char *argv[]) {
  int quantos, rank;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &quantos);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  std::vector<int> dims(2), periodico(2);
  int reordena;
  MPI_Comm torus_comm;

  // Topologia toro 2D
  dims[0] = dims[1] = 0;                 // Deixa MPI_Dims_create escolher
  MPI_Dims_create(quantos, 2, &dims[0]); // Melhores dimensoes
  periodico[0] = 1;
  periodico[1] = 1; // Periodico
  reordena = 1;     // Pode reordenar

  // Cria uma topologia linear com periodicidade
  MPI_Cart_create(MPI_COMM_WORLD, 2, &dims[0], &periodico[0], reordena,
                  &torus_comm);

  // Informacoes
  std::vector<int> coords(2);
  MPI_Cart_coords(torus_comm, rank, 2, &coords[0]);
  std::cout << "Processo " << rank << " tem coordenadas (" << coords[0] << ", "
            << coords[1] << ")\n";

  int da_esquerda = -1, da_direita = -1, de_cima = -1, de_baixo = -1, meu;

  meu = rank * rank;

  MPI_Status status;
  int fonte, destino;

  // Comunicacao para baixo
  MPI_Cart_shift(torus_comm, 0, 1, &fonte, &destino);
  MPI_Sendrecv(&meu, 1, MPI_INT, destino, 1, &de_cima, 1, MPI_INT, fonte, 1,
               torus_comm, &status);

  // Comunicacao para cima
  MPI_Cart_shift(torus_comm, 0, -1, &fonte, &destino);
  MPI_Sendrecv(&meu, 1, MPI_INT, destino, 2, &de_baixo, 1, MPI_INT, fonte, 2,
               torus_comm, &status);

  // Comunicacao para a direita
  MPI_Cart_shift(torus_comm, 1, 1, &fonte, &destino);
  MPI_Sendrecv(&meu, 1, MPI_INT, destino, 1, &da_esquerda, 1, MPI_INT, fonte, 1,
               torus_comm, &status);

  // Comunicacao para a esquerda
  MPI_Cart_shift(torus_comm, 1, -1, &fonte, &destino);
  MPI_Sendrecv(&meu, 1, MPI_INT, destino, 2, &da_direita, 1, MPI_INT, fonte, 2,
               torus_comm, &status);

  std::cout << "Rank: " << rank << ", de cima: " << de_cima
            << ", da esquerda: " << da_esquerda;
  std::cout << ", meu: " << meu << ", de baixo: " << de_baixo
            << ", da direita: " << da_direita << std::endl;

  MPI_Finalize();
  return 0;
}
