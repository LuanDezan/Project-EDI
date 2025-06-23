#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <locale.h>
#include <unistd.h>
#include "../constantes/constantes.h"
#include "../constantes/simulacao.h"
#include "../constantes/sistemaEmergencia.h"



// definicoes corporativas de layout
#define TABLE_WIDTH 80

// imprime linha horizontal superior/intermediaria com o char c
void print_horizontal(char c) {
    putchar('┌');
    for (int i = 0; i < TABLE_WIDTH - 2; i++) putchar(c);
    putchar('┐');
    putchar('\n');
}

// imprime borda de conteudo alinhado à esquerda
void print_row(const char *text) {
    // TABLE_WIDTH-4 = espaco interno disponivel (bordas + 2 espacos)
    printf("│ %-*s │\n", TABLE_WIDTH - 4, text);
}

// imprime linha horizontal inferior com o char c
void print_bottom(char c) {
    putchar('└');
    for (int i = 0; i < TABLE_WIDTH - 2; i++) putchar(c);
    putchar('┘');
    putchar('\n');
}

// conta numero de nos em uma arvore AVL
int contar_nos_avl(noAVL *raiz) {
    if (!raiz) return 0;
    return contar_nos_avl(raiz->esq) + 1 + contar_nos_avl(raiz->dir);
}


// Implementação de funções auxiliares
void obterServicosStr(Ocorrencia *oc, char *saida) {
    saida[0] = '\0';
    Tarefa *t = oc->tarefas;
    bool primeiro = true;

    while (t) {
        if (!primeiro) strcat(saida, ", ");
        strcat(saida, NOME_SERVICOS[t->servico]);
        if (t->concluida) strcat(saida, "(C)");
        t = t->prox;
        primeiro = false;
    }
}

static unsigned int global_id_counter = 0;
time_t tempoGlobal = 720;
Atendimento atendimentos_em_curso[NUM_BAIRROS][NUM_SERVICOS] = {0};
noAVL *arvoreAVL[NUM_BAIRROS][NUM_SERVICOS];
no *arvoreBST;

void inicializarArvores() {
    for (int b = 0; b < NUM_BAIRROS; b++) {
        for (int s = 0; s < NUM_SERVICOS; s++) {
            inicializarAVL(&arvoreAVL[b][s]);
        }
    }
    inicializar(&arvoreBST);
}

static int idxservico(int tipo) {
    switch (tipo) {
        case BOMBEIRO: return 0;
        case HOSPITAL: return 1;
        case POLICIA: return 2;
        case AMBULANCIA: return 3;
        default: return -1;
    }
}

void adicionarOcorrenciaNasArvores(Ocorrencia *oc) {
    inserir(&arvoreBST, oc);
    int bairro_idx = oc->bairro->id - 1;
    Tarefa *t = oc->tarefas;
    while (t) {
        int s = idxservico(t->servico);
        if (s >= 0 && s < NUM_SERVICOS) {
            inserirRec(&arvoreAVL[bairro_idx][s], oc);
        }
        t = t->prox;
    }
}

Ocorrencia *removerProximaDaAVL(int bairro, int servico) {
    Ocorrencia *oc = proximaChamada(arvoreAVL[bairro][servico]);
    if (oc) {
        arvoreAVL[bairro][servico] = removerRec(arvoreAVL[bairro][servico], oc);
    }
    return oc;
}

static void exibirFilaAVL(noAVL *raiz) {
    if (raiz == NULL) return;
    exibirFilaAVL(raiz->dir);
    Ocorrencia *oc = raiz->info;
    printf("| ID: %04d | Hora: %s | Gravidade: %d | Filas: %d |\n",
        oc->id, oc->horarioChegada, oc->gravidade, oc->tarefas_pendentes);
    exibirFilaAVL(raiz->esq);
}

static void exibirFilaAVLComContador(noAVL *raiz, int *pos, int *total_ocorrencias) {
    if (raiz == NULL) return;
    exibirFilaAVLComContador(raiz->dir, pos, total_ocorrencias);
    Ocorrencia *oc = raiz->info;
    printf(" %2d. [ID:%04d] Gravidade: %d | Chegada: %s | Filas: %d\n",
        (*pos)++, oc->id, oc->gravidade, oc->horarioChegada, oc->tarefas_pendentes);
    (*total_ocorrencias)++;
    exibirFilaAVLComContador(raiz->esq, pos, total_ocorrencias);
}

static void exibirFilaServicoRec(noAVL *raiz, int *pos) {
    if (raiz == NULL) return;
    exibirFilaServicoRec(raiz->dir, pos);
    Ocorrencia *oc = raiz->info;
    printf("%7d | %13d | %9d | %16s | %s\n",
        (*pos)++, oc->id, oc->gravidade, oc->horarioChegada, oc->cidadao->nomeCidadao);
    exibirFilaServicoRec(raiz->esq, pos);
}

static void exibirPainelRec(noAVL *raiz, int *pos) {
    if (raiz == NULL) return;
    exibirPainelRec(raiz->dir, pos);
    Ocorrencia *oc = raiz->info;
    printf("%7d | %13d | %9d | %15s | %s\n",
        (*pos)++, oc->id, oc->gravidade, oc->horarioChegada, oc->cidadao->nomeCidadao);
    exibirPainelRec(raiz->esq, pos);
}

int minutosFromHorario(const char* horario) {
    int horas, minutos;
    if (sscanf(horario, "%d:%d", &horas, &minutos) != 2) return 0;
    return horas * 60 + minutos;
}

void formatarHorario(int totalMinutos, char *saida) {
    totalMinutos = totalMinutos % (24*60);
    int horas = totalMinutos / 60;
    int minutos = totalMinutos % 60;
    sprintf(saida, "%02d:%02d", horas, minutos);
}

//  exibirCabecalho
void exibirCabecalho() {
    char horario_str[6];
    formatarHorario(tempoGlobal, horario_str);

    printf("\n\n\n");
    printf("╔════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║ SISTEMA DE ATENDIMENTO DE EMERGENCIAS                                      ║\n");
    printf("╠════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║                                                                            ║\n");
    printf("║ HORARIO ATUAL: %-58s  ║\n", horario_str);
    printf("║ CICLO DE SIMULACAO: %-54d ║\n", (int)((tempoGlobal - 720) / TEMPO_TICK));
    printf("║                                                                            ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void adicionarTarefa(Ocorrencia* oc, int servico) {
    if (!oc) return;
    Tarefa* nova = malloc(sizeof(Tarefa));
    if (!nova) return;
    nova->servico = servico;
    nova->concluida = false;
    strcpy(nova->horarioConclusao, "--:--");
    nova->prox = oc->tarefas;
    oc->tarefas = nova;
}

Ocorrencia *criarOcorrenciaAleatoria(Cidade *cidade, Bairro tabelaHashBairro[], Cidadao *tabelaHashCidadao[], bool ehNova, int maxHash) {
    Ocorrencia *oc = (Ocorrencia*) malloc(sizeof(Ocorrencia));
    if (!oc) return NULL;
    int tentativas = 0;

    do {
        oc->cidadao = selecionar_cidadao_aleatorio(tabelaHashCidadao, maxHash);
        tentativas++;
        if (tentativas > 100) {
            free(oc);
            return NULL;
        }
    } while (oc->cidadao && oc->cidadao->emOcorrencia);

    if (!oc->cidadao) {
        free(oc);
        return NULL;
    }

    oc->cidadao->emOcorrencia = true;
    oc->id = global_id_counter++;
    oc->gravidade = rand() % 5 + 1;
    oc->tarefas_pendentes = 0;

    const char *descricoes[][5] = {
        {"Mal-estar leve", "Perturbacao de sossego", "Furto simples", "Animal domestico perdido", "Problema de transito leve"},
        {"Acidente sem vitimas", "Roubo de celular", "Incendio pequeno", "Intoxicacao alimentar", "Briga de rua"},
        {"Acidente com vitimas leves", "Assalto a residencia", "Incendio em area verde", "Queda de arvore", "Ameaca com arma blanco"},
        {"Incendio residencial", "Acidente grave", "Tentativa de homicidio", "Pessoa desaparecida", "Violencia domestica"},
        {"Incendio de grandes proporcoes", "Acidente com multiplas vitimas", "Sequestro", "Desabamento", "Tiroreio em via publica"}
    };

    int tipos_servico[][4] = {
        {HOSPITAL, POLICIA, POLICIA, AMBULANCIA},
        {HOSPITAL, POLICIA, BOMBEIRO, AMBULANCIA},
        {HOSPITAL, POLICIA, BOMBEIRO, AMBULANCIA},
        {BOMBEIRO, HOSPITAL, POLICIA, AMBULANCIA},
        {BOMBEIRO, HOSPITAL, POLICIA, AMBULANCIA}
    };

    int grav_idx = oc->gravidade - 1;
    int desc_idx = rand() % 5;
    int tipo_idx = rand() % 3;
    oc->tipo = tipos_servico[grav_idx][tipo_idx];
    strncpy(oc->descricao, descricoes[grav_idx][desc_idx], sizeof(oc->descricao));
    oc->descricao[sizeof(oc->descricao) - 1] = '\0';

    oc->requer_bombeiro = (oc->tipo == BOMBEIRO) || (oc->gravidade >= 4 && rand() % 100 > 30);
    oc->requer_hospital = (oc->tipo == HOSPITAL) || (oc->gravidade >= 3 && rand() % 100 > 40);
    oc->requer_policia = (oc->tipo == POLICIA) || (oc->gravidade >= 2 && rand() % 100 > 20);
    oc->requer_ambulancia = (oc->tipo == AMBULANCIA) || (oc->gravidade >= 3 && rand() % 100 > 50);

    oc->tarefas = NULL;
    if (oc->requer_bombeiro) adicionarTarefa(oc, BOMBEIRO);
    if (oc->requer_hospital) adicionarTarefa(oc, HOSPITAL);
    if (oc->requer_policia) adicionarTarefa(oc, POLICIA);
    if (oc->requer_ambulancia) adicionarTarefa(oc, AMBULANCIA);

    if (!oc->tarefas) adicionarTarefa(oc, oc->tipo);

    Tarefa *t = oc->tarefas;
    while (t) {
        oc->tarefas_pendentes++;
        t = t->prox;
    }

    if (rand() % 100 < 60) {
        oc->bairro = buscar_bairro_por_id(tabelaHashBairro, oc->cidadao->idBairroDeResidencia, maxHash);
    } else {
        oc->bairro = encontrar_bairro_mais_proximo(oc->cidadao->latitude_atual, oc->cidadao->longitude_atual, tabelaHashBairro, maxHash);
    }

    if (!oc->bairro) {
        int fallback_id = (rand() % NUM_BAIRROS) + 1;
        oc->bairro = buscar_bairro_por_id(tabelaHashBairro, fallback_id, maxHash);
    }

    if (!oc->bairro) {
        oc->cidadao->emOcorrencia = false;
        free(oc);
        return NULL;
    }

    if (oc->requer_bombeiro && !servico_disponivel(cidade, oc->bairro->id, BOMBEIRO)) {
        Bairro* novo_bairro = encontrar_bairro_com_servico(cidade, oc->cidadao->latitude_atual, oc->cidadao->longitude_atual, BOMBEIRO, tabelaHashBairro, maxHash);
        if (novo_bairro) oc->bairro = novo_bairro;
    }

    if (oc->requer_hospital && !servico_disponivel(cidade, oc->bairro->id, HOSPITAL)) {
        Bairro* novo_bairro = encontrar_bairro_com_servico(cidade, oc->cidadao->latitude_atual, oc->cidadao->longitude_atual, HOSPITAL, tabelaHashBairro, maxHash);
        if (novo_bairro) oc->bairro = novo_bairro;
    }

    if (oc->requer_policia && !servico_disponivel(cidade, oc->bairro->id, POLICIA)) {
        Bairro* novo_bairro = encontrar_bairro_com_servico(cidade, oc->cidadao->latitude_atual, oc->cidadao->longitude_atual, POLICIA, tabelaHashBairro, maxHash);
        if (novo_bairro) oc->bairro = novo_bairro;
    }

    if (ehNova) {
        int offset = rand() % 5;
        formatarHorario(tempoGlobal + offset, oc->horarioChegada);
    } else {
        int offset = (rand() % 60) + 1;
        formatarHorario(tempoGlobal - offset, oc->horarioChegada);
    }

    if (oc->gravidade < 4) {
        oc->bairro = buscar_bairro_por_id(tabelaHashBairro, oc->cidadao->idBairroDeResidencia, maxHash);
        strcpy(oc->motivoEscolhaBairro, "Bairro residencia (gravidade < 4)");
    } else {
        oc->bairro = encontrar_bairro_mais_proximo(oc->cidadao->latitude_atual, oc->cidadao->longitude_atual, tabelaHashBairro, maxHash);
        strcpy(oc->motivoEscolhaBairro, "Bairro mais proximo (gravidade >= 4)");
    }

    char servicosStr[100] = "";
    obterServicosStr(oc, servicosStr);

    printf("\n\n===============================================================================\n");
    printf("                             <<< NOVA OCORRENCIA >>> \n");
    printf("===============================================================================\n");
    printf(" ID .............................: %04d\n", oc->id);
    printf(" Gravidade ......................: %d\n", oc->gravidade);
    printf(" Servicos .......................: %s\n", servicosStr);
    printf(" Bairro .........................: %s\n", oc->bairro->nomeDoBairro);
    printf(" Descricao ......................: %s\n", oc->descricao);
    printf(" Horario chegada ................. %s\n", oc->horarioChegada);
    printf(" Presente em ....................: %d fila(s)\n", oc->tarefas_pendentes);
    printf(" Motivo escolha bairro ..........: %s\n\n", oc->motivoEscolhaBairro);
    printf("===============================================================================\n\n");

    oc->finalizada = false;
    return oc;
}

void exibirOcorrencia(Ocorrencia* oc) {
    char servicosStr[100] = "";
    obterServicosStr(oc, servicosStr);

    printf("\n[ NOVA OCORRENCIA REGISTRADA ]\n");
    printf("============================================================\n");
    printf(" ID: %04d\n", oc->id);
    printf(" Gravidade: %d\n", oc->gravidade);
    printf(" Servicos: %s\n", servicosStr);
    printf(" Bairro: %s\n", oc->bairro->nomeDoBairro);
    printf(" Descricao: %s\n", oc->descricao);
    printf(" Horario chegada: %s\n", oc->horarioChegada);
    printf(" Presente em: %d fila(s)\n", oc->tarefas_pendentes);
    printf("============================================================\n\n");
}

int gerarOcorrenciasIniciais(Cidade *cidade, Bairro bairros[], Cidadao *cidadaos[], int maxHash) {
    int total = 0;
    int count = 10 + rand() % 10;

    for (int i = 0; i < count; i++) {
        Ocorrencia *oc = criarOcorrenciaAleatoria(cidade, bairros, cidadaos, false, maxHash);
        if (!oc || !oc->bairro) continue;
        adicionarOcorrenciaNasArvores(oc);
        total++;
    }
    return total;
}



void exibirEstadoInicialComAVL(noAVL *filas[NUM_BAIRROS][NUM_SERVICOS], Bairro tabelaHashBairro[]) {
     printf("\n\n\n\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    printf("                 <<< ESTADO INICIAL DAS FILAS DE ATENDIMENTO >>>                 \n");
    printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");


    for (int b = 0; b < NUM_BAIRROS; b++) {
        Bairro *bairro = buscar_bairro_por_id(tabelaHashBairro, b + 1, MAXHASH);
        if (!bairro) continue;
          printf("\n\n\n\n\n==============================================================================\n");
        printf("                            <<< BAIRRO: %s >>> ", bairro->nomeDoBairro);
          printf("\n==============================================================================");
        for (int s = 0; s < NUM_SERVICOS; s++) {
            printf("\n\n\n>>> SERVICO: %s <<<\n", NOME_SERVICOS[s]);
            printf("\n-------------------------------- FILA --------------------------------\n");
            if (arvoreAVL[b][s] == NULL) {
                printf("    [vazio] \n");
            } else {
                exibirFilaAVL(arvoreAVL[b][s]);
            }


            printf("---------------------------------------------------------------------------\n");


        }

    }


        printf("\n\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");

}




void exibirEstadoPosCiclo(DescritorFila *filas[][NUM_SERVICOS], Bairro tabelaHashBairro[]) {
    char horario_str[6];
    formatarHorario(tempoGlobal + TEMPO_TICK, horario_str);
    int total_ocorrencias = 0;
    int ciclo_atual = (tempoGlobal - 720) / TEMPO_TICK;

    printf("\n\n\n");
    printf("==============================================================================\n");
    printf("============= ESTADO DAS FILAS APOS %d CICLO(S) - %-8s =============\n", ciclo_atual, horario_str);
    printf("==============================================================================\n");
    printf("\n");

    for (int b = 0; b < NUM_BAIRROS; b++) {
        Bairro *bairro = buscar_bairro_por_id(tabelaHashBairro, b+1, MAXHASH);
        if (!bairro) continue;

        printf("\n\n\n");
        printf("BAIRRO: %s\n", bairro->nomeDoBairro);
        printf("------------------------------------------------------------------------------\n\n");

        for (int s = 0; s < NUM_SERVICOS; s++) {
            printf(" Servico: %s\n", NOME_SERVICOS[s]);
            printf("  --------------------------------------------------------------------------\n");

            if (filas[b][s]->tamanho == 0) {
                printf(" >>> FILA VAZIA <<<\n");
            } else {
                int pos = 1;
                exibirFilaAVLComContador(filas[b][s]->raiz, &pos, &total_ocorrencias);
            }

            printf("\n Total na fila: %d ocorrencia(s)\n\n", filas[b][s]->tamanho);
            printf("\n");
        }
            printf("  ----------------------------------------------------------------------------\n\n");
    }

    printf("==============================================================================\n");
    printf("    TOTAL GERAL DE OCORRENCIAS: %d\n", total_ocorrencias);
    printf("==============================================================================\n");
    printf("\n\n");
}

void exibirOcorrenciaDetalhada(Ocorrencia* oc) {
    char servicosStr[100] = "";
    obterServicosStr(oc, servicosStr);

    printf("============================================================\n");
    printf(" ID: %04d\n", oc->id);
    printf(" Gravidade: %d\n", oc->gravidade);
    printf(" Servicos: %s\n", servicosStr);
    printf(" Bairro: %s\n", oc->bairro->nomeDoBairro);
    printf(" Descricao: %s\n", oc->descricao);
    printf(" Horario chegada: %s\n", oc->horarioChegada);
    printf(" Presente em : %d fila(s)\n", oc->tarefas_pendentes);
    printf("\n INSERIDA NAS FILAS DO BAIRRO %s:\n", oc->bairro->nomeDoBairro);

    Tarefa* t = oc->tarefas;
    while (t) {
        printf(" - %s\n", NOME_SERVICOS[t->servico]);
        t = t->prox;
    }
    printf("============================================================\n\n");
}

void exibirFilaServico(DescritorFila* fila, const char* nomeServico) {
    printf("\n Fila de atendimento: %s\n", nomeServico);
    printf(" Ocorrencias aguardando: %d\n", fila->tamanho);
    printf("---------------------------------------------------\n");

    if(fila->tamanho == 0) {
        printf("Nenhuma ocorrencia na fila\n");
        return;
    }

    printf("Posicao | Identificacao | Gravidade | Horario registro | Cidadao\n");
    printf("--------|---------------|-----------|------------------|-------------------\n");

    int pos = 1;
    exibirFilaServicoRec(fila->raiz, &pos);
}

void exibirStatusAtendimento(Atendimento* at, const char* nomeServico, int servico_idx) {
    if(!at->ocorrencia) {
        printf("\n Status: Servico disponivel (aguardando ocorrencias)\n");
        return;
    }

    printf("\n--- ATENDIMENTO EM ANDAMENTO ---\n");
    printf("Servico: %s\n", nomeServico);
    printf("Ocorrencia: %04d\n", at->ocorrencia->id);
    printf("Cidadao: %s\n", at->ocorrencia->cidadao->nomeCidadao);
    printf("Tempo restante: %d minutos\n", at->tempo_restante);

    int completado = TEMPOS_ATENDIMENTO[servico_idx] - at->tempo_restante;
    int total = TEMPOS_ATENDIMENTO[servico_idx];
    printf("Progresso: %d/%d minutos\n", completado, total);
}

void exibirPainelFilas(DescritorFila* filas[NUM_BAIRROS][NUM_SERVICOS], Bairro tabelaHashBairro[]) {
    for(int b = 0; b < NUM_BAIRROS; b++) {
        Bairro* bairro = buscar_bairro_por_id(tabelaHashBairro, b+1, MAXHASH);
        if(!bairro) continue;

        printf("\n\n================================================\n");
        printf(" BAIRRO: %s\n", bairro->nomeDoBairro);
        printf("================================================\n");

        for(int s = 0; s < NUM_SERVICOS; s++) {
            printf("\n Fila de atendimento: %s\n", NOME_SERVICOS[s]);
            printf("Total de ocorrencias na fila: %d\n", filas[b][s]->tamanho);
            printf("---------------------------------------------------------------------------\n");

            if(filas[b][s]->tamanho == 0) {
                printf("Nenhuma ocorrencia na fila\n");
                continue;
            }

            printf(" Posicao | Identificacao | Gravidade | Horario chegada | Cidadao\n");
            printf("--------|---------------|-----------|-----------------|-------------------\n");

            int pos = 1;
            exibirPainelRec(filas[b][s]->raiz, &pos);
        }
    }
}




static void exibirFilasDeEspera(Bairro tabelaHashBairro[]) {
    printf("\n\n\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
printf("                <<< ESTADO INICIAL DAS FILAS DE ATENDIMENTO >>>           \n");
printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");

    for (int b = 0; b < NUM_BAIRROS; b++) {
        Bairro* bairro = buscar_bairro_por_id(tabelaHashBairro, b+1, MAXHASH);
        if (!bairro) continue;
        printf("\n\n\n------------------------------ BAIRRO: %s ------------------------------\n", bairro->nomeDoBairro);

        for (int s = 0; s < NUM_SERVICOS; s++) {
            printf("\n\n>>> SERVICO: %s <<<\n", NOME_SERVICOS[s]);
            printf("-------------------------------- FILA --------------------------------\n");
            if (arvoreAVL[b][s] == NULL) {
                printf(" [vazio] \n");
            } else {
                int pos = 1;
                int total_ocorrencias = 0;
                exibirFilaAVLComContador(arvoreAVL[b][s], &pos, &total_ocorrencias);
                printf("\nTotal na fila: %d ocorrencia(s)\n", total_ocorrencias);
            }
            printf("\n");
        }
    }
    printf("================================================\n");
}
// Função para exibir serviços de um bairro (adicionar antes de processarCicloAtendimento)
void exibirServicosDoBairro(Bairro *bairro, int bairro_idx) {
    printf("\n\n\n\n┌────────────────────────────────────────────────────────────────────────────────┐\n");
    printf("│ BAIRRO: %-66s     │\n", bairro->nomeDoBairro);
    printf("└────────────────────────────────────────────────────────────────────────────────┘\n");

    for (int s = 0; s < NUM_SERVICOS; s++) {
        Atendimento *at = &atendimentos_em_curso[bairro_idx][s];
        noAVL *fila = arvoreAVL[bairro_idx][s];
        int tamanho_fila = 0;

        // Contar elementos na fila
        if (fila != NULL) {
            int contador = 0;
            void contarFila(noAVL *raiz, int *cont) {
                if (raiz == NULL) return;
                contarFila(raiz->esq, cont);
                (*cont)++;
                contarFila(raiz->dir, cont);
            }
            contarFila(fila, &tamanho_fila);
        }
        printf("\n\n SERVIÇO: %-15s \n", NOME_SERVICOS[s]);

        if (at->ocorrencia) {
                  printf("  ┌──────────────────────────────────────────────────────────────────────┐\n");
            printf("  │ >>> ATENDIMENTO EM ANDAMENTO                                         │\n");
             printf("  ├──────────────────────────────────────────────────────────────────────┤\n");
            printf("  │ Ocorrência: %-55d  │\n", at->ocorrencia->id);
            printf("  │ Cidadão: %-58s  │\n", at->ocorrencia->cidadao->nomeCidadao);
            printf("  │ Tempo restante: %-50d   │\n", at->tempo_restante);
            printf("  └──────────────────────────────────────────────────────────────────────┘\n");
        } else {

                printf("  ┌──────────────────────────────────────────────────────────────────────┐\n");
                printf("  │ >>> NENHUM ATENDIMENTO EM ANDAMENTO                                  │\n");
                printf("  └──────────────────────────────────────────────────────────────────────┘\n\n");
            }

        }

    }



// Função processarCicloAtendimento modificada
void processarCicloAtendimento(Cidade *cidade, Bairro tabelaHashBairro[], Cidadao *tabelaHashCidadao[], historicoOcorrencias *historico) {
    char horario_str[6];
    formatarHorario(tempoGlobal, horario_str);
    exibirCabecalho();
        exibirFilasDeEspera(tabelaHashBairro);
    printf("\n\n                           ══════════════════════════════\n");
    printf("                            CICLO DE ATENDIMENTO %s\n", horario_str);
    printf("                           ══════════════════════════════\n\n\n");

    for (int b = 0; b < NUM_BAIRROS; b++) {
        Bairro *bairro = buscar_bairro_por_id(tabelaHashBairro, b + 1, MAXHASH);
        if (!bairro) continue;

        exibirServicosDoBairro(bairro, b);

        for (int s = 0; s < NUM_SERVICOS; s++) {
            Atendimento *at = &atendimentos_em_curso[b][s];
            if (at->ocorrencia && at->ocorrencia->finalizada) {
                memset(at, 0, sizeof(Atendimento));
                continue;
            }

            if (at->ocorrencia) {
                at->tempo_restante -= TEMPO_TICK;
                if (at->tempo_restante <= 0) {
                    Tarefa *t = at->ocorrencia->tarefas;
                    while (t) {
                        if (t->servico == s && !t->concluida) {
                            t->concluida = true;
                            formatarHorario(tempoGlobal, t->horarioConclusao);
                            at->ocorrencia->tarefas_pendentes--;
                            break;
                        }
                        t = t->prox;
                    }

                    if (at->ocorrencia->tarefas_pendentes == 0) {
                        at->ocorrencia->finalizada = true;
                        registrar_ocorrencia(historico, at->ocorrencia, horario_str);
                        if (at->ocorrencia->cidadao) at->ocorrencia->cidadao->emOcorrencia = false;
                        Tarefa *u = at->ocorrencia->tarefas;
                        while (u) {
                            Tarefa *prox = u->prox;
                            free(u);
                            u = prox;
                        }
                        remover(&arvoreBST, at->ocorrencia->id);
                        at->ocorrencia = NULL;
                    }
                    memset(at, 0, sizeof(Atendimento));
                }
            } else {
                Ocorrencia *oc = removerProximaDaAVL(b, s);
                if (oc && !oc->finalizada) {
                    at->ocorrencia = oc;
                    at->tempo_restante = TEMPOS_ATENDIMENTO[s];
                    strcpy(at->horario_inicio, horario_str);
                }
            }
        }
    }

    Ocorrencia *nova = criarOcorrenciaAleatoria(cidade, tabelaHashBairro, tabelaHashCidadao, true, MAXHASH);
    if (nova) {
        adicionarOcorrenciaNasArvores(nova);
    }

    tempoGlobal += TEMPO_TICK;
}




void liberarArvoreBST(no **raiz) {
    if (*raiz == NULL) return;
    liberarArvoreBST(&(*raiz)->esq);
    liberarArvoreBST(&(*raiz)->dir);
    free(*raiz);
    *raiz = NULL;
}
