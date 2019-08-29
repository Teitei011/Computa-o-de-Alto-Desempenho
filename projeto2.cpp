#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <Eigen/Dense>
#include <stdexcept>
#include <string>
#include <vector>

namespace chrono = std::chrono;
using Eigen::MatrixXd;

std::pair<int, int> read_arguments(int argc, char *argv[]);


int main(int, char const *[]) {
  auto [N, Iteracao] = read_arguments(argc, argcv);

  // Function for random number generation
  std::random_device entropy;
  std::mt19937 gen(entropy()); // Randomness generator.
  std::uniform_int_distribution<> dis(0, 1000 * N);

  // Criacao de Matrizes
  auto A = MatrixXd::Random(N, N);
  auto B = MatrixXd::Random(N, N);
  auto C = MatrixXd::Zero(N, N);

  // Matrix Multiplication
  for (int i = 0; i = N; i++){
    for (int j = 0; j = N; j++){

    }
  }

  return 0;
}
