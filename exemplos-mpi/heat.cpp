#include <armadillo>
#include <cstdlib>
#include <iostream>
#include <string>
#include <tuple>

using namespace arma;

std::tuple<int, double, std::string> read_args(int argc, char *argv[]);

int main(int argc, char *argv[]) {
  double const outside_temp = 0.;
  double box_f_value = -100.;

  auto [N, epsilon, filename] = read_args(argc, argv);

  mat u(N + 2, N + 2, fill::zeros);

  u.row(0) = outside_temp * ones(1, N + 2);
  u.row(N + 1) = outside_temp * ones(1, N + 2);
  u.col(0) = outside_temp * ones(N + 2, 1);
  u.col(N + 1) = outside_temp * ones(N + 2, 1);

  auto box_first = 2 * N / 5;
  auto box_last = 3 * N / 5;
  auto box_size = box_last - box_first + 1;

  mat f(N, N, fill::zeros);
  f(span(box_first, box_last), span(box_first, box_last)) =
      box_f_value * ones(box_size, box_size);

  auto h = 1. / N;

  mat newu = u;
  auto error = 2 * epsilon;
  while (error > epsilon) {
    auto newucenter = newu(span(1, N), span(1, N));
    auto uleft = u(span(1, N), span(0, N - 1));
    auto uright = u(span(1, N), span(2, N + 1));
    auto udown = u(span(0, N - 1), span(1, N));
    auto uup = u(span(2, N + 1), span(1, N));

    newucenter = 0.25 * (uleft + uright + udown + uup - h * h * f);
    error = abs(newu - u).max();

    u.swap(newu);
  }

  u.save(filename, raw_ascii);
  return 0;
}

std::tuple<int, double, std::string> read_args(int argc, char *argv[]) {
  if (argc != 4) {
    std::cerr << "Use as: " << argv[0] << " <mesh size> <tolerance> <filename>\n";
    exit(1);
  }

  auto N = std::stoi(argv[1]);
  if (N <= 0) {
    std::cerr << "Mesh size must be positive.\n";
    exit(2);
  }

  auto epsilon = std::stod(argv[2]);
  if (epsilon <= 0) {
    std::cerr << "Tolerance must be positive.\n";
    exit(2);
  }

  return {N, epsilon, std::string(argv[3])};
}
