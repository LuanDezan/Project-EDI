/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>


//TODO colocar o vetor de nome de bairros no arquivo de constantes
#define MAX 40
#define HASH_SIZE 20
#define MAXHASH 20
#define HASH 20


#include"..\constantes\constantes.h"

typedef struct Bairro
{
    int id;
    char nomeDoBairro[30];
    struct Bairro *prox;

}Bairro;

typedef struct Ocorrencia
{
    int id;
    char horario[6];
    int gravidade;
    int tipo;
    struct Bairro *bairro;
    struct Ocorrencia *prox;

} Ocorrencia;

typedef struct
{
    Ocorrencia *inicio;
    Ocorrencia *fim;
    int tamanho;

} DescritorFila;



int main()
{
    setlocale(LC_ALL, "Portuguese");
    srand(time(NULL));

    Bairro tabelaHashBairro[MAXHASH];
    for (int i = 0; i < MAXHASH; i++) tabelaHashBairro[i].prox = NULL;

    for (int i = 0; i < 5; i++) {  // só 5 bairros
        int id = i * 100;
        char nome[30];
        sprintf(nome, "Bairro %d", i);
        cadastrarBairro(id, nome, tabelaHashBairro);
    }

    DescritorFila *filas[4];
    for (int i = 0; i < 4; i++) {
        filas[i] = malloc(sizeof(DescritorFila));
        inicializarDescritorFila(filas[i]);
    }

    for (int i = 0; i < 12; i++) {
        Ocorrencia *nova = criarOcorrenciaAleatoria(tabelaHashBairro);
        if (nova)
            enviarOcorrencia(filas, nova);
    }

    mostrarFilaAtual(filas[0], "hospital");
    mostrarFilaAtual(filas[1], "policia");
    mostrarFilaAtual(filas[2], "bombeiro");
    mostrarFilaAtual(filas[3], "samu");

    for (int i = 0; i < 5; i++) {
        processarFilas(filas);
    }

    return 0;
}*/
