#include <iostream>
#include <vector>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int quantos, rank;
    int da_esquerda = -1, da_direita = -1, meu;
    int esquerda, direita;
    
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &quantos);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Calcula rank do processo da esquerda.
    esquerda = (rank != 0) ? rank - 1 : MPI_PROC_NULL;

    // Calcula rank do processo da direita.
    direita = (rank != quantos - 1) ? rank + 1 : MPI_PROC_NULL;

    // Calcula o valor local. Neste exemplo apenas o quadrado do rank.
    meu = rank * rank;

    std::vector<MPI_Request> req(4);   // Identificadores dos pedidos de comunicacao
    std::vector<MPI_Status> status(4); // Status das comunicacoes (depois de concluidas)

    // Dispara envios e recepcoes de ambos os lados simultaneamente.
    MPI_Isend(&meu,         1, MPI_INT, direita,  1, MPI_COMM_WORLD, &req[0]);
    MPI_Isend(&meu,         1, MPI_INT, esquerda, 2, MPI_COMM_WORLD, &req[1]);
    MPI_Irecv(&da_esquerda, 1, MPI_INT, esquerda, 1, MPI_COMM_WORLD, &req[2]);
    MPI_Irecv(&da_direita,  1, MPI_INT, direita,  2, MPI_COMM_WORLD, &req[3]);

    // Espera ate que todos os envios e recepcoes terminem.
    MPI_Waitall(4, &req[0], &status[0]);
  
    std::cout << "Rank: " << rank << ", da esquerda: " << da_esquerda
              <<", meu: " << meu << ", da direita " << da_direita << std::endl;

    MPI_Finalize();
    return 0;
}
