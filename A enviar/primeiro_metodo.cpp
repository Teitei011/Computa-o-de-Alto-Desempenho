#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace chrono = std::chrono;

int N;
std::pair<int, int> read_arguments(int argc, char *argv[]);
int matrix_product(int N, double  **A, double **B, double **C);
void create_random_matrix(int N, double **A, double **B);

void show_matrix(int N, double **A);

// function for random number generation
std::random_device entropy;
std::mt19937 gen(entropy()); // Randomness generator
std::uniform_real_distribution<double> dis(0, 1);

int main(int argc, char *argv[]) {
  auto [N, M] = read_arguments(argc, argv);


   // Creating the matrix
   double **A = new double *[N];
   double *SA = new double [N*N];

   double **B = new double *[N];
   double *SB = new double [N*N];

   double **C = new double *[N];
   double *SC = new double [N*N];


   // linking pointers
   for (int i = 0; i < N; i++) {
     A[i] = &SA[i*N];
     B[i] = &SB[i*N];
     C[i] = &SC[i*N];
   }



   // The time monitor
   double elapsed = 0;
   chrono::high_resolution_clock::time_point t1, t2;



   for (int ii = 0; ii < M; ii++) {

     // Adding Random Value to the matrix

     create_random_matrix(N, A, B);


     t1 = chrono::high_resolution_clock::now();

     // Multiplicacao das matrizes
     matrix_product(N, A, B, C);

  
     t2 = chrono::high_resolution_clock::now();

     auto dt = chrono::duration_cast<chrono::microseconds>(t2 - t1);
     elapsed += dt.count();
    }

   // Show timing results
   std::cout << N << " " << elapsed / M / 1e6 << std::endl;

  return 0;
}

void create_random_matrix(int N, double **A, double **B){
  for(int i = 0; i < N; i++){
    for(int j = 0; j < N; j++){
      A[i][j] = dis(gen);
      B[i][j] = dis(gen);
    }
  }
}

void show_matrix(int N, double **A){
  for (int i = 0; i < N; i++){
    for (int j = 0; j < N; j++){
      std::cout << A[i][j] << ' ';
    }
    std::cout << '\n';
  }
}


int matrix_product(int N, double **A, double **B, double **C)
{
  double soma;
  for (int i = 0; i < N; i++){
   for (int j = 0; j < N; j++){
     soma = 0;
      for (int k = 0; k < N; k++){
          soma += A[i][k] * B[k][j];
      }
      C[i][j] = soma;
    }
  }
  return 0;
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
