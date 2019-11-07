#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <mpi.h>
#include <numeric>
#include <string>
#include <vector>

using Vector = std::vector<double>;

class Matrix {
  std::vector<double> _storage;
  size_t _nrows, _ncols;

public:
  Matrix(size_t nrows = 0, size_t ncols = 0)
      : _storage(nrows * ncols), _nrows(nrows), _ncols(ncols) {}

  size_t nrows() const { return _nrows; }

  size_t ncols() const { return _ncols; }

  double &operator()(size_t i, size_t j) { return _storage[i * _ncols + j]; }

  double const &operator()(size_t i, size_t j) const {
    return _storage[i * _ncols + j];
  }
};

// Some prototypes.
Vector compute_vector(size_t N);
Matrix compute_matrix(size_t N);

Vector matrix_vector_product(Matrix const &m, Vector const &v);

std::tuple<std::vector<int>, std::vector<int>>
compute_scatter_param(size_t N, size_t nprocs);

//
// Parallel computation of matrix-vector product.
//
int main(int argc, char *argv[]) {
  int rank, nprocs;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // See if we have the needed command-line parameter.
  if (argc != 2) {
    if (rank == 0) {
      std::cerr << "Give the size of the matrix and vector\n";
    }
    MPI_Finalize();
    return 1;
  }

  // Read the number of elements and verify.
  int N = std::stoi(argv[1]);
  if (N <= 0) {
    if (rank == 0) {
      std::cerr << "Wrong matrix size.\n";
    }
    MPI_Finalize();
    return 1;
  }

  // Generate initial matrix and vetor.
  Vector v;
  Matrix m;

  if (rank == 0) {
    // Rank 0 computes.
    v = compute_vector(N);
    m = compute_matrix(N);
  } else {
    // All other ranks allocate space for copy of vector.
    v.resize(N);
  }

  // Compute parameters for distributing the matrix.
  auto [counts, desls] = compute_scatter_param(N, nprocs);

  // Create the local matrix piece.
  Matrix my_m(counts[rank], N);

  MPI_Datatype line;
  MPI_Type_contiguous(N, MPI_DOUBLE, &line);
  MPI_Type_commit(&line);

  // Distribute matrix values.
  MPI_Scatterv(&m(0, 0), &counts[0], &desls[0], line, &my_m(0, 0), counts[rank],
               line, 0, MPI_COMM_WORLD);

  // Broadcast vector values.
  MPI_Bcast(&v[0], N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  // Compute matrix-vetor product (for the local piece of the matrix)
  Vector my_vres = matrix_vector_product(my_m, v);

  // Gather all results in rank 0
  Vector vres;

  if (rank == 0) {
    vres.resize(N);
  }

  MPI_Gatherv(&my_vres[0], counts[rank], MPI_DOUBLE, &vres[0], &counts[0],
              &desls[0], MPI_DOUBLE, 0, MPI_COMM_WORLD);

  // Rank 0 output the result.
  if (rank == 0) {
    // Show sum of vector elements
    double s = std::accumulate(std::begin(vres), std::end(vres), 0.0);

    std::cout << std::setprecision(15);
    std::cout << "The sum of the resulting vector is " << s << std::endl;
  }

  MPI_Finalize();
  return 0;
}

// Initial vector computation.
Vector compute_vector(size_t N) {
  // Here just return a vetor 1..N
  Vector v(N);
  std::iota(begin(v), end(v), 1.0);

  return v;
}

// Initial matrix computation
Matrix compute_matrix(size_t N) {
  // Here just return an identity matrix
  Matrix m(N, N);

  // Generate an identity matrix.
  for (size_t i = 0; i < N; ++i) {
    m(i, i) = 1.0;
  }
  // Other values are already zero.

  return m;
}

Vector matrix_vector_product(Matrix const &m, Vector const &v) {
  Vector res(m.nrows());

  for (size_t i = 0; i < m.nrows(); ++i) {
    double s = 0.0;
    for (size_t j = 0; j < m.ncols(); ++j) {
      s += m(i, j) * v[j];
    }
    res[i] = s;
  }

  return res;
}

std::tuple<std::vector<int>, std::vector<int>>
compute_scatter_param(size_t N, size_t nprocs) {
  std::vector<int> counts(nprocs), desls(nprocs);

  int q = N / nprocs, r = N % nprocs;

  std::fill_n(begin(counts), r, q + 1);
  std::fill(begin(counts) + r, end(counts), q);

  desls[0] = 0;
  std::partial_sum(begin(counts), end(counts) - 1, begin(desls) + 1);

  return {counts, desls};
}
