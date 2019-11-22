#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <omp.h>
#include <random>
#include <string>

using Vector = std::vector<double>;

struct Matrix : public std::vector<Vector> {
  Matrix(size_t nlin, size_t ncol) : std::vector<Vector>(nlin, Vector(ncol)) {}
};

double dot_product(Vector const &a, Vector const &b);
Vector matrix_vector_product(Matrix const &m, Vector const &v);

int main(int argc, char *argv[]) {
  assert(argc == 3);

  auto N = std::stoi(argv[1]);
  assert(N > 0);

  auto num_threads = std::stoi(argv[2]);
  assert(num_threads > 0);

  omp_set_num_threads(num_threads);
  // omp_set_nested(true); // Not needed here.

  double t_ini = omp_get_wtime();

  Vector v(N);
  Matrix m(N, N);

  // Random number generation
  std::random_device rd;
  std::default_random_engine rand_gen(rd());
  std::uniform_real_distribution<double> dist(0, 1);
  auto gen_rand_real = [&]() { return dist(rand_gen); };

  std::generate(begin(v), end(v), gen_rand_real);

  for (int i = 0; i < N; i++) {
    std::generate(begin(m[i]), end(m[i]), gen_rand_real);
  }

  Vector vres = matrix_vector_product(m, v);

  double s{0.0};
#pragma omp parallel for default(none) shared(vres, N) reduction(+ : s)
  for (int i = 0; i < N; i++) {
    s += vres[i];
  }

  double t_fin = omp_get_wtime();

  std::cout << std::setprecision(15);
  std::cout << "The sum of the resulting vector is " << s << std::endl;
  std::cout << "Time taken: " << t_fin - t_ini << std::endl;

  return 0;
}

double dot_product(Vector const &a, Vector const &b) {
  double s{0.0};
  size_t N = a.size();

#pragma omp parallel for default(none) shared(a,b,N) reduction(+:s)     \
    schedule(static)
  for (size_t i = 0; i < N; ++i) {
    s += a[i] * b[i];
  }

  return s;
}

Vector matrix_vector_product(Matrix const &m, Vector const &v) {
  size_t nrows = m.size();
  Vector res(nrows);

#pragma omp parallel for default(none) shared(res, m, v, nrows)
  for (size_t i = 0; i < nrows; ++i) {
    res[i] = dot_product(m[i], v);
  }

  return res;
}
