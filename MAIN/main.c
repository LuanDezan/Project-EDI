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


#ifdef _WIN32
#include <conio.h>
void limpar_tela() { system("cls"); }
#else
#include <stdio.h>
#define clrscr() printf("\e[1;1H\e[2J")
void limpar_tela() { clrscr(); }
#endif

void aguardar_enter() {
    printf("\n\nPressione ENTER para continuar...");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // limpa total o buffer
}


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
limpar_tela();
    DescritorFila *filas[NUM_BAIRROS][NUM_SERVICOS];
    for (int b = 0; b < NUM_BAIRROS; b++) {
        for (int s = 0; s < NUM_SERVICOS; s++) {
            filas[b][s] = (DescritorFila*)malloc(sizeof(DescritorFila));
            inicializarDescritorFila(filas[b][s]);
        }
    }
printf("\n\n┌──────────────────────────────────────────────────────────────┐\n");
printf("│                 BAIRROS CADASTRADOS                          │\n");
printf("├──────────────────────────────────────────────────────────────┤\n");
printf("│ ID  │ Nome do Bairro                      │ Coordenadas      │\n");
printf("├──────────────────────────────────────────────────────────────┤\n");

for (int i = 1; i <= NUM_BAIRROS; i++) {
    Bairro *b = buscar_bairro_por_id(tabelaHashBairro, i, MAXHASH);
    if (b) {
        printf("│ %-3d │ %-34s    │ (%-5.0f, %-5.0f) │\n",
               b->id, b->nomeDoBairro, b->latitude, b->longitude);
    } else {
        printf("│ %-3d │ %-34s    │ %-17s │\n",
               i, "NAO ENCONTRADO!", "");
    }
}

printf("└──────────────────────────────────────────────────────────────┘\n\n");

    historicoOcorrencias historico;
    inicializar_historico(&historico);

    gerarCidadaosAleatorios(tabelaHashCidadao, tabelaHashBairro, MAXHASH);
    aguardar_enter();
limpar_tela();

    int totalOcorrencias = gerarOcorrenciasIniciais(&cidade, tabelaHashBairro, tabelaHashCidadao, filas, MAXHASH);
    exibirEstadoInicialFilas(filas, tabelaHashBairro);
    aguardar_enter();
limpar_tela();


    tempoGlobal = 720;  // 12:00
    while (tempoGlobal < 1080) {  // 18:00


        processarCicloAtendimento(filas, &cidade, tabelaHashBairro, tabelaHashCidadao, &historico);
  aguardar_enter();
limpar_tela();

    }


    printf("\n\n================ HISTORICO DE OCORRENCIAS ATENDIDAS ENTRE 12h-18h ================\n");
    imprimir_historico(&historico);
    destruir_historico(&historico);

    return 0;
}
