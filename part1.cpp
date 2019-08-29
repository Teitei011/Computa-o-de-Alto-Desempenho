#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace chrono = std::chrono;

std::pair<int, int> read_arguments(int argc, char *argv[]);

// Function for random number generation
 std::default_random_engine generator;
 std::uniform_real_distribution<double> distribution(0.0,1.0);

void show_matrix(double A[][100], int N);

int main(int argc, char *argv[]) {
  auto [N, M] = read_arguments(argc, argv);


   // Creating the matrix
   double A[N][N];
   double B[N][N];


   // The time monitor
   double elapsed = 0;
   chrono::high_resolution_clock::time_point t1, t2;


   // Adding Random Value to the matrix
   for(int i = 0; i < N; i++){
     for(int j = 0; j < N; j++){
       A[i][j] = distribution(generator);
       B[i][j] = distribution(generator);
     }
   }


   // Fazendo a Multiplicacao em si
    t1 = chrono::high_resolution_clock::now();
       for (int j = 0; j < N; j++){
        for (int i = 0; i < N; i++){
          soma = 0;
           for (int k = 0; k < N; k++){
               soma += A[i][k] * B[k, j];
           }
        }
       }
       // soma += A[i, k] * B[k, j];


       t2 = chrono::high_resolution_clock::now();

       auto dt = chrono::duration_cast<chrono::microseconds>(t2 - t1);
       elapsed += dt.count();

     std::cout << N << " " << elapsed / M / 1e6 << std::endl;
  return 0;
}

void show_matrix(double A[][2], int N){
  for(int i = 0; i < N; i++){
    for(int j = 0; j < N; j++){
      std::cout << A[i][j] << ' ';
    }
    std::cout << '\n';
   }
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
