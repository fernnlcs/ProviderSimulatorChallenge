#include <iostream>
#include <cctype>
#include <fstream>
#include <string>
#include "estoque.h"
using namespace std;

inline void titulo(const char* titulo)
{ // Imprime o texto como título (com pontilhado abaixo)
	cout << titulo << endl;
	for (unsigned i = 0; i < strlen(titulo); i++)
		cout << "-";
	cout << endl;
}

inline void capitular(char* palavra)
{ // Primeira letra maiúscula e demais minúsculas
	palavra[0] = toupper(palavra[0]);
	for (unsigned i = 1; i < strlen(palavra); i++)
		palavra[i] = tolower(palavra[i]);
}


int main()
{
	// Definindo os padrões de acentuação e casas decimais
	system("chcp 1252 > nul");
	cout.setf(ios_base::fixed, ios_base::floatfield);
	cout.precision(2);

	// Constante: nome do arquivo de estoque
	const char arqEstoque[] = "estoque.bin";

	Estoque estoque;

	// Abrindo arquivo inicial de Estoque
	ifstream estoqueInicial;
	estoqueInicial.open(arqEstoque, ios_base::in | ios_base::binary);

	if (estoqueInicial.is_open())
	{ // Preenche 'estoque.capacidade' com a quantidade de itens do estoque
		Item contador;
		while (estoqueInicial.read((char*)&contador, sizeof(Item))) // Percorre arquivo para CONTAR itens
			estoque.capacidade++;
		
		// Reinicia a leitura do arquivo
		estoqueInicial.clear();
		estoqueInicial.seekg(0, ios::beg);
	}

	// Cria vetor de 2 variáveis auxiliares (para alternar durante as expansões)
	Item* auxiliar[2];

	// (estoque.expansoes % 2) sempre será usado para saber QUAL posição de 'auxiliar[?]' está preenchida no momento
	// Primeiramente, essa posição se torna um vetor dinâmico
	auxiliar[estoque.expansoes % 2] = new Item[estoque.capacidade];

	// O estoque OFICIAL recebe o endereço do auxiliar definido anteriormente
	estoque.produto = &auxiliar[estoque.expansoes % 2];

	// Grava os itens da lista 'estoqueInicial' no vetor 'estoque'
	// Obs: mesmo que o arquivo indicado não exista, a linha abaixo NÃO gerará nenhum erro
	for (int i = 0; estoqueInicial.read((char*) & (*estoque.produto)[i], sizeof(Item)); i++)
		estoque.nItens++;

	estoqueInicial.close();


	// Menu Principal
	char opcao;
	do
	{
		cout << "Sistema de Controle"	<< endl
			<< "==================="	<< endl
			<< "(P)edir"				<< endl
			<< "(A)dicionar"			<< endl
			<< "(E)xcluir"				<< endl
			<< "(L)istar"				<< endl
			<< "(S)air"					<< endl
			<< "==================="	<< endl
			<< "Opção: [_]\b\b";
		cin >> opcao;
		opcao = toupper(opcao); // Converte minúsculas em MAIÚSCULAS
		cout << endl;

		// Direcionando para a opção selecionada
		if (opcao == 'P')
		{ // PEDIR
			titulo("Pedir");
			cout << "Arquivo: ";

			char arqPedido[40];
			cin >> arqPedido;
			ifstream pedido;
			pedido.open(arqPedido, ios_base::in);

			if (pedido.is_open())
			{ // Arquivo existente e aberto com sucesso
				char cliente[30];
				pedido.getline(cliente, 30);

				// Ignora linha pontilhada
				pedido.get();		// Primeira ocorrência de '\n'
				while (pedido.get() != '\n');

				// Item descartável (para percorrer o Arquivo .txt)
				Encomenda leitura;

				// Carrinho de compras (funcionamento semelhante ao estoque)
				int nPedidos = 0;
				Encomenda* conjunto[2];
				Encomenda** venda = &conjunto[nPedidos % 2];
				(*venda) = new Encomenda[nPedidos];

				bool falhou = false;
				while (pedido >> leitura.solicitado.nome, pedido >> leitura.solicitado.quantidade)
				{
					// Reseta dados de 'leitura', exceto 'leitura.solicitado', que acabou de ser sobrescrito
					resetar(leitura);

					// Padroniza o nome do produto
					capitular(leitura.solicitado.nome);

					// Item consta no estoque? (Padrão: Não consta)
					for (unsigned i = 0; (i < estoque.nItens) && (leitura.idEstoque == inexistente); i++)
						if (!strcmp(leitura.solicitado.nome, (*estoque.produto)[i].nome))
						{ // Sim, consta.
							// Associa ao produto do estoque
							leitura.idEstoque = i;

							// Ajusta o preço do item pedido
							leitura.solicitado.preco = (*estoque.produto)[i].preco * leitura.solicitado.quantidade;
						}

					if (leitura.idEstoque == inexistente)
					{ // Não consta.
						leitura.erro = inexistente;
						falhou = true;
					}

					// Verifica se o item já foi pedido anteriormente
					bool repetido = false;

					for (int i = 0; i < nPedidos && !repetido; i++)
						if (!strcmp(leitura.solicitado.nome, (*venda)[i].solicitado.nome))
						{ // Item já apareceu na lista antes. Apenas atualiza os dados (não expande o vetor).
							repetido = true;
							(*venda)[i] += leitura;
						}

					if (!repetido)
					{ // Não repetido, insere novo item no vetor
						// Incrementa o número de pedidos
						nPedidos++;

						// Cria o vetor maior. OBS: [nPedidos % 2] é o índice que NÃO está em uso
						conjunto[nPedidos % 2] = new Encomenda[nPedidos];

						// Copia o vetor menor para o maior
						for (int i = 0; i < (nPedidos - 1); i++)
							conjunto[nPedidos % 2][i] << (*venda)[i];

						// DELETA o vetor menor (dá NEW em um vetor e DELETE no outro)
						delete[] * venda;

						// Atribui o vetor maior a 'venda'
						venda = &conjunto[nPedidos % 2];

						// Adiciona o novo item pedido ao final do vetor
						(*venda)[nPedidos - 1] << leitura;
					}

					// Se o item consta no estoque...
					if ((*venda)[nPedidos - 1].idEstoque != inexistente)
						if ((*venda)[nPedidos - 1].solicitado.quantidade > (*estoque.produto)[(*venda)[nPedidos - 1].idEstoque].quantidade)
						{ // Foi solicitado MAIS que o disponível em estoque...
							(*venda)[nPedidos - 1].erro = insuficiente;
							falhou = true;
						}
				}

				if (falhou)
				{ // Houve qualquer falha no pedido...
					cout << "Pedido falhou!" << endl;
					for (int i = 0; i < nPedidos; i++)
						if ((*venda)[i].erro == insuficiente)
							cout << (*venda)[i].solicitado.nome << ": "
							<< "Solicitado = " << (*venda)[i].solicitado.quantidade << " kg / "
							<< "Em estoque = " << (*estoque.produto)[(*venda)[i].idEstoque].quantidade << " kg" << endl;
						else
							if ((*venda)[i].erro == inexistente)
								cout << (*venda)[i].solicitado.nome << ": "
								<< "Solicitado = " << (*venda)[i].solicitado.quantidade << " kg / "
								<< "Item não consta no estoque" << endl;
				}
				else
				{
					cout << "Sucesso! Gerando NFC..." << endl;
					float total = 0.0f;
					char arqNFC[40];
					trocarExtensao(arqNFC, arqPedido, "nfc");

					// Criando e configurando objeto nfc
					ofstream nfc;
					nfc.open(arqNFC, ios_base::out);
					nfc.setf(ios_base::fixed, ios_base::floatfield);
					nfc.precision(2);
					nfc << cliente << endl
						<< "--------------------------------------" << endl;

					for (int i = 0; i < nPedidos; i++)
					{
						// Desconta a quantidade no estoque
						(*estoque.produto)[(*venda)[i].idEstoque].quantidade -= (*venda)[i].solicitado.quantidade;

						// Grava as informações no arquivo .nfc
						nfc << (*venda)[i].solicitado.nome << ": "
							<< (*venda)[i].solicitado.quantidade << " kg a R$ "
							<< (*estoque.produto)[(*venda)[i].idEstoque].preco << "/kg = R$ "
							<< (*venda)[i].solicitado.preco << endl;

						// Acumula preço
						total += (*venda)[i].solicitado.preco;
					}
					nfc << "--------------------------------------" << endl;
					nfc << "Compra = R$ " << total << endl;
					if (total >= 1000)
					{ // Analisa desconto
						float desconto = 0.1f * total;
						total -= desconto;
						nfc << "Desconto = R$ " << desconto << endl;
					}
					nfc << "Total = R$ " << total << endl;

					nfc.close();
				}

				delete[] * venda;
			}
			else // Falha na abertura do arquivo (inexistente)
				cout << "O arquivo '" << arqPedido << "' não pôde ser aberto!" << endl;

			pedido.close();
		}
		else
			if (opcao == 'A')
			{ // ADICIONAR
				titulo("Adicionar");

				Item novo;
				cout << "Produto: ";
				cin.get();
				cin.getline(novo.nome, 20);
				capitular(novo.nome);		// Corrige imperfeições na digitação do nome do produto
				cout << "Preço: ";
				cin >> novo.preco;
				cout << "Quantidade: ";
				cin >> novo.quantidade;

				// Inclusão ou Atualização? - Procura por itens de mesmo nome no estoque
				bool inedito = true;
				for (unsigned i = 0; i < estoque.nItens && inedito; i++)
					if (!strcmp((*estoque.produto)[i].nome, novo.nome))
					{ // Atualização, apenas.
						inedito = false;
						(*estoque.produto)[i] += novo; // Atualiza o Item (operator+= para Itens, em 'estoque.cpp')
					}

				if (inedito) // Inclusão - O produto é inédito.
					adicionar(estoque, auxiliar, novo);
			}
			else
				if (opcao == 'E')
				{ // EXCLUIR
					titulo("Excluir");

					// Exibindo lista numerada
					for (unsigned i = 0; i < estoque.nItens; i++)
						cout << i + 1 << ") " << (*estoque.produto)[i].nome << endl;

					// Confirmações do usuário
					cout << endl << "Número do produto: ";
					unsigned eliminado;
					cin >> eliminado;

					if (eliminado > 0 && eliminado <= estoque.nItens)
					{
						cout << "Deseja excluir '" << (*estoque.produto)[eliminado - 1].nome << "'? (S/N) ";
						char confirmacao;
						cin >> confirmacao;
						confirmacao = toupper(confirmacao);

						if (confirmacao == 'S') // Transferindo ÚLTIMO item para o local do item eliminado
						{
							(*estoque.produto)[eliminado - 1] << (*estoque.produto)[--estoque.nItens];
							cout << "Produto excluído." << endl;
						}
						else
							cout << "Exclusão cancelada." << endl;
					}
					else
					{
						cin.clear();
						cin.get();
						cout << "Esse não é um número válido." << endl;
					}
				}
				else
					if (opcao == 'L')
					{ // LISTAR
						titulo("Listagem");
						for (unsigned i = 0; i < estoque.nItens; i++)
							cout << (*estoque.produto)[i];
					}
					else
						if (opcao != 'S') // Não pertence às opções do Menu Principal
							cout << "Opção inválida. Tente novamente.";

		cout << endl;
		system("pause");
		system("cls");
	} while (opcao != 'S');

	// SAIR

	// Atualizando arquivo de estoque
	ofstream estoqueFinal;
	estoqueFinal.open(arqEstoque, ios_base::out | ios_base::binary);
	for (unsigned i = 0; i < estoque.nItens; i++)
		estoqueFinal.write((char*) & (*estoque.produto)[i], sizeof(Item));
	estoqueFinal.close();

	// Deletando o vetor dinâmico do estoque
	delete[] * estoque.produto;

	return 0;
}
