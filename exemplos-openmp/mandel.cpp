#include <algorithm>
#include <array>
#include <complex>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>

using Point = std::complex<double>;    // Um ponto no plano
using Resolution = std::array<int, 2>; // Resolucao x (largura), y (altura)

// Alguns tipos de excecao para erros de linha de comando.
struct TooFewArgsException {};
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

// Calcula a imagem de Mandelbrot para regiao delimitada por
// lower_left e upper_right, com a resolucao especificada.
Image mandel_block(Point lower_left, Point upper_right, Resolution resolution);

//
// Codigo principal.
//

int main(int argc, char *argv[]) {
  try {
    // Here is the interesting part.

    auto [lower_left, upper_right, resolution] = read_args(argc, argv);

    auto image = mandel_block(lower_left, upper_right, resolution);

    if (argc < 8) {
      throw TooFewArgsException();
    }

    std::ofstream output(argv[7]);
    if (output.good()) {
      output << image;
    } else {
      std::cerr << "Could not open output file.\n";
    }
  }
  // Now for some lame error handling.
  catch (TooFewArgsException e) {
    std::cerr << "usage: " << argv[0] << " <lowerleftreal> <lowerleftimag> "
              << " <upperrightreal> <upperrightimag> "
              << " <resolution-x> <resolution-y> "
              << " <outputfilename>\n";
  } catch (InvalidRegionException e) {
    std::cerr << "Position of region delimiting points is wrong.\n";
  } catch (InvalidResolutionException e) {
    std::cerr << "Image resolution is wrong.\n";
  }

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

std::tuple<Point, Point, Resolution> read_args(int argc, char *argv[]) {
  if (argc < 7) {
    throw TooFewArgsException();
  }

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

#pragma omp parallel for default(none)                                         \
    shared(resolution, lower_left, hx, hy, image) schedule(static, 10)
  for (int row = 0; row < resolution[1]; ++row) {
    auto y = lower_left.imag() + row * hy + hy / 2;
    for (int col = 0; col < resolution[0]; ++col) {
      auto x = lower_left.real() + col * hx + hx / 2;
      image(row, col) = mandel(Point{x, y});
    }
  }

  return image;
}
