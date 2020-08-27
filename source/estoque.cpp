#include <iostream>
#include <string>
#include "estoque.h"

void operator<<(Item& recebe, const Item& fornece)
{
	strcpy_s(recebe.nome, sizeof(fornece.nome), fornece.nome);
	recebe.quantidade = fornece.quantidade;
	recebe.preco = fornece.preco;
}

void operator+=(Item& recebe, const Item& fornece)
{
	recebe.quantidade += fornece.quantidade;
	recebe.preco = fornece.preco;
}

void operator<<(Encomenda& recebe, const Encomenda& fornece)
{
	recebe.solicitado << fornece.solicitado;
	recebe.erro = fornece.erro;
	recebe.idEstoque = fornece.idEstoque;
}

ostream& operator<<(ostream& os, const Item& produto)
{
	os << produto.nome;
	if (strlen(produto.nome) < 8)
		os << "\t";
	os << "\t - R$ " << produto.preco << "\t - " << produto.quantidade << " kg" << endl;

	return os;
}

void operator<<(Item* destino, const Estoque origem)
{
	for (unsigned i = 0; i < origem.nItens; i++)
		destino[i] << (*origem.produto)[i];
}

void trocarExtensao(char* arquivoDestino, const char* arquivoOrigem, const char* novaExtensao)
{
	unsigned i = 0;
	while (i < strlen(arquivoOrigem) - 3)
		arquivoDestino[i] = arquivoOrigem[i++];
	strcpy_s(&(arquivoDestino[i]), strlen(novaExtensao) + 1, novaExtensao);
}

void resetar(Encomenda& enc)
{
	enc.erro = sucesso;
	enc.idEstoque = inexistente;
}

bool empurrar(Estoque& estoque, const Item& item)
{
	bool result = false;
	if (estoque.capacidade > estoque.nItens)
	{
		(*estoque.produto)[estoque.nItens++] << item;
		result = true;
	}

	return result;
}

bool expandir(Estoque& estoque, Item** auxiliar)
{
	bool result = false;
	if (estoque.nItens == estoque.capacidade)
	{
		// AUMENTA a capacidade
		estoque.capacidade += unsigned(pow(2, estoque.expansoes));

		// Cria NOVO Item auxiliar
		auxiliar[++estoque.expansoes % 2] = new Item[estoque.capacidade];

		// COPIA o estoque antigo para o novo (auxiliar)
		auxiliar[estoque.expansoes % 2] << estoque;

		// DELETA o auxiliar anterior
		delete[] * estoque.produto;

		// ATRIBUI o novo auxiliar ao estoque
		estoque.produto = &auxiliar[estoque.expansoes % 2];

		result = true;
	}
	return result;
}

bool adicionar(Estoque& estoque, Item** auxiliar, Item& novo)
{
	bool result = false;

	// Se PRECISAR expandir
	if (estoque.nItens >= estoque.capacidade)
		expandir(estoque, auxiliar);

	empurrar(estoque, novo);

	return result;
}

void operator+=(Encomenda& destino, const Encomenda& origem)
{
	destino.solicitado.quantidade += origem.solicitado.quantidade;
	if (origem.idEstoque != inexistente) // Só mexe no preço se o item constar no estoque
		destino.solicitado.preco += origem.solicitado.preco;
}
