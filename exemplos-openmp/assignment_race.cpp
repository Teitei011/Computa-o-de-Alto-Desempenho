#include <cassert>
#include <iostream>
#include <omp.h>
#include <string>

// Here we have a race condition on the variable value.

int main(int argc, char *argv[]) {
  assert(argc == 2);

  int n_threads = std::stoi(argv[1]);
  assert(n_threads > 0);

  int value, tid;

#pragma omp parallel default(none) shared(value) private(tid)                  \
    num_threads(n_threads)
  {
    tid = omp_get_thread_num();
    value = 2 * tid;
  }

  // Guess what will be printed.
  std::cout << "Value is " << value << std::endl;

  return 0;
}
