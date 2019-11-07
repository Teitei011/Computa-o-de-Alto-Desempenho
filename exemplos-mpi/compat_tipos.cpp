#include <iomanip>
#include <iostream>
#include <mpi.h>
#include <vector>

class MatrizQuadrada {
  std::vector<double> _armazem;
  size_t _n;

public:
  MatrizQuadrada(int n) : _armazem(n * n), _n(n) {}

  size_t size() const { return _n; }

  double &operator()(size_t i, size_t j) { return _armazem[i * _n + j]; }

  double const &operator()(size_t i, size_t j) const {
    return _armazem[i * _n + j];
  }
};

std::ostream &operator<<(std::ostream &os, MatrizQuadrada const &m) {
  auto N = m.size();
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < N; ++j) {
      os << std::setw(4) << m(i, j);
    }
    os << std::endl;
  }
  return os;
}

int main(int argc, char **argv) {
  int const N = 10;
  int const TAG = 7;

  int quantos;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &quantos);

  if (quantos != 1) {
    // Roda somente com 1 processo!
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
      std::cerr << "Rodar com um processo apenas.\n";
    }

    MPI_Finalize();
    return 1;
  }

  MatrizQuadrada matorig(N), matnova(N);

  // Inicializa matriz original
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      matorig(i, j) = i - j;
    }
  }

  MPI_Datatype tLinha, tColuna;

  // Cria um tipo para as linhas
  MPI_Type_contiguous(N, MPI_DOUBLE, &tLinha);
  MPI_Type_commit(&tLinha);

  // Cria um tipo para as colunas
  MPI_Type_vector(N, 1, N, MPI_DOUBLE, &tColuna);
  MPI_Type_commit(&tColuna);

  MPI_Status status;

  // Transmite linha por linha,
  // recebe coluna por coluna
  for (int j = 0; j < N; ++j) {
    MPI_Sendrecv(&matorig(j, 0), 1, tLinha, 0, TAG, &matnova(0, j), 1, tColuna,
                 0, TAG, MPI_COMM_WORLD, &status);
  }

  std::cout << "Matriz original:\n";
  std::cout << matorig;

  std::cout << "Matriz transposta:\n";
  std::cout << matnova;

  MPI_Finalize();
  return 0;
}
