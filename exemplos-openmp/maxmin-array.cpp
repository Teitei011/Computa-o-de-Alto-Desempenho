#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  assert(argc == 2);

  int max, min;

  auto N = std::stoi(argv[1]);
  assert(N > 0);

  // Random number generation.
  std::random_device rd;
  std::default_random_engine rand_gen(rd());
  std::uniform_int_distribution<> dist(0, std::numeric_limits<int>::max());

  // Create a random array of N int elements.
  std::vector<int> array(N);
  auto random_int = [&]() { return dist(rand_gen); };
  std::generate(begin(array), end(array), random_int);

#pragma omp parallel default(none) shared(max, min, N, array) num_threads(2)
  {
#pragma omp sections
    {
#pragma omp section
      { // First section computes minimum.
        min = array[0];
        for (int i = 1; i < N; ++i) {
          if (array[i] < min) {
            min = array[i];
          }
        }
      }

#pragma omp section
      { // Second section computes maximum.
        max = array[0];
        for (int i = 1; i < N; ++i) {
          if (array[i] > max) {
            max = array[i];
          }
        }
      }
    } // End of sections. Synchronize threads.
  }   // End of parallel execution.

  std::cout << "Generated " << N << " random numbers." << std::endl;
  std::cout << "Largest is " << max << std::endl
            << "Smallest is " << min << std::endl;

  return 0;
}
