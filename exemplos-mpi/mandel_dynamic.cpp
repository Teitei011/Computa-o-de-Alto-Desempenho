#include <algorithm>
#include <array>
#include <complex>
#include <fstream>
#include <mpi.h>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

using Point = std::complex<double>;    // Um ponto no plano
using Resolution = std::array<int, 2>; // Resolucao x (largura), y (altura)

// Alguns tipos de excecao para erros de linha de comando.
struct InvalidRegionException {};
struct InvalidResolutionException {};

// Classe para representar uma imagem.
// Cada pixel e o numero de iteracoes para detectar |z_n| > 2
class Image {
  std::vector<uint8_t> _values;
  size_t _nrows, _ncols;

public:
  Image(size_t nrows = 0, size_t ncols = 0)
      : _values(nrows * ncols), _nrows(nrows), _ncols(ncols) {}

  void resize(size_t nrows, size_t ncols) {
    _values.resize(nrows * ncols);
    _nrows = nrows;
    _ncols = ncols;
  }

  size_t num_rows() const { return _nrows; }
  size_t num_cols() const { return _ncols; }

  // Operador de funcao e usado para indexacao (linha, coluna).
  uint8_t &operator()(size_t i, size_t j) { return _values[i * _ncols + j]; }

  uint8_t const &operator()(size_t i, size_t j) const {
    return _values[i * _ncols + j];
  }
};

//
// Prototipos de alguma funcoes usadas pelo main.
//

// Escrita de imagem em stream.
std::ostream &operator<<(std::ostream &os, Image const &image);

// Leitura de argumentos de linha de comando.
// Retorna tupla (canto esquerdo inferior, canto direito superior, resolucao).
std::tuple<Point, Point, Resolution> read_args(int argc, char *argv[]);

// Executa codigo do mestre, distribuindo tarefas entre escravos e coletando
// resultados.
Image master(Resolution resolution, int n_slaves);

// Executa tarefa de um dos escravos (identificado pelo rank)
void slave(Point lower_left, Point upper_right, Resolution resolution);

//
// Codigo principal.
//

int main(int argc, char *argv[]) {
  int rank, nprocs;

  MPI_Init(&argc, &argv);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  if (nprocs == 1) {
    std::cerr << "At least one slave is needed.\n";
    MPI_Finalize();
    return 1;
  }

  try {
    auto [lower_left, upper_right, resolution] = read_args(argc, argv);
    if (rank == 0) {
      auto image = master(resolution, nprocs - 1);

      std::ofstream output(argv[7]);
      if (output.good()) {
        output << image;
      } else {
        std::cerr << "Could not open output file.\n";
      }
    } else {
      slave(lower_left, upper_right, resolution);
    }
  }
  // Now for some lame error handling.
  catch (InvalidRegionException e) {
    std::cerr << "Position of region delimiting points is wrong.\n";
    MPI_Abort(MPI_COMM_WORLD, 1);
  } catch (InvalidResolutionException e) {
    std::cerr << "Image resolution is wrong.\n";
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  MPI_Finalize();
  return 0;
}

std::ostream &operator<<(std::ostream &os, Image const &image) {
  for (size_t i = 0; i < image.num_rows(); ++i) {
    for (size_t j = 0; j < image.num_cols(); ++j) {
      os << static_cast<int>(image(i, j)) << " ";
    }
    os << std::endl;
  }
  return os;
}

std::tuple<Point, Point, Resolution> read_args(int, char *argv[]) {
  double ar = std::stof(argv[1]);
  double ai = std::stof(argv[2]);
  double br = std::stof(argv[3]);
  double bi = std::stof(argv[4]);

  if (ar >= br || ai >= bi) {
    throw InvalidRegionException();
  }

  Point lower_left(ar, ai), upper_right(br, bi);

  int inx = std::stoi(argv[5]);
  int iny = std::stoi(argv[6]);

  if (inx <= 0 || iny <= 0) {
    throw InvalidResolutionException();
  }

  auto resolution = Resolution{inx, iny};

  return {lower_left, upper_right, resolution};
}

enum Tags { INIT_TAG, TASK_TAG, RESULT_TAG, END_TAG };

Image master(Resolution resolution, int n_slaves) {
  int const TASK_SIZE = 10;
  int current_base = 0;

  MPI_Datatype mpi_row;
  MPI_Type_contiguous(resolution[0], MPI_UINT8_T, &mpi_row);
  MPI_Type_commit(&mpi_row);

  Image image(resolution[1], resolution[0]);

  int active_slaves = n_slaves;

  while (active_slaves > 0) {
    std::array<int, 2> task_data;
    MPI_Status status;

    MPI_Recv(&task_data[0], 2, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
             MPI_COMM_WORLD, &status);
    auto idle_slave = status.MPI_SOURCE;

    if (status.MPI_TAG == RESULT_TAG) {
      auto [starting_row, num_rows] = task_data;
      MPI_Recv(&image(starting_row, 0), num_rows, mpi_row, idle_slave,
               RESULT_TAG, MPI_COMM_WORLD, &status);
    }

    if (current_base < resolution[1]) {
      task_data[0] = current_base;
      if (current_base + TASK_SIZE <= resolution[1]) {
        task_data[1] = TASK_SIZE;
      } else {
        task_data[1] = resolution[1] - current_base;
      }

      current_base += task_data[1];

      MPI_Send(&task_data[0], 2, MPI_INT, idle_slave, TASK_TAG, MPI_COMM_WORLD);
    } else {
      MPI_Send(&task_data[0], 0, MPI_INT, idle_slave, END_TAG, MPI_COMM_WORLD);
      --active_slaves;
    }
  }

  return image;
}

uint8_t mandel(Point c, uint8_t iter_max = 127) {
  uint8_t iter{0};
  Point z{c};

  while (iter < iter_max && abs(z) <= 2.0) {
    ++iter;
    z = z * z + c;
  }

  return iter;
}

Image mandel_block(Point lower_left, Point upper_right, Resolution resolution) {
  double hx = (upper_right.real() - lower_left.real()) / resolution[0];
  double hy = (upper_right.imag() - lower_left.imag()) / resolution[1];

  Image image(resolution[1], resolution[0]);

  for (int row = 0; row < resolution[1]; ++row) {
    auto y = lower_left.imag() + row * hy + hy / 2;
    for (int col = 0; col < resolution[0]; ++col) {
      auto x = lower_left.real() + col * hx + hx / 2;
      image(row, col) = mandel(Point{x, y});
    }
  }

  return image;
}

void slave(Point lower_left, Point upper_right, Resolution resolution) {
  std::array<int, 2> task_data;

  MPI_Datatype mpi_row;

  // Tamanho vertical de um pixel.
  auto hy = (upper_right.imag() - lower_left.imag()) / resolution[1];

  MPI_Type_contiguous(resolution[0], MPI_UINT8_T, &mpi_row);
  MPI_Type_commit(&mpi_row);

  MPI_Send(&task_data[0], 0, MPI_INT, 0, INIT_TAG, MPI_COMM_WORLD);

  while (true) {
    MPI_Status status;
    MPI_Recv(&task_data[0], 2, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD,
             &status);

    if (status.MPI_TAG == END_TAG) {
      break;
    }

    auto [starting_row, num_rows] = task_data;

    // Calculo dos limites da regiao do rank atual.
    auto inferior_imag = lower_left.imag() + hy * starting_row;
    auto superior_imag = inferior_imag + hy * num_rows;

    Point local_lower_left{lower_left.real(), inferior_imag};
    Point local_upper_right{upper_right.real(), superior_imag};

    Resolution local_resolution{resolution[0], num_rows};

    auto image =
        mandel_block(local_lower_left, local_upper_right, local_resolution);

    MPI_Send(&task_data[0], 2, MPI_INT, 0, RESULT_TAG, MPI_COMM_WORLD);
    MPI_Send(&image(0, 0), num_rows, mpi_row, 0, RESULT_TAG, MPI_COMM_WORLD);
  }
}
