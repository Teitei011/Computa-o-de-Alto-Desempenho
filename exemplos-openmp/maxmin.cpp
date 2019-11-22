#include <cassert>
#include <iostream>
#include <string>

// Sharing without race.

int main(int argc, char *argv[]) {
  assert(argc == 3);

  auto a = std::stoi(argv[1]);
  auto b = std::stoi(argv[2]);

  int max, min;

#pragma omp parallel default(none) shared(max, min, a, b) num_threads(2)
  {
#pragma omp sections
    {
#pragma omp section
      { // Start first section
        if (a < b) {
          min = a;
        } else {
          min = b;
        }
      } // End of first section.

#pragma omp section
      { // Start second section
        if (a > b) {
          max = a;
        } else {
          max = b;
        }
      } // End of second section.
    }   // End of sections. Synchronize threads.
  }     // End of parallel execution.

  std::cout << "Largest is " << max << ", smallest is " << min << std::endl;

  return 0;
}
