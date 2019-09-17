#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace chrono = std::chrono;

// Hand-mplemented quicksort of a vector of ints (for comparison)
void quick_sort(std::vector<int> &v);

// Custom function to read command line arguments
std::pair<int, int> read_arguments(int argc, char *argv[]);

/**
 * Read number of elements (N) from the command line, generate a
 * random vector of this size and sort it.
 * Repeat M times for statistics (M is the second command line argument).
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

  for (int j = 0; j < M; ++j) {

    std::generate(begin(rnd_array), end(rnd_array), draw);

    t1 = chrono::high_resolution_clock::now();

    quick_sort(rnd_array);

    t2 = chrono::high_resolution_clock::now();

    auto dt = chrono::duration_cast<chrono::microseconds>(t2 - t1);
    elapsed += dt.count();
  }

  // Show timing resuts.
  std::cout << N << " " << elapsed / M / 1e6 << std::endl;
}

// Return the middle value among a, b and c. If two are equal, return
// one of the equals.
inline int median_of_three(int a, int b, int c) {
  if (a < b) {
    if (b < c)
      return b; // a < b && b < c
    else if (a < c)
      return c; // a < c && c <= b
    else
      return a; // c <= a && a < b
  } else {
    if (a < c)
      return a; // b <= a && a < c
    else if (b < c)
      return c; // b < c && c <= a
    else
      return b; // c <= b && b <= a
  }
}

using vector_int_it = std::vector<int>::iterator;

// Shrinks interval [st, fn) on the left while first value is less than val.
inline void shrink_left_while_less(vector_int_it &st, vector_int_it &fn,
                                   int val) {
  while (st != fn && *st < val) {
    ++st;
  }
}

// Shrinks interval [st, fn) on the left while first value is equal to val.
inline void shrink_left_while_equal(vector_int_it &st, vector_int_it &fn,
                                    int val) {
  while (st != fn && *st == val) {
    ++st;
  }
}

// Shinks interval [st, fn) on the right while last value is greater than val.
inline void shrink_right_while_greater(vector_int_it &st, vector_int_it &fn,
                                       int val) {
  while (st != fn && *(fn - 1) > val) {
    --fn;
  }
}

/**
 * Sort elements from ini (included) to fin (excluded) using
 * quicksort.
 */
void quick_sort(vector_int_it const &ini, vector_int_it const &fin) {
  auto interval_size = fin - ini;
  if (interval_size > 2) {

    // Choose the key as median from first, last and middle
    int key = median_of_three(*ini, *(ini + interval_size / 2), *(fin - 1));

    // Split elements according to key
    auto end_small = ini;
    auto begin_large = fin;
    shrink_left_while_less(end_small, begin_large, key);
    auto end_equal = end_small;
    shrink_left_while_equal(end_equal, begin_large, key);
    shrink_right_while_greater(end_equal, begin_large, key);
    while (end_equal != begin_large) {
      // In [ini,end_small) all are < key
      // In [end_small, end_equal) all are == key
      // In [begin_large, fin) all are > key
      // In [end_equal, begin_large) are unknown
      // *end_equal != key

      // Put value at end_equal at the right place
      if (*end_equal < key) {
        std::iter_swap(end_equal, end_small);
        // Ranges of smallers increased by one, range of
        // equals shifted.
        ++end_small;
        ++end_equal;
      } else {
        std::iter_swap(end_equal, begin_large - 1);
        // Increase range of greaters while possible
        shrink_right_while_greater(end_equal, begin_large, key);
      }
      // Increase range of equals if possible
      shrink_left_while_equal(end_equal, begin_large, key);
    }

    // Recursive call.  The two are garateed to be smaller than
    // the original, as the value(s) equal to key are ignored.
    quick_sort(ini, end_small);
    quick_sort(begin_large, fin);
  } else if (interval_size == 2 && *ini > *(ini + 1)) {
    std::iter_swap(ini, ini + 1);
  }
}

/**
 * Sort all elements of array v using quick sort.
 */
void quick_sort(std::vector<int> &v) { quick_sort(begin(v), end(v)); }

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
