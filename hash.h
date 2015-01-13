#ifndef HASH_H
#define HASH_H
typedef struct tabela {
	char *chave;
	int dado;
	struct tabela *prox; // colisão encadeada.
} hashTable;


void iniciarTabela(hashTable *tabelaHash, int tamanho);

#endif
