#include <algorithm>
#include <cassert>
#include <climits>
#include <iostream>
#include <omp.h>
#include <random>
#include <string>

double dot_product(std::vector<double> const &a, std::vector<double> const &b);

int main(int argc, char *argv[]) {
  assert(argc == 3);

  auto N = std::stoi(argv[1]);
  assert(N > 0);

  auto num_threads = std::stoi(argv[2]);
  assert(num_threads > 0);

  // Random number generation
  std::random_device rd;
  std::default_random_engine rand_gen(rd());
  std::uniform_real_distribution<double> dist(0, 1);
  auto gen_rand_real = [&]() { return dist(rand_gen); };

  std::vector<double> a(N), b(N);
  std::generate(begin(a), end(a), gen_rand_real);
  std::generate(begin(b), end(b), gen_rand_real);

  omp_set_num_threads(num_threads);

  double dotprod = dot_product(a, b);

  std::cout << "The dot product is " << dotprod << std::endl;

  return 0;
}

double dot_product(std::vector<double> const &a, std::vector<double> const &b) {
  double s{0.0};
  size_t N = a.size(); // b must be the same size
#pragma omp parallel for default(none) shared(a,b,N) reduction(+:s)     \
    schedule(static,10)
  for (size_t i = 0; i < N; ++i) {
    s += a[i] * b[i];
  }
  return s;
}
