#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <fstream>
#include <mpi.h>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

using Point = std::complex<double>;    // Um ponto no plano
                                       // real: direcao x
                                       // imaginario: direcao y
using Resolution = std::array<int, 2>; // Resolucao x (largura) , y (altura)

// Alguns tipos de excecao para erros de linha de comando.
struct InvalidRegionException {};
struct InvalidResolutionException {};

// Classe para representar uma imagem.
// Cada pixel e o numero de iteracoes para detectar |z_n| > 2
class Image {
  std::vector<uint8_t> _values;
  size_t _nrows, _ncols; // linhas: y, colunas: x

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

// Calcula como dividir n elementos em nprocs processos.
std::tuple<std::vector<int>, std::vector<int>> compute_division(int n,
                                                                int nprocs);

// Calcula a imagem de Mandelbrot para regiao delimitada por
// lower_left e upper_right, com a resolucao especificada.
Image mandel_block(Point lower_left, Point upper_right, Resolution resolution);

// Calcula numero de processos nas direcoes x e y, tentando ser orioircuibak
// a resolucao fornecida.
std::tuple<int, int> grid_size(int nprocs, Resolution resolution);

// Calcula posicao x e y do rank em grade com nprocs_x x nprocs_y.
std::tuple<int, int> grid_coords(int rank, int nprocs_row, int nprocs_col);

//
// Codigo principal.
//

int main(int argc, char *argv[]) {
  int rank, nprocs;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  try {
    // Here is the interesting part.

    auto [lower_left, upper_right, resolution] = read_args(argc, argv);
    auto [nprocs_x, nprocs_y] = grid_size(nprocs, resolution);
    auto [ncols, starting_col] = compute_division(resolution[0], nprocs_x);
    auto [nrows, starting_row] = compute_division(resolution[1], nprocs_y);

    auto [row_rank, col_rank] = grid_coords(rank, nprocs_y, nprocs_x);

    // Tamanho vertical de um pixel.
    auto hy = (upper_right.imag() - lower_left.imag()) / resolution[1];

    // Calculo dos limites y da regiao do rank atual.
    auto inferior_imag = lower_left.imag() + hy * starting_row[row_rank];
    auto superior_imag =
        row_rank != nprocs_y - 1
            ? lower_left.imag() + hy * starting_row[row_rank + 1]
            : upper_right.imag();

    // Tamanho horizontal de um pixel.
    auto hx = (upper_right.real() - lower_left.real()) / resolution[0];

    // Calculo dos limites x da regiao do rank atual.
    auto inferior_real = lower_left.real() + hx * starting_col[col_rank];
    auto superior_real =
        col_rank != nprocs_x - 1
            ? lower_left.real() + hx * starting_col[col_rank + 1]
            : upper_right.real();

    Point local_lower_left{inferior_real, inferior_imag};
    Point local_upper_right{superior_real, superior_imag};

    Resolution local_resolution{ncols[col_rank], nrows[row_rank]};

    auto image_local =
        mandel_block(local_lower_left, local_upper_right, local_resolution);

    // Define um tipo para a recepcao dos dados de cada um dos ranks.
    std::vector<MPI_Datatype> recv_type(nprocs);

    for (int i = 0; i < nprocs; ++i) {
      auto [row_i, col_i] = grid_coords(i, nprocs_y, nprocs_x);
      MPI_Type_vector(nrows[row_i], ncols[col_i], resolution[0], MPI_UINT8_T,
                      &recv_type[i]);
      MPI_Type_commit(&recv_type[i]);
    }

    MPI_Request send_req;
    MPI_Status st;

    // Envia dados locais para rank 0
    MPI_Isend(&image_local(0, 0), nrows[row_rank] * ncols[col_rank],
              MPI_UINT8_T, 0, 1, MPI_COMM_WORLD, &send_req);

    Image image_global;

    if (rank == 0) {
      image_global.resize(resolution[1], resolution[0]);

      // Rank 0 recebe dados enviados e coloca nas posicoes adequadas.
      for (int i = 0; i < nprocs; ++i) {
        auto [row_i, col_i] = grid_coords(i, nprocs_y, nprocs_x);
        MPI_Recv(&image_global(starting_row[row_i], starting_col[col_i]), 1,
                 recv_type[i], i, 1, MPI_COMM_WORLD, &st);
      }

      // Salva imagem.
      std::ofstream output(argv[7]);
      if (output.good()) {
        output << image_global;
      } else {
        std::cerr << "Could not open output file.\n";
      }
    }

    MPI_Wait(&send_req, &st);
  }
  // Now for some lame error handling.
  catch (InvalidRegionException e) {
    if (rank == 0) {
      std::cerr << "Position of region delimiting points is wrong.\n";
    }
  } catch (InvalidResolutionException e) {
    if (rank == 0) {
      std::cerr << "Image resolution is wrong.\n";
    }
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

std::tuple<int, int> grid_size(int nprocs, Resolution resolution) {
  auto px = static_cast<int>(round(
      sqrt(nprocs * (resolution[0] / static_cast<double>(resolution[1])))));
  if (px == 0) {
    px = 1;
  }

  while (nprocs % px != 0) {
    --px;
  }

  return {px, nprocs / px};
}

std::tuple<int, int> grid_coords(int rank, int nprocs_row, int nprocs_col) {
  auto row = rank / nprocs_col;
  auto col = rank % nprocs_col;

  return {row, col};
}

std::tuple<std::vector<int>, std::vector<int>> compute_division(int n,
                                                                int nprocs) {
  std::vector<int> conts(nprocs), desls(nprocs);

  int q = n / nprocs, r = n % nprocs;

  std::fill_n(begin(conts), r, q + 1);
  std::fill(begin(conts) + r, end(conts), q);

  desls[0] = 0;
  std::partial_sum(begin(conts), end(conts) - 1, begin(desls) + 1);

  return {conts, desls};
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
