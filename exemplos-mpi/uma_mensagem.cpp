#include <iostream>
#include <mpi.h>
#include <string>

int main(int argc, char *argv[]) {
  int const MAX = 256; // Tamanho maximo da mensagem
  int const TAG = 1;   // Tag a utilizar nas comunicacoes

  int quantos, rank;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &quantos);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (quantos != 2) {
    // Precisa rodar com 2 processos.
    if (rank == 0) {
      std::cerr << "Este programa funciona apenas com 2 processos!\n";
    }
  } else {
    // Rodando com 2 processos
    if (rank == 0) {
      // Rodando no processo 0
      MPI_Status status;
      char mensagem[MAX];

      MPI_Recv(mensagem, MAX, MPI_CHAR, 1, TAG, MPI_COMM_WORLD, &status);

      std::cout << "Recebi a mensagem: " << mensagem << std::endl;
    } else {
      // Rodando no processo 1
      std::string mensagem{"Um ola de seu amigo, processo "};
      mensagem += std::to_string(rank);

      MPI_Send(mensagem.c_str(), mensagem.length() + 1, MPI_CHAR, 0, TAG,
               MPI_COMM_WORLD);
    }
  }

  MPI_Finalize();
  return 0;
}
