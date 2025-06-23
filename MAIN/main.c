#ifdef _WIN32
#include <windows.h>
#endif
#include <locale.h>

#include"../constantes/constantes.h"
#include <locale.h>
#define MAX 40
#define MAXHASH 20
#define NUM_BAIRROS 4
#define NUM_SERVICOS 4
#include"../constantes/simulacao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include<stdbool.h>

const char *NOME_SERVICOS[] = {"Bombeiro", "Policia", "Hospital", "Ambulância "};
const int TEMPOS_ATENDIMENTO[] = {10, 20, 10, 15};

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
    while ((c = getchar()) != '\n' && c != EOF); // limpa o buffer
}

int main() {
    #ifdef _WIN32
    // força o console do Windows a trabalhar em UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif


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


    int max_servicos_width = strlen("Servicos Disponiveis");
    for (int i = 1; i <= NUM_BAIRROS; i++) {
        Bairro *b = buscar_bairro_por_id(tabelaHashBairro, i, MAXHASH);
        if (b) {
            char servicos[100] = "";
            bool primeiro = true;

            if (servico_disponivel(&cidade, b->id, BOMBEIRO)) {
                if (!primeiro) strcat(servicos, " ");
                strcat(servicos, "Bombeiro");
                primeiro = false;
            }
            if (servico_disponivel(&cidade, b->id, HOSPITAL)) {
                if (!primeiro) strcat(servicos, " ");
                strcat(servicos, "Hospital");
                primeiro = false;
            }
            if (servico_disponivel(&cidade, b->id, POLICIA)) {
                if (!primeiro) strcat(servicos, " ");
                strcat(servicos, "Policia");
                primeiro = false;
            }
            if (servico_disponivel(&cidade, b->id, AMBULANCIA)) {
                if (!primeiro) strcat(servicos, " ");
                strcat(servicos, "Ambulancia");
            }

            int largura_servicos = strlen(servicos);
            if (largura_servicos > max_servicos_width) {
                max_servicos_width = largura_servicos;
            }
        }
    }

    // definir larguras fixas das colunas
    int largura_id = 5;
    int largura_nome = 23;
    int largura_coord = 22;
    max_servicos_width = max_servicos_width > 25 ? max_servicos_width : 25;
    int largura_servicos = max_servicos_width + 2;

    // imprimir linha superior
    printf("\n\n┌");
    for (int i = 0; i < largura_id; i++) printf("─");
    printf("┬");
    for (int i = 0; i < largura_nome; i++) printf("─");
    printf("┬");
    for (int i = 0; i < largura_coord; i++) printf("─");
    printf("┬");
    for (int i = 0; i < largura_servicos; i++) printf("─");
    printf("┐\n");

    // imprimir cabecalho
    printf("│ %-3s │ %-21s │ %-20s │ %-*s │\n",
        "ID", "Nome do Bairro", "Coordenadas", largura_servicos - 2, "Servicos Disponiveis");

    // imprimir linha divisoria
    printf("├");
    for (int i = 0; i < largura_id; i++) printf("─");
    printf("┼");
    for (int i = 0; i < largura_nome; i++) printf("─");
    printf("┼");
    for (int i = 0; i < largura_coord; i++) printf("─");
    printf("┼");
    for (int i = 0; i < largura_servicos; i++) printf("─");
    printf("┤\n");

    // imprimir os bairros
    for (int i = 1; i <= NUM_BAIRROS; i++) {
        Bairro *b = buscar_bairro_por_id(tabelaHashBairro, i, MAXHASH);
        if (b) {
            char servicos[200] = "";
            bool primeiro = true;

            if (servico_disponivel(&cidade, b->id, BOMBEIRO)) {
                if (!primeiro) strcat(servicos, " ");
                strcat(servicos, "Bombeiro");
                primeiro = false;
            }
            if (servico_disponivel(&cidade, b->id, HOSPITAL)) {
                if (!primeiro) strcat(servicos, " ");
                strcat(servicos, "Hospital");
                primeiro = false;
            }
            if (servico_disponivel(&cidade, b->id, POLICIA)) {
                if (!primeiro) strcat(servicos, " ");
                strcat(servicos, "Policia");
                primeiro = false;
            }
            if (servico_disponivel(&cidade, b->id, AMBULANCIA)) {
                if (!primeiro) strcat(servicos, " ");
                strcat(servicos, "Ambulancia");
            }

            char coordenadas[30];
            snprintf(coordenadas, sizeof(coordenadas), "(%.2f, %.2f)", b->latitude, b->longitude);

            printf("│ %-3d │ %-21s │ %-20s │ %-*s │\n",
                b->id, b->nomeDoBairro, coordenadas, largura_servicos - 2, servicos);
        } else {
            printf("│ %-3d │ %-21s │ %-20s │ %-*s │\n",
                i, "NAO ENCONTRADO!", "", largura_servicos - 2, "");
        }
    }

    // imprimir linha inferior
    printf("└");
    for (int i = 0; i < largura_id; i++) printf("─");
    printf("┴");
    for (int i = 0; i < largura_nome; i++) printf("─");
    printf("┴");
    for (int i = 0; i < largura_coord; i++) printf("─");
    printf("┴");
    for (int i = 0; i < largura_servicos; i++) printf("─");
    printf("┘\n\n");

    // Inicializar as árvores BST e AVL
    inicializarArvores();

    historicoOcorrencias historico;
    inicializar_historico(&historico);

    gerarCidadaosAleatorios(tabelaHashCidadao, tabelaHashBairro, MAXHASH);
    aguardar_enter();


    gerarOcorrenciasIniciais(&cidade, tabelaHashBairro, tabelaHashCidadao, MAXHASH);
    exibirEstadoInicialComAVL(arvoreAVL, tabelaHashBairro);
    aguardar_enter();


    tempoGlobal = 720; // 12:00
    while (tempoGlobal < 1080) { // 18:00
        processarCicloAtendimento(&cidade, tabelaHashBairro, tabelaHashCidadao, &historico);
        aguardar_enter();

    }

    printf("\n\n================ HISTORICO DE OCORRENCIAS ATENDIDAS ENTRE 12h-18h ================\n");
    imprimir_historico(&historico);
    destruir_historico(&historico);

    // Liberar memória das árvores
    for (int b = 0; b < NUM_BAIRROS; b++) {
        for (int s = 0; s < NUM_SERVICOS; s++) {
            liberarArvoreAVL(&arvoreAVL[b][s]);
        }
    }
    liberarArvoreBST(&arvoreBST);

    return 0;
}
