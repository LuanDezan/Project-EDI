#include"../constantes/constantes.h"
#include <locale.h>
#define MAX 40
#define MAXHASH 20
#define NUM_BAIRROS 4
#define NUM_SERVICOS 4  // Alterado para 4 serviços
#include"../constantes/simulacao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include<stdbool.h>

const char *NOME_SERVICOS[] = {"Bombeiro", "Policia", "Hospital", "Ambulância"};
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


    int max_servicos_width = strlen("Servicos Disponiveis"); // garantir pelo menos o tamanho do cabecalho

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

// garantir largura minima de 25 para a coluna de servicos
max_servicos_width = max_servicos_width > 25 ? max_servicos_width : 25;
int largura_servicos = max_servicos_width + 2; // +2 para margem nas bordas laterais

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


historicoOcorrencias historico;
    inicializar_historico(&historico);
    gerarCidadaosAleatorios(tabelaHashCidadao, tabelaHashBairro, MAXHASH);
    aguardar_enter();
    limpar_tela();
    int totalOcorrencias = gerarOcorrenciasIniciais(&cidade, tabelaHashBairro, tabelaHashCidadao, filas, MAXHASH);
    exibirEstadoInicialFilas(filas, tabelaHashBairro);
    aguardar_enter();
    limpar_tela();
    tempoGlobal = 720; // 12:00
    while (tempoGlobal < 1080) { // 18:00
        processarCicloAtendimento(filas, &cidade, tabelaHashBairro, tabelaHashCidadao, &historico);
        aguardar_enter();
        limpar_tela();
    }
    printf("\n\n================ HISTORICO DE OCORRENCIAS ATENDIDAS ENTRE 12h-18h ================\n");
    imprimir_historico(&historico);
    destruir_historico(&historico);
    return 0;
}
