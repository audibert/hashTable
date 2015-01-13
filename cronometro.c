#include "cronometro.h"

#include <time.h>
#include <stdio.h>
clock_t _tempo;

void indicaTempo(FILE *arq, int ninsercoes, int linha) {
	if(linha == 2)
		fprintf(arq, "%d;%.2f;=B%d-%d\n", ninsercoes, (double) (clock() - _tempo) / ((double) CLOCKS_PER_SEC), linha, 0);
	else
		fprintf(arq, "%d;%.2f;=B%d-B%d\n", ninsercoes, (double) (clock() - _tempo) / ((double) CLOCKS_PER_SEC), linha, linha-1);
}

void inicia_cronometro() {
    _tempo = clock();
    
}

double para_cronometro() {
	//printf("%d, %s %.9f s\n", i, texto, (double) (clock() - _tempo) / ((double) CLOCKS_PER_SEC));
	double v;
	v = (double) (clock() - _tempo) / ((double) CLOCKS_PER_SEC);
	return v;

}
