#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <numeric>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  int quantos, rank;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &quantos);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Verificacao de parametros para o programa.
  if (argc != 3) {
    // Nao foram especificados os parametros corretamente.
    if (rank == 0) {
      // Rank 0 escreve a mensagem de erro.
      std::cerr << "Passe nome de arquivo e numero de elementos "
                   "como parametro!\n";
    }
    MPI_Finalize(); // Todos terminam
    exit(1);
  }

  // Parametro 2 e o numero de elementos.
  int N = std::stoi(argv[2]);

  std::vector<float> originais;

  // Leitura dos dados (processo 0)
  if (rank == 0) {
    originais.resize(N);
    // Parametro 1 e o nome do arquivo.
    std::ifstream arqDados(argv[1]);
    if (arqDados.fail()) {
      // Problemas no arquivo? Aborta.
      MPI_Abort(MPI_COMM_WORLD, 2);
    }
    for (int i = 0; i < N; ++i) {
      arqDados >> originais[i];
    }
  }

  // Calculo do numero de elementos por processo.
  // Calculo realizado por todos os processos.
  std::vector<int> conts(quantos), desls(quantos);

  int q = N / quantos;
  int r = N % quantos;
  std::fill_n(begin(conts), r, q + 1);

  std::fill(begin(conts) + r, end(conts), q);
  desls[0] = 0;

  std::partial_sum(begin(conts), end(conts) - 1, begin(desls) + 1);

  // Vetor para guardar dados do rank
  std::vector<float> dados(conts[rank]);

  // Distribuicao dos elementos para os processos
  MPI_Scatterv(&originais[0], &conts[0], &desls[0], MPI_FLOAT, &dados[0],
               conts[rank], MPI_FLOAT, 0, MPI_COMM_WORLD);

  // Somatoria local dos elementos recebidos
  float somalocal = std::accumulate(begin(dados), end(dados), 0.0f);

  // Calculando agora a somatoria total
  float somaglobal;

  // Reducao final para encontrar somatoria global
  MPI_Reduce(&somalocal, &somaglobal, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

  std::cout << "Soma local no processo " << rank << " = " << somalocal
            << std::endl;

  MPI_Barrier(MPI_COMM_WORLD);

  // Impressao do resultado (presente no processo 0)
  if (rank == 0) {
    std::cout << "Soma total: " << somaglobal << std::endl;
  }

  MPI_Finalize();
  return 0;
}
