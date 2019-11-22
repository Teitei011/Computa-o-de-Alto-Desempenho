#include <cassert>
#include <chrono>
#include <iostream>
#include <omp.h>
#include <random>
#include <string>
#include <thread>
#include <vector>

// Global random number generator.
std::mt19937 rand_gen{std::random_device{}()};

// A type for the shared buffer.
struct buffer_type {
#ifdef DEBUG
  long _num_acc = 0;
#endif
  static int constexpr size = 10;
  std::vector<int> value;
  int n{0};
  buffer_type() : value(size) {}
};

void consumer(buffer_type &buffer);
void producer(buffer_type &buffer);

int main(int argc, char *argv[]) {
  assert(argc == 2);

  auto num_threads = std::stoi(argv[1]);
  assert(num_threads > 1);
  omp_set_num_threads(num_threads);

  buffer_type buffer;

#pragma omp parallel default(none) shared(buffer)
  {
    int tid = omp_get_thread_num();
    if (tid == 0) {
      consumer(buffer);
    } else {
      producer(buffer);
    }
  }

  return 0;
}

int get_value(buffer_type &buffer) {
  while (true) {
#ifdef DEBUG
    ++buffer._num_acc;
    if (buffer._num_acc % 10000000 == 0) {
      std::cout << "get_value, n=" << buffer.n << std::endl;
    }
#endif
    if (buffer.n > 0) {
      buffer.n--;
      assert(0 <= buffer.n && buffer.n < buffer.size);
      return buffer.value[buffer.n];
    }
  }
}

void solve_problem(int) {
  // Do something.
  std::uniform_int_distribution ud(1, 5);
  std::this_thread::sleep_for(std::chrono::milliseconds{ud(rand_gen)});
}

void consumer(buffer_type &buffer) {
  while (true) {
    // Get value from buffer.
    int value = get_value(buffer);
    // Do some useful work with the value.
    solve_problem(value);
  }
}

void put_value(buffer_type &buffer, int val) {
  while (true) {
#ifdef DEBUG
    ++buffer._num_acc;
    if (buffer._num_acc % 10000000 == 0) {
      std::cout << "put_value, n=" << buffer.n << std::endl;
    }
#endif
    if (buffer.n < buffer.size) {
      buffer.value[buffer.n] = val;
      buffer.n++;
      assert(0 < buffer.n && buffer.n <= buffer.size);
      return;
    }
  }
}

int compute_problem() {
  std::uniform_int_distribution ud(10, 50);
  static int value{0};
  std::this_thread::sleep_for(std::chrono::milliseconds{ud(rand_gen)});
  return value++;
}

void producer(buffer_type &buffer) {
  while (true) {
    // Compute a value
    int value = compute_problem();
    // Insert in buffer
    put_value(buffer, value);
  }
}
