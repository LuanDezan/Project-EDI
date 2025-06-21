        #include <stdio.h>
        #include <stdlib.h>
        #include <string.h>
        #include <time.h>
        #include<stdbool.h>
        #include"../constantes/constantes.h"
        #include <locale.h>
        #define MAX 40
        #define MAXHASH 20
        #define NUM_BAIRROS 4
        #define NUM_SERVICOS 3
        #include"../constantes/simulacao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include<stdbool.h>
#include"../constantes/constantes.h"
#include <locale.h>

#define MAX 40
#define MAXHASH 20
#define NUM_BAIRROS 4
#define NUM_SERVICOS 3

#include"../constantes/simulacao.h"

int main() {
    setlocale(LC_ALL, "pt_BR.UTF-8");
    srand(time(NULL));

    Cidade cidade;
    Cidadao *tabelaHashCidadao[MAXHASH] = {NULL};
    Bairro tabelaHashBairro[MAXHASH];
    for (int i = 0; i < MAXHASH; i++) {
        tabelaHashBairro[i].id = -1;
        tabelaHashBairro[i].prox = NULL;
    }

    const char *NOMES_BAIRROS[NUM_BAIRROS] = {"Centro", "Norte", "Sul", "Leste"};
    for (int i = 0; i < NUM_BAIRROS; i++) {
        cadastrarBairro(i+1, NOMES_BAIRROS[i], tabelaHashBairro);
    }

    inicializarCidade(&cidade, tabelaHashBairro, MAXHASH);
    criarServicosParaBairros(&cidade, tabelaHashBairro, MAXHASH);

    DescritorFila *filas[NUM_BAIRROS][NUM_SERVICOS];
    for (int b = 0; b < NUM_BAIRROS; b++) {
        for (int s = 0; s < NUM_SERVICOS; s++) {
            filas[b][s] = (DescritorFila*)malloc(sizeof(DescritorFila));
            inicializarDescritorFila(filas[b][s]);
        }
    }

    printf("\n\nBairros cadastrados:\n");
    for (int i = 1; i <= NUM_BAIRROS; i++) {
        Bairro *b = buscar_bairro_por_id(tabelaHashBairro, i, MAXHASH);
        if (b) {
            printf("\nBairro %d: %s (%.4f, %.4f)\n", b->id, b->nomeDoBairro, b->latitude, b->longitude);
        } else {
            printf("\nBairro %d nao encontrado!\n", i);
        }
    }

    historicoOcorrencias historico;
    inicializar_historico(&historico);

    gerarCidadaosAleatorios(tabelaHashCidadao, tabelaHashBairro, MAXHASH);
    int totalOcorrencias = gerarOcorrenciasIniciais(tabelaHashBairro,tabelaHashCidadao,filas,MAXHASH);
    exibirEstadoInicialFilas(filas, tabelaHashBairro);

    tempoGlobal = 720;
    while (tempoGlobal < 1080) {
        processarCicloAtendimento(filas, tabelaHashBairro, tabelaHashCidadao, &historico);
    }

    printf("\n\n================ HISTORICO DE OCORRENCIAS ATENDIDAS ================\n");
    imprimir_historico(&historico);
    destruir_historico(&historico);

    return 0;
}
