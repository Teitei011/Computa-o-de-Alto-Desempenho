#include <algorithm>
#include <cmath>
#include <iostream>
#include <mpi.h>
#include <vector>

int main(int argc, char *argv[]) {
  int quantos, rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &quantos);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  std::vector<int> incluidos;
  MPI_Group todos, subgrupo;
  MPI_Comm subcom;

  MPI_Comm_group(MPI_COMM_WORLD, &todos);

  for (int i = rank % 2; i < quantos; i += 2)
    incluidos.push_back(i);

  MPI_Group_incl(todos, incluidos.size(), &incluidos[0], &subgrupo);
  MPI_Comm_create(MPI_COMM_WORLD, subgrupo, &subcom);

  int meu, total;

  meu = pow(2, rank);

  MPI_Reduce(&meu, &total, 1, MPI_INT, MPI_SUM, 0, subcom);

  if (rank == 0) {
    std::cout << "Soma das potencias pares:   " << total << std::endl;
  } else if (rank == 1) {
    std::cout << "Soma das potencias impares: " << total << std::endl;
  }

  MPI_Finalize();
  return 0;
}
