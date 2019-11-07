#include <array>
#include <iostream>
#include <limits>
#include <mpi.h>
#include <string>
#include <vector>

int constexpr MAXLEN = 64;

struct Pessoa {
  long id;
  std::array<char, MAXLEN> nome;
};

int main(int argc, char *argv[]) {
  int quantos, rank;

  MPI_Init(&argc, &argv);

  MPI_Comm_size(MPI_COMM_WORLD, &quantos);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  std::vector<Pessoa> pessoas;

  // Inicializacao dos dados.
  if (rank == 0) {
    pessoas.resize(quantos);
    std::cout << "Digite dados de " << quantos << " pessoas:\n";
    for (int i = 0; i < quantos; ++i) {
      std::cout << "Nome:       ";
      std::cin.get(&pessoas[i].nome[0], MAXLEN);
      std::cout << "Identidade: ";
      std::cin >> pessoas[i].id;
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
  }

  std::array<MPI_Datatype, 2> tipoPessoa{MPI_LONG, MPI_CHAR};
  std::array<int, 2> compPessoa{1, MAXLEN};
  std::array<MPI_Aint, 2> deslPessoa;
  MPI_Datatype tPessoa;

  Pessoa umapessoa;

  // Construcao de um tipo para a transmissao.
  MPI_Aint start_address;
  MPI_Get_address(&umapessoa, &start_address);
  MPI_Get_address(&umapessoa.id, &deslPessoa[0]);
  MPI_Get_address(&umapessoa.nome[0], &deslPessoa[1]);
  deslPessoa[1] -= start_address;
  deslPessoa[0] -= start_address;

  MPI_Type_create_struct(2, &compPessoa[0], &deslPessoa[0], &tipoPessoa[0],
                         &tPessoa);

  MPI_Type_commit(&tPessoa);

  // Distribuicao dos dados para os processos
  MPI_Scatter(&pessoas[0], 1, tPessoa, &umapessoa, 1, tPessoa, 0,
              MPI_COMM_WORLD);

  std::cout << "Processo " << rank << " recebeu: " << &umapessoa.nome[0] << " "
            << umapessoa.id << std::endl;

  MPI_Finalize();
  return 0;
}
