#include <iostream>
#include <mpi.h>

int main(int argc, char *argv[]) {
  int quantos, rank;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &quantos);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (quantos %2 == 0){ // Condicao par

    if (rank < quantos / 2){
      std::cout << "O processo " << rank << " esta entre os primeiros." << '\n';
    }else{
      std::cout << "O processo " << rank << " esta entre os ultimos." << '\n';
    }

  } else{ // Condicao impar
    if(rank < (quantos - 1) / 2){
      std::cout << "O processo " << rank << " esta entre os primeiros." << '\n';
    } else if(rank == quantos - 1){
      std::cout << "O processo " << rank << " e o lanterninha." << '\n';
    }
    else{
      std::cout << "O processo " << rank << " esta entre os ultimos." << '\n';
    }
  }

  // std::cout << "Processo " << rank << " de " << quantos << " rodando." << std::endl;

  MPI_Finalize();
  return 0;
}
