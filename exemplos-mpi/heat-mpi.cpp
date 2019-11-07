#include <algorithm>
#include <armadillo>
#include <array>
#include <cstdlib>
#include <iostream>
#include <mpi.h>
#include <string>
#include <tuple>

using namespace arma;

std::tuple<int, double, std::string> read_args(int argc, char *argv[],
                                               int rank);
std::tuple<int, int> compute_local_sizes(int N, std::array<int, 2> dims);
std::tuple<int, int> find_f_overlap(int f_first, int f_last, int grid_pos,
                                    int Nblock);

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  double const outside_temp = 0.;
  double box_f_value = -100.;

  auto [N, epsilon, filename] = read_args(argc, argv, rank);

  int nprocs;
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  std::array<int, 2> dims, periodic;
  int reordena = 0;

  // Topologia malha 2D
  dims[0] = dims[1] = 0;
  MPI_Dims_create(nprocs, 2, &dims[0]);
  periodic[0] = periodic[1] = 0;

  MPI_Comm grid_comm;
  MPI_Cart_create(MPI_COMM_WORLD, 2, &dims[0], &periodic[0], reordena,
                  &grid_comm);

  auto [Nr, Nc] = compute_local_sizes(N, dims);

  auto TN = Nr * dims[0];

  mat u(Nr + 2, Nc + 2, fill::zeros);
  mat f(Nr, Nc, fill::zeros);

  std::array<int, 2> coords;
  MPI_Cart_coords(grid_comm, rank, 2, &coords[0]);

  if (coords[0] == 0) {
    u.row(0) = outside_temp * ones(1, Nc + 2);
  }
  if (coords[0] == dims[0] - 1) {
    u.row(Nr + 1) = outside_temp * ones(1, Nc + 2);
  }
  if (coords[1] == 0) {
    u.col(0) = outside_temp * ones(Nr + 2, 1);
  }
  if (coords[1] == dims[1] - 1) {
    u.col(Nc + 1) = outside_temp * ones(Nr + 2, 1);
  }

  auto ini_f = 2 * TN / 5;
  auto fin_f = 3 * TN / 5;

  auto [size_row, st_row] = find_f_overlap(ini_f, fin_f, coords[0], Nr);
  auto [size_col, st_col] = find_f_overlap(ini_f, fin_f, coords[1], Nc);

  if (size_row > 0 && size_col > 0) {
    auto rows = span(st_row, st_row + size_row - 1);
    auto cols = span(st_col, st_col + size_col - 1);
    f(rows, cols) = box_f_value * ones(size_row, size_col);
  }

  MPI_Datatype mpi_column, mpi_row;

  MPI_Type_contiguous(Nr, MPI_DOUBLE, &mpi_column);
  MPI_Type_commit(&mpi_column);

  MPI_Type_vector(Nc, 1, Nr + 2, MPI_DOUBLE, &mpi_row);
  MPI_Type_commit(&mpi_row);

  auto h = 1.0 / TN;

  mat newu = u;
  auto error = 2 * epsilon;
  while (error > epsilon) {

    int source, destination;
    MPI_Status status;

    MPI_Cart_shift(grid_comm, 1, 1, &source, &destination);
    MPI_Sendrecv(&u(1, Nc), 1, mpi_column, destination, 1, &u(1, 0), 1,
                 mpi_column, source, 1, grid_comm, &status);

    MPI_Cart_shift(grid_comm, 1, -1, &source, &destination);
    MPI_Sendrecv(&u(1, 1), 1, mpi_column, destination, 2, &u(1, Nc + 1), 1,
                 mpi_column, source, 2, grid_comm, &status);

    MPI_Cart_shift(grid_comm, 0, 1, &source, &destination);
    MPI_Sendrecv(&u(Nr, 1), 1, mpi_row, destination, 3, &u(0, 1), 1, mpi_row,
                 source, 3, grid_comm, &status);

    MPI_Cart_shift(grid_comm, 0, -1, &source, &destination);
    MPI_Sendrecv(&u(1, 1), 1, mpi_row, destination, 4, &u(Nr + 1, 1), 1,
                 mpi_row, source, 4, grid_comm, &status);

    auto newucenter = newu(span(1, Nr), span(1, Nc));
    auto uleft = u(span(1, Nr), span(0, Nc - 1));
    auto uright = u(span(1, Nr), span(2, Nc + 1));
    auto udown = u(span(0, Nr - 1), span(1, Nc));
    auto uup = u(span(2, Nr + 1), span(1, Nc));

    newucenter = 0.25 * (uleft + uright + udown + uup - h * h * f);
    auto local_error = abs(newu - u).max();

    MPI_Allreduce(&local_error, &error, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

    u.swap(newu);
  }

  MPI_Datatype mpi_mat_send;
  MPI_Type_vector(Nc, Nr, Nr + 2, MPI_DOUBLE, &mpi_mat_send);
  MPI_Type_commit(&mpi_mat_send);

  MPI_Status status;
  MPI_Request req;
  MPI_Isend(&u(1, 1), 1, mpi_mat_send, 0, 5, MPI_COMM_WORLD, &req);
  if (rank == 0) {
    mat global_u(TN, TN);
    std::array<int, 2> icoords;
    mat ublock(Nr, Nc);
    for (int i = 0; i < nprocs; ++i) {
      MPI_Cart_coords(grid_comm, i, 2, &icoords[0]);
      MPI_Recv(&ublock(0, 0), Nr * Nc, MPI_DOUBLE, i, 5, MPI_COMM_WORLD,
               &status);
      auto rows = span(Nr * icoords[0], Nr * icoords[0] + Nr - 1);
      auto cols = span(Nc * icoords[1], Nc * icoords[1] + Nc - 1);
      global_u(rows, cols) = ublock;
    }

    global_u.save(filename, raw_ascii);
  }

  MPI_Wait(&req, &status);

  MPI_Finalize();
  return 0;
}

std::tuple<int, double, std::string> read_args(int argc, char *argv[],
                                               int rank) {
  if (argc != 4) {
    if (rank == 0) {
      std::cerr << "Use as: " << argv[0]
                << " <mesh size> <tolerance> <filename>\n";
    }
    MPI_Finalize();
    exit(1);
  }

  auto N = std::stoi(argv[1]);
  if (N <= 0) {
    if (rank == 0) {
      std::cerr << "Mesh size must be positive.\n";
    }
    MPI_Finalize();
    exit(2);
  }

  auto epsilon = std::stod(argv[2]);
  if (epsilon <= 0) {
    if (rank == 0) {
      std::cerr << "Tolerance must be positive.\n";
    }
    MPI_Finalize();
    exit(2);
  }

  return {N, epsilon, std::string(argv[3])};
}

std::tuple<int, int> compute_local_sizes(int N, std::array<int, 2> dims) {
  auto Nr = N / dims[0] + static_cast<int>(N % dims[0] != 0);
  auto Nc = N / dims[1] + static_cast<int>(N % dims[1] != 0);
  auto TNr = dims[0] * Nr;
  auto TNc = dims[1] * Nc;
  while (TNr != TNc) {
    if (TNr < TNc) {
      ++Nr;
      TNr += dims[0];
    } else {
      ++Nc;
      TNc += dims[1];
    }
  }

  return {Nr, Nc};
}

std::tuple<int, int> find_f_overlap(int f_first, int f_last, int grid_pos,
                                    int Nblock) {
  auto first = std::max(f_first, grid_pos * Nblock);
  auto last = std::min(f_last, grid_pos * Nblock + Nblock - 1);
  auto size = last >= first ? last - first + 1 : 0;

  return {size, first - grid_pos * Nblock};
}
