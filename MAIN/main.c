#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 40
#define MAXHASH 20
#define NUM_BAIRROS 4
#define NUM_SERVICOS 4



// estruturas

typedef struct Bairro
{
    int id;
    char nomeDoBairro[30];
    struct Bairro *prox;
} Bairro;

typedef struct Ocorrencia
{
    int id;
    char horarioChegada[6];
    char horarioAtendimento[6];
    int gravidade;
    int tipo;
    Bairro *bairro;
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
    srand(time(NULL));

    Bairro tabelaHashBairro[MAXHASH];
    for (int i = 0; i < MAXHASH; i++) tabelaHashBairro[i].prox = NULL;

    const char *NOMES_BAIRRO[NUM_BAIRROS] = {"Centro", "Norte", "Sul", "Leste"};

    for (int i = 0; i < NUM_BAIRROS; i++)
    {
        cadastrarBairro(i + 1, NOMES_BAIRRO[i], tabelaHashBairro);
    }



    DescritorFila *filas[NUM_SERVICOS];
    for (int i = 0; i < NUM_SERVICOS; i++)
    {
        filas[i] = malloc(sizeof(DescritorFila));
        inicializarDescritorFila(filas[i]);
    }



    for (int i = 0; i < 16; i++)
    {
        Ocorrencia *nova = criarOcorrenciaAleatoria(tabelaHashBairro);

        if (nova)
        {
            enviarOcorrencia(filas, nova);
        }

    }

    const char *TIPOS_SERVICO[NUM_SERVICOS] = {"HOSPITAL", "POLICIA", "BOMBEIRO", "SAMU"};

    printf("\n============== ESTADO INICIAL DAS FILAS ==============\n");
    for (int i = 0; i < NUM_SERVICOS; i++)
    {
        mostrarFilaAtual(filas[i], TIPOS_SERVICO[i]);
    }


    int tempoRestante[NUM_SERVICOS] = {0}; // inicia tudo zerado
    Ocorrencia *emAtendimento[NUM_SERVICOS] = {NULL}; // controle externo da funcao

    processarFilas(filas, tempoRestante, tabelaHashBairro, emAtendimento);

        printf("\n\n============== ESTADO FINAL DAS FILAS ==============\n");
        for (int i = 0; i < NUM_SERVICOS; i++)
        {
            mostrarFilaAtual(filas[i], TIPOS_SERVICO[i]);

        }

        return 0;
}




