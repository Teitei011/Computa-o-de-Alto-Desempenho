#include <iostream>
#include <mpi.h>
#include <string>

int main(int argc, char *argv[]) {
  int const MAX = 256; // Tamanho maximo da mensagem

  int quantos, rank;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &quantos);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0) {
    // Rodando no processo 0
    int tamanho;

    for (int i = 1; i < quantos; i++) {
      char mensagem[MAX];
      MPI_Status status;

      MPI_Recv(mensagem, MAX, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG,
              MPI_COMM_WORLD, &status);
      //MPI_Recv(mensagem, MAX, MPI_CHAR, i, i + 100,
      //         MPI_COMM_WORLD, &status);

      MPI_Get_count(&status, MPI_CHAR, &tamanho);

      std::cout << "Mensagem recebida de " << status.MPI_SOURCE << ", com tag "
                << status.MPI_TAG << ", tamanho " << tamanho << ": ";
      std::cout << mensagem << std::endl;
    }
  } else {
    // Rodando em processo diferente de 0
    std::string mensagem = "Um ola de seu ";
    mensagem += (rank % 2 == 0) ? "grande" : "bom";
    mensagem += " amigo, processo ";
    mensagem += std::to_string(rank);

    // O tag nao precisa variar com o rank.
    // Esta assim apenas para demonstrar MPI_ANY_TAG.
    MPI_Send(mensagem.c_str(), mensagem.length() + 1, MPI_CHAR, 0, rank + 100,
             MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}
