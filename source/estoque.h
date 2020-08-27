#include <iostream>
using namespace std;

enum Erro { sucesso, insuficiente, inexistente = -1 }; 

struct Item
{
	char nome[30];
	float preco;
	int quantidade;
};

struct Estoque
{
	Item** produto = 0; // Ponteiro para vetor de itens
	unsigned nItens = 0;
	unsigned capacidade = 0;
	unsigned expansoes = 0;
};

struct Encomenda
{
	Item solicitado{"", 0.0f, 0};
	int erro = sucesso;
	int idEstoque = inexistente;
	/*

	A qual Item do estoque a encomenda se refere?
	
	Considere 'idEstoque' como o ÍNDICE do vetor 'estoque' correspondente ao produto.
	Exemplo: (*estoque)[idEstoque]

	O objetivo desse Registro é evitar percorrer repetidamente o estoque em busca do nome do produto, 
	pois já teremos seu índice.
	Caso o produto não exista, o idEstoque é negativo (inexistente = -1).

	*/
};

void		operator+=		(Item& recebe,			const Item& fornece			);
void		operator+=		(Encomenda& destino,	const Encomenda& origem			);
void		operator<<		(Item& recebe,			const Item& fornece			);
void		operator<<		(Encomenda& recebe,		const Encomenda& fornece	);
void		operator<<		(Item* destino,			const Estoque origem		);
ostream&	operator<<		(ostream& os,			const Item& produto			);
void		trocarExtensao	(char* arquivoDestino,	const char* arquivoOrigem,	const char* novaExtensao);
void		resetar			(Encomenda& enc			);
bool		empurrar		(Estoque& estoque,		const Item& item			);
bool		expandir		(Estoque& estoque,		Item** auxiliar				);
bool		adicionar		(Estoque& estoque,		Item** auxiliar,			Item& novo);
