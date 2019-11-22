
#include <iostream>
#include <omp.h>

// Shows how to create new threads in OpenMP.
// OpenMP uses fork/join parallelism.

int main() {
  // How many processors are there?
  int nprocs = omp_get_num_procs();

  std::cout << "Running on " << nprocs << " processor";
  if (nprocs > 1) {
    std::cout << "s";
  }
  std::cout << ".\n";

  // Set number of threads. 4 times the number of processors. You
  // could use instead num_threads(4*nprocs) at the omp parallel
  // pragma below.
  omp_set_num_threads(4 * nprocs);

  int nthreads, tid;

  // Fork a team of threads giving them their own copies of variable tid
#pragma omp parallel default(none) private(tid, nthreads) shared(std::cout)
  {
    // Obtain and print thread id.
    tid = omp_get_thread_num();
    std::cout << "Hello World from thread " << tid << std::endl;

    // Only master thread does this
    if (tid == 0) {
      nthreads = omp_get_num_threads();
      std::cout << "The total current number of threads is " << nthreads
                << std::endl;
    }
  } // All threads join master thread and terminate.

  std::cout << "\nNow there is only one thread.\n";

  return 0;
}
