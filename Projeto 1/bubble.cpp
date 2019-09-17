#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace chrono = std::chrono;

void bubble_sort(std::vector<int> &v);

// Custom function to read command line arguments
std::pair<int, int> read_arguments(int argc, char *argv[]);

/**
 * Read number of elements (N) from the command line, generate a
 * random vector of this size and sort it.  Repeat M times for
 * statistics (M is the second command line argument).
 */
int main(int argc, char *argv[]) {
  // Read N and M from command line
  auto [N, M] = read_arguments(argc, argv);

  // Function for random number generation
  std::random_device entropy;
  std::mt19937 gen(entropy()); // Randomness generator.
  std::uniform_int_distribution<> dis(0, 1000 * N);
  auto draw = [&gen, &dis]() { return dis(gen); };

  std::vector<int> rnd_array(N);

  // Time sorting M random arrays of N elements.
  double elapsed = 0;
  chrono::high_resolution_clock::time_point t1, t2;

  for (int j = 0; j < M; j++) {

    std::generate(begin(rnd_array), end(rnd_array), draw);

    t1 = chrono::high_resolution_clock::now();

    bubble_sort(rnd_array);

    t2 = chrono::high_resolution_clock::now();

    auto dt = chrono::duration_cast<chrono::microseconds>(t2 - t1);
    elapsed += dt.count();
  }

  // Show timing resuts.
  std::cout << N << " " << elapsed / M / 1e6 << std::endl;
}

/**
 * Sort the elements of array v using bubble sort.
 */
void bubble_sort(std::vector<int> &v) {
  bool swapped;
  unsigned int n = v.size();
  do {
    swapped = false;
    for (unsigned int i = 0; i < n - 1; ++i)
      if (v[i + 1] < v[i]) {
        std::swap(v[i], v[i + 1]);
        swapped = true;
      }
    --n;
  } while (swapped);
}

std::pair<int, int> read_arguments(int argc, char *argv[]) {
  int N, M;

  // We need exactly three arguments (considering program name).
  if (argc != 3) {
    std::cout << "Usage: " << argv[0]
              << " <number of elements> <number of arrays>\n";
    exit(1);
  }

  // Read arguments.
  try {
    N = std::stoi(argv[1]);
    M = std::stoi(argv[2]);
  } catch (std::invalid_argument &) {
    std::cerr << "Command line argument invalid: must be int.\n";
    exit(1);
  } catch (std::out_of_range &) {
    std::cerr << "Number too large or too small.\n";
    exit(1);
  }

  return {N, M};
}
