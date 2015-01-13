#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "hash.h"
#include "cronometro.h"



#define hashUtilizada 2
//1 = funcao dada
//2 = funcao criada
#define tratamentoUtilizado 2
//1 = busca linear
//2 = encadeamento
int colisoes = 0;
int tamanhoDaTabela = 100;
int numeroElementos = 0;
float cargaMaxima = 0.6;
//***************************************************************
//************** INICIALIZAÇÃO DA TABELA ************************
//***************************************************************

void iniciarTabela(hashTable *tabelaHash, int tamanho){
	hashTable novo;
	novo.chave = "\0";
	novo.dado = 0;
	novo.prox = NULL;
	
	int i;
	
	for(i = 0; i < tamanho; i++) {
		tabelaHash[i] = novo;
	}

}


//***************************************************************
//************** TRATAMENTO DE COLISAO **************************
//***************************************************************

// Busca Linear
void trataColisao1(hashTable *tabelaHash, hashTable novo ,int posicao){
	hashTable *n;
	//Ponteiro para a tabela
	n = &tabelaHash[posicao];
	//Enquanto dado n for 0
	while(n->dado != 0) {
		posicao += 1;
		
		if(posicao == tamanhoDaTabela) posicao=0;
		//referencia novamente
		n = &tabelaHash[posicao];
	}
	tabelaHash[posicao] = novo;
}

// Encadeado
void trataColisao2(hashTable *tabelaHash, hashTable novo ,int posicao){
	hashTable *aux, *aux2;
	// É inserida na primeira posição da lista encadeada, pois não tem necessidade de percorrer ela toda
	aux = &tabelaHash[posicao];
	aux2 = aux->prox;
	aux->prox = (hashTable *) malloc(sizeof(hashTable));
	
	aux->prox->dado = novo.dado;
	aux->prox->chave = novo.chave;
	aux->prox->prox = aux2;
}


//***************************************************************
//************** FUNCAO HASH NÚMERO 2 ***************************
//***************************************************************
// Hash encontrada em: http://burtleburtle.net/bob/hash/doobs.html
// 

int hash2(char *string){
	extern int tamanhoDaTabela;
	int len = strlen(string);
	unsigned long hash, i;
	for (hash=0, i=0; i<len; ++i)
		hash = (hash<<4)^(hash>>28)^string[i];
	return (hash % tamanhoDaTabela);

}
 



//***************************************************************
//************** FUNCAO HASH NÚMERO 1 ***************************
//***************************************************************

int hash1(char *chav){
	extern int tamanhoDaTabela;
	int h = 0;
	int n,i;
	n = strlen(chav);
	for(i=0;i<n;i++){
		h += chav[i]*31*n;
	}
	return h%tamanhoDaTabela;
}

//***************************************************************
//************** PREENCHIMENTO DA TABELA ************************
//***************************************************************


void inserirNaTabela(hashTable *tabelaHash, hashTable novo){	
	extern int numeroElementos;
	int pos;
	if(hashUtilizada==1)	pos = hash1(novo.chave);
	else pos = hash2(novo.chave);
		if(tabelaHash[pos].dado != 0) { // Se a posição estiver ocupada
			switch(tratamentoUtilizado) {
				case 1: // Linear
					trataColisao1(tabelaHash, novo, pos);
					numeroElementos++;
					break;
				case 2: // Encadeado
					trataColisao2(tabelaHash, novo, pos);
					
					break;
			}

		}else{
			tabelaHash[pos] = novo;
			numeroElementos++;
		}

}




//***************************************************************
//************** IMPRESSÃO DE TODA TABELA ***********************
//***************************************************************
//Usada somente para testes
void imprimiTabela(hashTable *tabelaHash){
	int i, cont=0;
	FILE *saida = fopen("saida2.txt", "w+");
	extern int tamanhoDaTabela;
	
	for(i = 0; i < tamanhoDaTabela; i++){
		if(tabelaHash[i].dado != 0){
		fprintf(saida,"%s %d -> posicao %d\n", tabelaHash[i].chave, tabelaHash[i].dado, i);
		
		}else if(tratamentoUtilizado == 2) {
			hashTable *aux = &tabelaHash[i];
			while(aux->prox != NULL ) {
				fprintf(saida,"%s %d -> posicao %d\n", aux->chave, tabelaHash->dado, i);
				aux = aux->prox;
			}
		}
		if(tabelaHash[i].dado == 0)
			cont++;
	}
	printf("Posicoes vazias: %i", cont);
	printf("\n");
	
	
}

//***************************************************************
//************************** REHASH *****************************
//***************************************************************

hashTable * reHash(hashTable vet[]){
	extern int tamanhoDaTabela;
	extern int numeroElementos;
	numeroElementos = 0;
	int maximo = tamanhoDaTabela;
	tamanhoDaTabela = tamanhoDaTabela << 1;
	
	int i;
	hashTable *tabelaAtualizada, novo;
	//Nodo auxiliar
	hashTable *n;
	
	novo.dado = 0;
	novo.chave = "\0";
	novo.prox = NULL;
	//Maloca nova tabela
	tabelaAtualizada = (hashTable *) malloc(tamanhoDaTabela * sizeof(hashTable));
	//Inicia a tabela com 0
	iniciarTabela(tabelaAtualizada, tamanhoDaTabela);
	//Reinsere todas as posições no novo vetor.
	for(i = 0; i < maximo; i++ ) {
		n = &vet[i];
		
		novo.dado = n->dado;
		novo.chave = n->chave;
		novo.prox = NULL;
		
		if(novo.dado != 0){
			inserirNaTabela(tabelaAtualizada,novo);
		}
		//Caso o tratamento utilizado seja o encadeado, é necessário
		if(tratamentoUtilizado==2){
			while(n->prox!=NULL){
				n = n->prox;
				novo.dado = n->dado;
				novo.chave = n->chave;
				novo.prox = NULL;
				if(novo.dado!=0){
				 inserirNaTabela(tabelaAtualizada,novo);
					
				 }
				
			}
		}
	}
	
	
	
	free(vet);
	return tabelaAtualizada;
	
}

//***************************************************************
//***************** ENCONTRAR CHAVE *****************************
//***************************************************************

int encontrarChave(char *chave, hashTable *tabela){
	extern int tamanhoDaTabela;
	int posicao;
	if(hashUtilizada == 1) posicao = hash1(chave);
	if(hashUtilizada == 2) posicao = hash2(chave);
	hashTable *aux = &tabela[posicao];
	
	// Se o tratamento é encadeado, ele percorre toda a lista daquela posição...
	if(tratamentoUtilizado == 2) {
		if(strcmp(aux->chave, chave)==0) return aux->dado;
		while(aux->prox != NULL) {
			aux = aux->prox;
			if(strcmp(aux->chave, chave)==0) return aux->dado;
			
		}
		return -1;
		
	}else{
		//Senão faz uma busca linear para encontrar o dado, parando em i-1, se não encontrar retorna -1
		int i = posicao;
		if(strcmp(aux->chave,chave)==0) return aux->dado;
		while(i != posicao-1) {
			i++;
			if(i >= tamanhoDaTabela) i = 0;
			aux = &tabela[i];
			if(strcmp(aux->chave, chave)==0) return aux->dado;
		}
		return -1;
		
	}
	return 0;
}


int main(int argc, char **argv){
	extern int tamanhoDaTabela;
	char learq[150];
	char *parteinteira, *chave, *numero;
	int encontradas = 0, naoEncontradas = 0, ninsercoes=0;
	float fatorDeCarga;
	
	extern float cargaMaxima;
	hashTable *tabelaHash;
	tabelaHash = (hashTable *) calloc(tamanhoDaTabela,  sizeof(hashTable) );
 	iniciarTabela(tabelaHash, tamanhoDaTabela);

	//Criação do arquivo xlxs para as simulações!!
	//Descomentar essas linhas caso queira a tabela de inserções (excel).
/*
	int linha = 2;
	char *arquivo;
	switch(tratamentoUtilizado) {
		case 1:
			if(hashUtilizada == 1) arquivo = "hash1linear.xlsx";
			else arquivo = "hash2linear.xlsx";
			break;
		case 2:
			if(hashUtilizada == 1) arquivo = "hash1encadeada.xlsx";
			else arquivo = "hash2encadeada.xlsx";
			break;
		default:
			printf("Você colocou um tratamento que não existe!!");
			exit(1);
	}
	//printf("Criando arquivo... feito!\n");
	FILE *colisao = fopen(arquivo, "w+");
	fprintf(colisao, "Encadeada;Tempo\n");
	inicia_cronometro();*/
	while(fgets(learq,150,stdin) && learq[0]!='\n'){
		parteinteira = strtok(learq, "\n");
		chave = strtok(parteinteira, " ");
		numero = NULL;
		numero = strtok(NULL, " ");
		
		if(numero!=NULL){
			hashTable novo;
			novo.dado = atoi(numero);
			
			novo.chave = (char*) malloc (150*sizeof(char));
			novo.prox = NULL;
			strcpy(novo.chave, chave);

			inserirNaTabela(tabelaHash,novo);
			ninsercoes++;
			//Descomentar caso queira a tabela com o tempo de inserção
		/*	if(ninsercoes % 20000 == 0) {
				
				indicaTempo(colisao, ninsercoes, linha);
				linha++;
			}
			*/
			//CALCULA FATOR DE CARGA
			fatorDeCarga = (float) numeroElementos/tamanhoDaTabela;
			
			//Rehash se fatorDeCarga for >= cargaMaxima%;
			if(fatorDeCarga >= cargaMaxima) {
				numeroElementos=0;
				tabelaHash = reHash(tabelaHash);
			}
		//Busca
		}else{
			hashTable novo;
			
			novo.chave = (char*) malloc(150*sizeof(char));
			strcpy(novo.chave, chave);
			int encontrou = -1;
			 encontrou = encontrarChave(novo.chave, tabelaHash);
			(encontrou != -1) ? encontradas++ : naoEncontradas++;
			//Se encontrou ele printa
			if(encontrou != -1) 
				printf("%d\n", encontrou);
		}
	}
	free(tabelaHash);
	//fclose(colisao);
	//printf("n insercoes %d", ninsercoes);
	//printf("%d", tratamentoUtilizado);
	//printf("Encontradas: %d\n", encontradas);
	//printf("Nao encontradas: %d\n", naoEncontradas);
	//printf("Tamanho do vetor: %i\n", tamanhoDaTabela);
	//printf("Numero de insercoes: %i\n", numeroElementos);
	
	return 0;
	
	
}
