// simulacao.c
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


const char *NOME_SERVICOS[] = {"Bombeiro", "Policia", "Hospital"};
const int TEMPOS_ATENDIMENTO[] = {10, 20, 10};
time_t tempoGlobal = 720;
Atendimento atendimentos_em_curso[NUM_BAIRROS][NUM_SERVICOS] = {0};

static int idxservico(int tipo) {
    switch (tipo) {
        case BOMBEIRO: return 0;
        case HOSPITAL: return 1;
        case POLICIA: return 2;
        default: return -1;
    }
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

void exibirDetalhesCidadao(const Cidadao *cid) {
    printf(" Cidadao: %s\n", cid->nomeCidadao);
    printf(" CPF: %s\n", cid->CPF);
    printf(" Bairro residencia: %d\n", cid->idBairroDeResidencia);
    printf(" Localizacao atual: (%.4f, %.4f)\n", cid->latitude_atual, cid->longitude_atual);
}

void obterServicosStr(Ocorrencia *oc, char *saida) {
    saida[0] = '\0';
    Tarefa *t = oc->tarefas;
    bool primeiro = true;
    while (t) {
        if (!primeiro) strcat(saida, ", ");
        strcat(saida, NOME_SERVICOS[t->servico]);
        t = t->prox;
        primeiro = false;
    }
}

void limparTela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void exibirCabecalho() {
    char horario_str[6];
    formatarHorario(tempoGlobal, horario_str);
    printf("============================================================\n");
    printf("| SISTEMA DE ATENDIMENTO DE EMERGENCIAS - HORARIO: %s |\n", horario_str);
    printf("============================================================\n\n");
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

Ocorrencia *criarOcorrenciaAleatoria(Bairro tabelaHashBairro[], Cidadao *tabelaHashCidadao[], bool ehNova, int maxHash) {
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
    oc->id = rand() % 10000;
    oc->gravidade = rand() % 5 + 1;

    const char *descricoes[][5] = {
        {"Mal-estar leve", "Perturbacao de sossego", "Furto simples", "Animal domestico perdido", "Problema de transito leve"},
        {"Acidente sem vitimas", "Roubo de celular", "Incendio pequeno", "Intoxicacao alimentar", "Briga de rua"},
        {"Acidente com vitimas leves", "Assalto a residencia", "Incendio em area verde", "Queda de arvore", "Ameaca com arma branca"},
        {"Incendio residencial", "Acidente grave", "Tentativa de homicidio", "Pessoa desaparecida", "Violencia domestica"},
        {"Incendio de grandes proporcoes", "Acidente com multiplas vitimas", "Sequestro", "Desabamento", "Tiroreio em via publica"}
    };

    int tipos_servico[][3] = {
        {HOSPITAL, POLICIA, POLICIA},
        {HOSPITAL, POLICIA, BOMBEIRO},
        {HOSPITAL, POLICIA, BOMBEIRO},
        {BOMBEIRO, HOSPITAL, POLICIA},
        {BOMBEIRO, HOSPITAL, POLICIA}
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

    oc->tarefas = NULL;
    if (oc->requer_bombeiro) adicionarTarefa(oc, BOMBEIRO);
    if (oc->requer_hospital) adicionarTarefa(oc, HOSPITAL);
    if (oc->requer_policia) adicionarTarefa(oc, POLICIA);
    if (!oc->tarefas) adicionarTarefa(oc, oc->tipo);

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

    if (ehNova) {
        int offset = rand() % TEMPO_TICK;
        formatarHorario(tempoGlobal + offset, oc->horarioChegada);
    } else {
        int offset = -(rand() % 60 + 1);
        formatarHorario(tempoGlobal + offset, oc->horarioChegada);
    }

    char servicosStr[100] = "";
    obterServicosStr(oc, servicosStr);
    printf("\n [ NOVA OCORRENCIA ]\n");
    printf("--------------------------------------------------------------\n");
    printf(" ID: %04d\n", oc->id);
    printf(" Gravidade: %d\n", oc->gravidade);
    printf(" Servicos: %s\n", servicosStr);
    printf(" Bairro: %s\n", oc->bairro->nomeDoBairro);
    printf(" Descricao: %s\n", oc->descricao);
    printf(" Horario chegada: %s\n", oc->horarioChegada);
    printf("--------------------------------------------------------------\n");

    return oc;
}

void inicializarDescritorFila(DescritorFila *fila) {
    fila->fim = NULL;
    fila->inicio = NULL;
    fila->tamanho = 0;
}

void filaPrioritaria(DescritorFila *fila, Ocorrencia *oc) {
    if (!fila || !oc) return;
    if (!fila->inicio || oc->gravidade > fila->inicio->gravidade) {
        oc->prox = fila->inicio;
        fila->inicio = oc;
        if (!fila->fim) fila->fim = oc;
    } else {
        Ocorrencia *atual = fila->inicio;
        while (atual->prox && atual->prox->gravidade >= oc->gravidade) {
            atual = atual->prox;
        }
        oc->prox = atual->prox;
        atual->prox = oc;
        if (!oc->prox) fila->fim = oc;
    }
    fila->tamanho++;
}

int gerarOcorrenciasIniciais(Bairro bairros[], Cidadao *cidadaos[], DescritorFila *filas[NUM_BAIRROS][NUM_SERVICOS], int maxHash) {
    int total = 0;
    int count = 10 + rand() % 20;
    for (int i = 0; i < count; i++) {
        Ocorrencia *oc = criarOcorrenciaAleatoria(bairros, cidadaos, false, maxHash);
        if (!oc) continue;
        if (!oc->bairro) {
            if (oc->cidadao) oc->cidadao->emOcorrencia = false;
            free(oc);
            continue;
        }
        int rawTipo = oc->tipo;
        int servico = idxservico(rawTipo);
        int bairro_idx = oc->bairro->id - 1;
        if (bairro_idx < 0 || bairro_idx >= NUM_BAIRROS || servico < 0 || servico >= NUM_SERVICOS) {
            if (oc->cidadao) oc->cidadao->emOcorrencia = false;
            free(oc);
            continue;
        }
        filaPrioritaria(filas[bairro_idx][servico], oc);
        total++;
    }
    return total;
}

void exibirEstadoInicialFilas(DescritorFila *filas[NUM_BAIRROS][NUM_SERVICOS], Bairro tabelaHashBairro[]) {
    printf("\n[ ESTADO INICIAL DAS FILAS DE ATENDIMENTO ]\n");
    for (int b = 0; b < NUM_BAIRROS; b++) {
        Bairro *bairro = buscar_bairro_por_id(tabelaHashBairro, b+1, MAXHASH);
        if (!bairro) {
            printf("ERRO: bairro com id %d nao encontrado!\n", b+1);
            continue;
        }
        printf("\n\n\n Bairro: %s\n", bairro->nomeDoBairro);
        printf("====================================================================\n");
        for (int s = 0; s < NUM_SERVICOS; s++) {
            printf("\nServico: %s\n", NOME_SERVICOS[s]);
            printf("--------------------------------------------------------------------\n");
            printf(" | ID | CHEGADA | GRAVIDADE | STATUS|  SERVICOS REQUERIDOS |\n");
            printf("--------------------------------------------------------------------\n");
            if (filas[b][s]->tamanho == 0) {
                printf(" [FILA VAZIA]\n");
            } else {
                Ocorrencia *atual = filas[b][s]->inicio;
                while (atual) {
                    char servicosStr[50] = "";
                    obterServicosStr(atual, servicosStr);
                    printf(" %04d | %s | %d | %s |\n", atual->id, atual->horarioChegada, atual->gravidade, servicosStr);
                    atual = atual->prox;
                }
            }
            printf("--------------------------------------------------------------------\n");
            printf(" TOTAL: %d OCORRENCIAS\n\n\n", filas[b][s]->tamanho);
        }
    }
}

void exibirEstadoPosCiclo(DescritorFila *filas[NUM_BAIRROS][NUM_SERVICOS], Bairro tabelaHashBairro[]) {
    char horario_str[6];
    formatarHorario(tempoGlobal, horario_str);
    printf("\n[ ESTADO DAS FILAS APOS O CICLO (%s) ]\n", horario_str);
    for (int b = 0; b < NUM_BAIRROS; b++) {
        Bairro *bairro = buscar_bairro_por_id(tabelaHashBairro, b+1, MAXHASH);
        if (!bairro) {
            printf("ERRO: bairro com id %d nao encontrado!\n", b+1);
            continue;
        }
        printf("\n\n\n Bairro: %s\n", bairro->nomeDoBairro);
        printf("====================================================================\n");
        for (int s = 0; s < NUM_SERVICOS; s++) {
            printf("\n\n Servico: %s\n", NOME_SERVICOS[s]);
            printf("--------------------------------------------------------------------\n");
            printf("| ID | CHEGADA | GRAVIDADE | STATUS|  SERVICOS REQUERIDOS |\n");
            printf("--------------------------------------------------------------------\n");
            if (atendimentos_em_curso[b][s].ocorrencia) {
                Ocorrencia *oc = atendimentos_em_curso[b][s].ocorrencia;
                char servicosStr[50] = "";
                obterServicosStr(oc, servicosStr);
                printf(" %04d | %s | %d | [EM ATENDIMENTO] |%s |\n", oc->id, oc->horarioChegada, oc->gravidade, servicosStr);
            }
            Ocorrencia *atual = filas[b][s]->inicio;
            while (atual) {
                char servicosStr[50] = "";
                obterServicosStr(atual, servicosStr);
                printf(" %04d | %s | %d | [PENDENTE] | %s |\n", atual->id, atual->horarioChegada, atual->gravidade, servicosStr);
                atual = atual->prox;
            }
            if (!atendimentos_em_curso[b][s].ocorrencia && filas[b][s]->tamanho == 0) {
                printf(" [FILA VAZIA]\n");
            }
            int em_atendimento = atendimentos_em_curso[b][s].ocorrencia ? 1 : 0;
            printf("--------------------------------------------------------------------\n");
            printf(" TOTAL OCORRENCIAS: %d | EM ATENDIMENTO: %d\n\n", filas[b][s]->tamanho + em_atendimento, em_atendimento);
        }
    }
}

void processarCicloAtendimento(DescritorFila *filas[NUM_BAIRROS][NUM_SERVICOS], Bairro tabelaHashBairro[], Cidadao *tabelaHashCidadao[], historicoOcorrencias *historico) {
    char horario_str[6];
    formatarHorario(tempoGlobal, horario_str);
    limparTela();
    exibirCabecalho();
    printf("\n[ CICLO DE ATENDIMENTO (%s) ]\n", horario_str);

    for (int b = 0; b < NUM_BAIRROS; b++) {
        Bairro *bairro = buscar_bairro_por_id(tabelaHashBairro, b+1, MAXHASH);
        if (!bairro) continue;
        printf("\nBairro: %s\n", bairro->nomeDoBairro);
        printf("--------------------------------------------------------------------\n");
        for (int s = 0; s < NUM_SERVICOS; s++) {
            printf("\nServico: %s\n", NOME_SERVICOS[s]);
            printf("--------------------------------------------------------------------\n");
            if (atendimentos_em_curso[b][s].ocorrencia) {
                Atendimento *at = &atendimentos_em_curso[b][s];
                at->tempo_restante -= TEMPO_TICK;
                if (at->tempo_restante <= 0) {
                    printf(" [ATENDIMENTO CONCLUIDO]\n");
                    printf(" ID: %04d\n", at->ocorrencia->id);
                    char servicosStr[50] = "";
                    obterServicosStr(at->ocorrencia, servicosStr);
                    printf(" Servicos: %s\n", servicosStr);
                    int espera = tempoGlobal - minutosFromHorario(at->ocorrencia->horarioChegada);
                    printf(" Chegada: %s | Espera: %d min\n", at->ocorrencia->horarioChegada, espera);
                    printf(" Gravidade: %d\n", at->ocorrencia->gravidade);

                    // REGISTRO NO HISTÓRICO
                    registrar_ocorrencia(historico, at->ocorrencia->horarioChegada, horario_str, at->ocorrencia->gravidade, at->ocorrencia->tipo, at->ocorrencia->bairro);

                    if (at->ocorrencia->cidadao) at->ocorrencia->cidadao->emOcorrencia = false;
                    free(at->ocorrencia);
                    memset(at, 0, sizeof(Atendimento));
                } else {
                    printf(" [ATENDIMENTO EM ANDAMENTO]\n");
                    printf(" ID: %04d\n", at->ocorrencia->id);
                    char servicosStr[50] = "";
                    obterServicosStr(at->ocorrencia, servicosStr);
                    printf(" Servicos: %s\n", servicosStr);
                    int espera = tempoGlobal - minutosFromHorario(at->ocorrencia->horarioChegada);
                    printf(" Chegada: %s | Espera: %d min\n", at->ocorrencia->horarioChegada, espera);
                    printf(" Tempo restante: %d min\n", at->tempo_restante);
                }
            } else if (filas[b][s] && filas[b][s]->inicio) {
                Ocorrencia *oc = filas[b][s]->inicio;
                filas[b][s]->inicio = oc->prox;
                filas[b][s]->tamanho--;
                if (filas[b][s]->tamanho == 0) filas[b][s]->fim = NULL;
                atendimentos_em_curso[b][s].ocorrencia = oc;
                atendimentos_em_curso[b][s].tempo_restante = TEMPOS_ATENDIMENTO[s];
                strcpy(atendimentos_em_curso[b][s].horario_inicio, horario_str);
                printf(" [INICIANDO NOVO ATENDIMENTO]\n");
                printf(" ID: %04d\n", oc->id);
                char servicosStr[50] = "";
                obterServicosStr(oc, servicosStr);
                printf(" Servicos: %s\n", servicosStr);
                int espera = tempoGlobal - minutosFromHorario(oc->horarioChegada);
                printf(" Chegada: %s | Espera: %d min\n", oc->horarioChegada, espera);
                printf(" Tempo previsto: %d min\n", TEMPOS_ATENDIMENTO[s]);
            } else {
                printf(" [SEM OCORRENCIAS PARA ATENDER]\n");
            }
            printf("--------------------------------------------------------------------\n\n\n");
        }
        printf("\n[ NOVAS OCORRENCIAS GERADAS ]\n\n\n");
        int max_novas = 2;
        int novas = rand() % (max_novas + 1);
        for (int i = 0; i < novas; i++) {
            Ocorrencia *oc = criarOcorrenciaAleatoria(tabelaHashBairro, tabelaHashCidadao, true, MAXHASH);
            if (oc) {
                int bairro_idx = oc->bairro->id - 1;
                int servico = oc->tipo;
                if (filas[bairro_idx][servico]->tamanho < 50) {
                    filaPrioritaria(filas[bairro_idx][servico], oc);
                } else {
                    printf(" [FILA CHEIA] Ocorrencia %d descartada\n", oc->id);
                    if (oc->cidadao) oc->cidadao->emOcorrencia = false;
                    free(oc);
                }
            }
        }
    }
    tempoGlobal += TEMPO_TICK;
    printf("\nProcessando proximo ciclo...\n");
    sleep(10);
    limparTela();
    exibirCabecalho();
    exibirEstadoPosCiclo(filas, tabelaHashBairro);
    printf("\nPreparando proximo ciclo...\n");
    sleep(10);
}
