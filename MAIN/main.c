#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



//TODO colocar o vetor de nome de bairros no arquivo de constantes
#define MAX 40
#define HASH_SIZE 20
#define MAXHASH 20
#define HASH 20


#include"..\constantes\constantes.h"
#include"..\Cidadao\Cidadao.c"
#include"..\SAMU\Samu.c"
#include"..\bairro\Bairro.c"
#include"..\MAIN\main.c"
#include"..\hospital\HospitalComHash.c"
#include"..\policia\PoliciaComHash.c"
#include"..\constantes\constantes.h"


int main() {
    srand(time(NULL));

    Bairro tabelaHashBairro[MAXHASH];
    inicializarTabelaBairro(tabelaHashBairro);

    for (int i = 0; i < 10; i++) {
        int id = i * 100;
        char nome[30];
        sprintf(nome, "Bairro%d", i);
        cadastrarBairro(id, nome, tabelaHashBairro);
    }

    for (int i = 0; i < 5; i++) {
        gerarOcorrencia(tabelaHashBairro);
    }

    mostrarFila();
    return 0;
}

