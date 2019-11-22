#include <cassert>
#include <iostream>
#include <numeric>
#include <omp.h>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  assert(argc == 3);

  auto N = std::stoi(argv[1]);
  assert(N > 0);

  auto num_threads = std::stoi(argv[2]);
  assert(num_threads > 0);

  // Create an array from 0 to N-1.
  std::vector<int> array(N);
  std::iota(begin(array), end(array), 0);

  // Use one accumulator per thread
  std::vector<int> acc(num_threads);

  int sum{0};

#pragma omp parallel default(none) shared(N, array, acc, num_threads)          \
    num_threads(num_threads)
  {
    int tid = omp_get_thread_num();

    acc[tid] = 0;
    for (int i = tid; i < N; i += num_threads) {
      acc[tid] += array[i];
    }

  } // End of parallel execution.

  for (int i = 0; i < num_threads; i++) {
    sum += acc[i];
  }

  if (sum == N * (N - 1) / 2) {
    std::cout << "Nice, I can sum!" << std::endl;
  } else {
    std::cout << "Back to school!" << std::endl;
  }

  return 0;
}