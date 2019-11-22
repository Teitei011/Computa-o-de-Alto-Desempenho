#include <cassert>
#include <iostream>
#include <omp.h>
#include <string>

// Race due to sharing.

int main(int argc, char *argv[]) {
  assert(argc == 3);

  int N = std::stoi(argv[1]);
  assert(N > 0);

  int n_threads = std::stoi(argv[2]);
  assert(n_threads > 0);

  int sum{0};

  /* Fork the threads */
#pragma omp parallel default(none) shared(N, sum, n_threads)                   \
    num_threads(n_threads)
  {
    int tid = omp_get_thread_num();
    for (int i = tid; i < N; i += n_threads) {
      sum += i;
    }
  } /* All threads join master thread and terminate */

  std::cout << "Final value is " << sum << ", expected: " << (N - 1) * N / 2
            << std::endl;

  return 0;
}
