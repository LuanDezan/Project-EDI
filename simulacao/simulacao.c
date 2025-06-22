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
#define MAX_OCORRENCIAS_SIMULTANEAS 200
static unsigned int global_id_counter = 0;

time_t tempoGlobal = 720;
Atendimento atendimentos_em_curso[NUM_BAIRROS][NUM_SERVICOS] = {0};

static int idxservico(int tipo) {
    switch (tipo) {
        case BOMBEIRO: return 0;
        case HOSPITAL: return 1;
        case POLICIA: return 2;
        case AMBULANCIA: return 3; // Novo serviço
        default: return -1;
    }
}
// converte um horario no formato string para minutos
int minutosFromHorario(const char* horario) {
    int horas, minutos;
    // extrai horas e minutos da string
    if (sscanf(horario, "%d:%d", &horas, &minutos) != 2) return 0;
    return horas * 60 + minutos;
}

// converte minutos para o formato de horario (HH:MM)
void formatarHorario(int totalMinutos, char *saida) {
    // garante que o tempo fique dentro de um dia (0-1440 minutos)
    totalMinutos = totalMinutos % (24*60);
    int horas = totalMinutos / 60;
    int minutos = totalMinutos % 60;
    // formata a string com 2 digitos para horas e minutos
    sprintf(saida, "%02d:%02d", horas, minutos);
}


void exibirDetalhesCidadao(const Cidadao *cid) {
    printf(" Cidadao: %s\n", cid->nomeCidadao);
    printf(" CPF: %s\n", cid->CPF);
    printf(" Bairro residencia: %d\n", cid->idBairroDeResidencia);
    printf(" Localizacao atual: (%.4f, %.4f)\n", cid->latitude_atual, cid->longitude_atual);
}

// cria uma string com a lista de servicos de uma ocorrencia
void obterServicosStr(Ocorrencia *oc, char *saida) {
    saida[0] = '\0'; // inicia a string vazia
    Tarefa *t = oc->tarefas;
    bool primeiro = true;
    while (t) {
        if (!primeiro) strcat(saida, ", "); // adiciona virgula entre servicos
        strcat(saida, NOME_SERVICOS[t->servico]); // adiciona nome do servico
        if (t->concluida) {
            strcat(saida, "(C)"); // marca servicos concluidos
        }
        t = t->prox; // proxima tarefa
        primeiro = false;
    }
}

// exibe o cabecalho do sistema com informacoes do tempo e ciclo
void exibirCabecalho() {
    char horario_str[6];
    formatarHorario(tempoGlobal, horario_str);
    printf("\n\n\n");
    printf("╔════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                     SISTEMA DE ATENDIMENTO DE EMERGENCIAS                  ║\n");
    printf("╠════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║                                                                            ║\n");
    printf("║    HORARIO ATUAL:    %-10s                                            ║\n", horario_str);
    printf("║    CICLO DE SIMULACAO: %-4d                                                ║\n",  (int)((tempoGlobal - 720) / TEMPO_TICK));
    printf("║                                                                            ║\n");
    printf("╚════════════════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}


// adiciona uma nova tarefa a uma ocorrencia
void adicionarTarefa(Ocorrencia* oc, int servico) {
    if (!oc) return; // verifica se a ocorrencia existe
    Tarefa* nova = malloc(sizeof(Tarefa)); // aloca memoria para nova tarefa
    if (!nova) return; // verifica alocacao
    nova->servico = servico; // define o tipo de servico
    nova->concluida = false; // marca como nao concluida
    strcpy(nova->horarioConclusao, "--:--"); // inicia horario vazio
    nova->prox = oc->tarefas; // adiciona no inicio da lista
    oc->tarefas = nova; // atualiza ponteiro
}

// cria uma nova ocorrencia aleatoria
Ocorrencia *criarOcorrenciaAleatoria(Cidade *cidade, Bairro tabelaHashBairro[], Cidadao *tabelaHashCidadao[], bool ehNova, int maxHash) {
    Ocorrencia *oc = (Ocorrencia*) malloc(sizeof(Ocorrencia));
    if (!oc) return NULL; // verifica alocacao

    // tenta encontrar um cidadao disponivel
    int tentativas = 0;
    do {
        oc->cidadao = selecionar_cidadao_aleatorio(tabelaHashCidadao, maxHash);
        tentativas++;
        // evita loop infinito
        if (tentativas > 100) {
            free(oc);
            return NULL;
        }
    } while (oc->cidadao && oc->cidadao->emOcorrencia); // repete ate achar cidadao livre

    // se nao encontrou cidadao, cancela
    if (!oc->cidadao) {
        free(oc);
        return NULL;
    }

    // marca cidadao como ocupado
    oc->cidadao->emOcorrencia = true;
    oc->id = global_id_counter++; // atribui id unico
    oc->gravidade = rand() % 5 + 1; // gravidade aleatoria entre 1 e 5
    oc->tarefas_pendentes = 0; // inicia sem tarefas pendentes

    // banco de descricoes predefinidas por nivel de gravidade
    const char *descricoes[][5] = {
        {"Mal-estar leve", "Perturbacao de sossego", "Furto simples", "Animal domestico perdido", "Problema de transito leve"},
        {"Acidente sem vitimas", "Roubo de celular", "Incendio pequeno", "Intoxicacao alimentar", "Briga de rua"},
        {"Acidente com vitimas leves", "Assalto a residencia", "Incendio em area verde", "Queda de arvore", "Ameaca com arma blanco"},
        {"Incendio residencial", "Acidente grave", "Tentativa de homicidio", "Pessoa desaparecida", "Violencia domestica"},
        {"Incendio de grandes proporcoes", "Acidente com multiplas vitimas", "Sequestro", "Desabamento", "Tiroreio em via publica"}
    };

 // tipos de servicos mais comuns por gravidade
 int tipos_servico[][4] = {
        {HOSPITAL, POLICIA, POLICIA, AMBULANCIA},
        {HOSPITAL, POLICIA, BOMBEIRO, AMBULANCIA},
        {HOSPITAL, POLICIA, BOMBEIRO, AMBULANCIA},
        {BOMBEIRO, HOSPITAL, POLICIA, AMBULANCIA},
        {BOMBEIRO, HOSPITAL, POLICIA, AMBULANCIA}
    };
    // seleciona descricao e tipo baseado na gravidade
    int grav_idx = oc->gravidade - 1;
    int desc_idx = rand() % 5;
    int tipo_idx = rand() % 3;

    oc->tipo = tipos_servico[grav_idx][tipo_idx]; // define tipo principal
    strncpy(oc->descricao, descricoes[grav_idx][desc_idx], sizeof(oc->descricao)); // copia descricao
    oc->descricao[sizeof(oc->descricao) - 1] = '\0'; // garante terminator

    oc->requer_bombeiro = (oc->tipo == BOMBEIRO) || (oc->gravidade >= 4 && rand() % 100 > 30);
    oc->requer_hospital = (oc->tipo == HOSPITAL) || (oc->gravidade >= 3 && rand() % 100 > 40);
    oc->requer_policia = (oc->tipo == POLICIA) || (oc->gravidade >= 2 && rand() % 100 > 20);
    oc->requer_ambulancia = (oc->tipo == AMBULANCIA) || (oc->gravidade >= 3 && rand() % 100 > 50); // Novo serviço

    // Adiciona tarefas conforme necessário
    oc->tarefas = NULL;
    if (oc->requer_bombeiro) adicionarTarefa(oc, BOMBEIRO);
    if (oc->requer_hospital) adicionarTarefa(oc, HOSPITAL);
    if (oc->requer_policia) adicionarTarefa(oc, POLICIA);
    if (oc->requer_ambulancia) adicionarTarefa(oc, AMBULANCIA); // Novo serviço
    // garante pelo menos uma tarefa
    if (!oc->tarefas) adicionarTarefa(oc, oc->tipo);

    // conta tarefas pendentes
    Tarefa *t = oc->tarefas;
    while (t) {
        oc->tarefas_pendentes++;
        t = t->prox;
    }

    // escolhe bairro para atendimento (60% de chance de ser o bairro do cidadao)
    if (rand() % 100 < 60) {
        oc->bairro = buscar_bairro_por_id(tabelaHashBairro, oc->cidadao->idBairroDeResidencia, maxHash);
    } else {
        // ou o bairro mais proximo da localizacao atual
        oc->bairro = encontrar_bairro_mais_proximo(oc->cidadao->latitude_atual, oc->cidadao->longitude_atual, tabelaHashBairro, maxHash);
    }

    // se nao encontrou bairro, usa um aleatorio
    if (!oc->bairro) {
        int fallback_id = (rand() % NUM_BAIRROS) + 1;
        oc->bairro = buscar_bairro_por_id(tabelaHashBairro, fallback_id, maxHash);
    }

    // se ainda nao encontrou, cancela ocorrencia
    if (!oc->bairro) {
        oc->cidadao->emOcorrencia = false;
        free(oc);
        return NULL;
    }

    // verifica se os servicos necessarios estao disponiveis no bairro
    // se nao estiverem, busca um bairro proximo que tenha
    if (oc->requer_bombeiro && !servico_disponivel(cidade, oc->bairro->id, BOMBEIRO)) {
        Bairro* novo_bairro = encontrar_bairro_com_servico(
            cidade,
            oc->cidadao->latitude_atual,
            oc->cidadao->longitude_atual,
            BOMBEIRO,
            tabelaHashBairro,
            maxHash
        );
        if (novo_bairro) {
            oc->bairro = novo_bairro;
        }
    }

    // mesma verificacao para hospital
    if (oc->requer_hospital && !servico_disponivel(cidade, oc->bairro->id, HOSPITAL)) {
        Bairro* novo_bairro = encontrar_bairro_com_servico(
            cidade,
            oc->cidadao->latitude_atual,
            oc->cidadao->longitude_atual,
            HOSPITAL,
            tabelaHashBairro,
            maxHash
        );
        if (novo_bairro) {
            oc->bairro = novo_bairro;
        }
    }

    // mesma verificacao para policia
    if (oc->requer_policia && !servico_disponivel(cidade, oc->bairro->id, POLICIA)) {
        Bairro* novo_bairro = encontrar_bairro_com_servico(
            cidade,
            oc->cidadao->latitude_atual,
            oc->cidadao->longitude_atual,
            POLICIA,
            tabelaHashBairro,
            maxHash
        );
        if (novo_bairro) {
            oc->bairro = novo_bairro;
        }
    }

    // define horario de chegada da ocorrencia
    if (ehNova) {
        // para ocorrencias novas: pequeno atraso aleatorio
        int offset = rand() % 5;
        formatarHorario(tempoGlobal + offset, oc->horarioChegada);
    } else {
        // para ocorrencias antigas: tempo passado
        int offset = (rand() % 60) + 1;
        formatarHorario(tempoGlobal - offset, oc->horarioChegada);
    }


        if (oc->gravidade < 4) {
        // Gravidade baixa: bairro de residência
        oc->bairro = buscar_bairro_por_id(tabelaHashBairro,
                                         oc->cidadao->idBairroDeResidencia,
                                         maxHash);
        strcpy(oc->motivoEscolhaBairro, "Bairro residencia (gravidade < 4)");
    } else {
        // Gravidade alta: bairro mais próximo
        oc->bairro = encontrar_bairro_mais_proximo(
            oc->cidadao->latitude_atual,
            oc->cidadao->longitude_atual,
            tabelaHashBairro,
            maxHash
        );
        strcpy(oc->motivoEscolhaBairro, "Bairro mais proximo (gravidade >= 4)");
    }



    // prepara string com servicos para exibicao
    char servicosStr[100] = "";
    obterServicosStr(oc, servicosStr);

    // exibe detalhes da nova ocorrencia
printf("\n\n===============================================================================\n");
printf("                             <<< NOVA OCORRENCIA >>>                            \n");
printf("===============================================================================\n");
printf(" ID ......................: %04d\n", oc->id);
printf(" Gravidade ...............: %d\n", oc->gravidade);
printf(" Servicos ................: %s\n", servicosStr);
printf(" Bairro ..................: %s\n", oc->bairro->nomeDoBairro);
printf(" Descricao ...............: %s\n", oc->descricao);
printf(" Horario chegada .........: %s\n", oc->horarioChegada);
printf(" Presente em ............: %d fila(s)\n", oc->tarefas_pendentes);
printf(" Motivo escolha bairro ............: %s\n\n", oc->motivoEscolhaBairro);
printf("===============================================================================\n");

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


// inicia uma fila vazia
void inicializarDescritorFila(DescritorFila *fila) {
    fila->inicio = NULL; // sem elementos
    fila->fim = NULL;    // sem elementos
    fila->tamanho = 0;   // tamanho zero
}

// adiciona uma ocorrencia na fila com prioridade por gravidade
void filaPrioritaria(DescritorFila *fila, Ocorrencia *oc) {
    // verifica se pode adicionar
    if (!fila || !oc || oc->finalizada) return;

    // cria novo no
    NoFila *novo = (NoFila*)malloc(sizeof(NoFila));
    if (!novo) return;
    novo->ocorrencia = oc;
    novo->prox = NULL;

    // insere no inicio se for mais grave ou fila vazia
    if (!fila->inicio || oc->gravidade > fila->inicio->ocorrencia->gravidade) {
        novo->prox = fila->inicio;
        fila->inicio = novo;
        if (!fila->fim) fila->fim = novo;
    } else {
        // busca posicao correta na fila
        NoFila *atual = fila->inicio;
        while (atual->prox && atual->prox->ocorrencia->gravidade >= oc->gravidade) {
            atual = atual->prox;
        }
        novo->prox = atual->prox;
        atual->prox = novo;
        // atualiza fim se necessario
        if (!novo->prox) fila->fim = novo;
    }
    fila->tamanho++; // incrementa contador
}

// remove a primeira ocorrencia da fila
Ocorrencia* desenfileirar(DescritorFila *fila) {
    // verifica se tem elementos
    if (!fila || !fila->inicio) return NULL;

    // remove o primeiro elemento
    NoFila *temp = fila->inicio;
    Ocorrencia *oc = temp->ocorrencia;
    fila->inicio = temp->prox;
    if (!fila->inicio) fila->fim = NULL; // fila vazia
    free(temp); // libera memoria
    fila->tamanho--; // decrementa contador

    // verifica se a ocorrencia foi finalizada
    if (oc && oc->finalizada) {
        return NULL;
    }
    return oc; // retorna ocorrencia
}

// gera ocorrencias iniciais para comecar a simulacao
int gerarOcorrenciasIniciais(Cidade *cidade, Bairro bairros[], Cidadao *cidadaos[], DescritorFila *filas[][NUM_SERVICOS], int maxHash) {
    int total = 0;
    int count = 10 + rand() % 10; // entre 10 e 19 ocorrencias

    for (int i = 0; i < count; i++) {
        // cria ocorrencia (false = ocorrencia antiga)
        Ocorrencia *oc = criarOcorrenciaAleatoria(cidade, bairros, cidadaos, false, maxHash);

        // verifica se criou corretamente
        if (!oc || !oc->bairro) continue;

        // calcula indice do bairro
        int bairro_idx = oc->bairro->id - 1;
        if (bairro_idx < 0 || bairro_idx >= NUM_BAIRROS) continue;

        // adiciona em cada fila de servico necessario
        Tarefa *t = oc->tarefas;
        while (t) {
            int servico_idx = idxservico(t->servico);
            if (servico_idx >= 0 && servico_idx < NUM_SERVICOS) {
                filaPrioritaria(filas[bairro_idx][servico_idx], oc);
            }
            t = t->prox;
        }
        total++; // conta ocorrencias criadas
    }
    return total; // retorna total de ocorrencias
}



void exibirEstadoInicialFilas(DescritorFila *filas[NUM_BAIRROS][NUM_SERVICOS], Bairro tabelaHashBairro[]) {
 printf("\n\n\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
printf("                <<< ESTADO INICIAL DAS FILAS DE ATENDIMENTO >>>           \n");
printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");

    for (int b = 0; b < NUM_BAIRROS; b++) {
        Bairro *bairro = buscar_bairro_por_id(tabelaHashBairro, b+1, MAXHASH);
        if (!bairro) continue;
        printf("\n\n\n------------------------------ BAIRRO: %s ------------------------------\n", bairro->nomeDoBairro);

        for (int s = 0; s < NUM_SERVICOS; s++) {
            printf("\n\n>>> SERVICO: %s <<<\n", NOME_SERVICOS[s]);
            printf("-------------------------------- FILA --------------------------------\n");
            if (filas[b][s]->tamanho == 0) {
                printf(" [VAZIO] \n");
            } else {
                NoFila *atual = filas[b][s]->inicio;
                while (atual) {
                    printf("| ID: %04d | Hora: %s | Gravidade: %d | Filas: %d |\n",
                          atual->ocorrencia->id,
                          atual->ocorrencia->horarioChegada,
                          atual->ocorrencia->gravidade,
                          atual->ocorrencia->tarefas_pendentes);
                    atual = atual->prox;
                }
            }
            printf("--------------------------------------------------------------------\n");
            printf("Total na fila: %d\n\n", filas[b][s]->tamanho);
        }
    }
      printf("\n\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
}



// exibe o estado das filas apos um ciclo de atendimento
void exibirEstadoPosCiclo(DescritorFila *filas[][NUM_SERVICOS], Bairro tabelaHashBairro[]) {
    char horario_str[6];
    formatarHorario(tempoGlobal + TEMPO_TICK, horario_str); // tempo apos o ciclo
    int total_ocorrencias = 0;
    int ciclo_atual = (tempoGlobal - 720) / TEMPO_TICK; // calcula numero do ciclo


    // Cabeçalho principal
    printf("\n\n\n");
    printf("==============================================================================\n");
    printf("=============   ESTADO DAS FILAS APOS %d CICLO(S) - %-8s   =============\n", ciclo_atual, horario_str);
    printf("==============================================================================\n");
    printf("\n");

    for (int b = 0; b < NUM_BAIRROS; b++) {
        Bairro *bairro = buscar_bairro_por_id(tabelaHashBairro, b+1, MAXHASH);
        if (!bairro) continue;

        // Cabeçalho do bairro
        printf("\n\n\n");
        printf("BAIRRO: %s\n", bairro->nomeDoBairro);
        printf("------------------------------------------------------------------------------\n\n");

        for (int s = 0; s < NUM_SERVICOS; s++) {
            // Cabeçalho do serviço
            printf("  Servico: %s\n", NOME_SERVICOS[s]);
            printf("  --------------------------------------------------------------------------\n");

            if (filas[b][s]->tamanho == 0) {
                printf("  >>> FILA VAZIA <<<\n");
            } else {
                NoFila *atual = filas[b][s]->inicio;
                int pos = 1;

                while (atual) {
                    printf("  %2d. [ID:%04d] Gravidade: %d | Chegada: %s |  Filas: %d\n",
                           pos++,
                           atual->ocorrencia->id,
                           atual->ocorrencia->gravidade,
                           atual->ocorrencia->horarioChegada,
                           atual->ocorrencia->tarefas_pendentes);
                    atual = atual->prox;
                    total_ocorrencias++;
                }
            }

            printf("\n  Total na fila: %d ocorrencia(s)\n\n", filas[b][s]->tamanho);
            printf("\n");
        }

        printf("------------------------------------------------------------------------------\n\n");
    }

    // Rodapé
    printf("==============================================================================\n");
    printf("TOTAL GERAL DE OCORRENCIAS: %d\n", total_ocorrencias);
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

    // Mostrar em qual fila foi collcoado
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
    printf("------------------------------------------------\n");
    if(fila->tamanho == 0) {
        printf("Nenhuma ocorrencia na fila\n");
        return;
    }
    printf("Posicao | Identificacao | Gravidade | Horario registro | Cidadao\n");
    printf("--------|---------------|-----------|------------------|----------------\n");
    NoFila* atual = fila->inicio;
    int pos = 1;
    while(atual) {
        printf("%7d | %13d | %9d | %16s | %s\n",
               pos++,
               atual->ocorrencia->id,
               atual->ocorrencia->gravidade,
               atual->ocorrencia->horarioChegada,
               atual->ocorrencia->cidadao->nomeCidadao);
        atual = atual->prox;
    }
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
            printf("\nFila de atendimento: %s\n", NOME_SERVICOS[s]);
            printf("Total de ocorrencias na fila: %d\n", filas[b][s]->tamanho);
            printf("------------------------------------------------\n");

            if(filas[b][s]->tamanho == 0) {
                printf("Nenhuma ocorrencia na fila\n");
                continue;
            }

            printf("Posicao | Identificacao | Gravidade | Horario chegada | Cidadao\n");
            printf("--------|---------------|-----------|-----------------|----------------\n");

            NoFila* atual = filas[b][s]->inicio;
            int pos = 1;
            while(atual) {
                printf("%7d | %13d | %9d | %15s | %s\n",
                       pos++,
                       atual->ocorrencia->id,
                       atual->ocorrencia->gravidade,
                       atual->ocorrencia->horarioChegada,
                       atual->ocorrencia->cidadao->nomeCidadao);
                atual = atual->prox;
            }
        }
    }
}

void processarCicloAtendimento(DescritorFila *filas[][NUM_SERVICOS],
                               Cidade *cidade,
                               Bairro tabelaHashBairro[],
                               Cidadao* tabelaHashCidadao[],
                               historicoOcorrencias* historico) {

    char horario_str[6];
    formatarHorario(tempoGlobal, horario_str);

    // 1. Exibir cabeçalho do sistema
    exibirCabecalho();

        char ciclo_title[50];
    snprintf(ciclo_title, sizeof(ciclo_title), "CICLO DE ATENDIMENTO %s", horario_str);
    int title_len = strlen(ciclo_title);
    int padding = (80 - title_len) / 2;

    printf("\n\n");
    printf("%*s", padding, "");
    for (int i = 0; i < title_len + 4; i++) printf("═");
    printf("\n%*s  %s  \n", padding, "", ciclo_title);
    printf("%*s", padding, "");
    for (int i = 0; i < title_len + 4; i++) printf("═");
    printf("\n\n\n");

    // 4. Processar e exibir atendimentos em curso
    for (int b = 0; b < NUM_BAIRROS; b++) {
        Bairro *bairro = buscar_bairro_por_id(tabelaHashBairro, b+1, MAXHASH);
        if (!bairro) continue;

        // Cabeçalho do bairro
        printf("\n\n┌────────────────────────────────────────────────────────────────────────────────┐\n");
        printf("│ BAIRRO: %-66s     │\n", bairro->nomeDoBairro);
        printf("└────────────────────────────────────────────────────────────────────────────────┘\n");
        printf("\n");

        for (int s = 0; s < NUM_SERVICOS; s++) {
            Atendimento* at = &atendimentos_em_curso[b][s];

            if (at->ocorrencia && at->ocorrencia->finalizada) {
                memset(at, 0, sizeof(Atendimento));
                continue;
            }

            if (at->ocorrencia) {
                // Exibição de atendimento em andamento
                printf("  ┌──────────────────────────────────────────────────────────────────────┐\n");
                printf("  │ >>> ATENDIMENTO EM ANDAMENTO                                         │\n");
                printf("  ├──────────────────────────────────────────────────────────────────────┤\n");
                printf("  │ Serviço: %-58s  │\n", NOME_SERVICOS[s]);
                printf("  │ Ocorrência: %-54d   │\n", at->ocorrencia->id);
                printf("  │ Cidadão: %-58s  │\n", at->ocorrencia->cidadao->nomeCidadao);
                printf("  │ Tempo restante: %-50d   │\n", at->tempo_restante);
                printf("  └──────────────────────────────────────────────────────────────────────┘\n\n");
                // processa o atendimento
                at->tempo_restante -= TEMPO_TICK;

                if (at->tempo_restante <= 0) {
                    Tarefa* tarefa = at->ocorrencia->tarefas;
                    while (tarefa) {
                        if (tarefa->servico == s && !tarefa->concluida) {
                            tarefa->concluida = true;
                            formatarHorario(tempoGlobal, tarefa->horarioConclusao);
                            at->ocorrencia->tarefas_pendentes--;
                            break;
                        }
                        tarefa = tarefa->prox;
                    }



                    if (at->ocorrencia->tarefas_pendentes == 0) {
        // MARCA COMO FINALIZADA ANTES DE LIBERAR
        at->ocorrencia->finalizada = true;


        registrar_ocorrencia(historico, at->ocorrencia, horario_str);

        if (at->ocorrencia->cidadao) {
            at->ocorrencia->cidadao->emOcorrencia = false;
        }

        // Libera mem com segurança
        Tarefa* t = at->ocorrencia->tarefas;
        while (t) {
            Tarefa* prox = t->prox;
            free(t);
            t = prox;
        }
        free(at->ocorrencia);
        at->ocorrencia = NULL; //
    }

                    // Limpa o atendimento apos processar
                    memset(at, 0, sizeof(Atendimento));
                }
                        } else if (filas[b][s]->inicio != NULL) {
                Ocorrencia* oc = desenfileirar(filas[b][s]);
                if (oc && !oc->finalizada) {
                    at->ocorrencia = oc;
                    at->tempo_restante = TEMPOS_ATENDIMENTO[s];
                    strcpy(at->horario_inicio, horario_str);

                    // Exibio de novo atendimento
                    printf("  ┌──────────────────────────────────────────────────────────────────────┐\n");
                    printf("  │ >>> INICIO DE NOVO ATENDIMENTO                                       │\n");
                    printf("  ├──────────────────────────────────────────────────────────────────────┤\n");
                    printf("  │ Serviço: %-58s  │\n", NOME_SERVICOS[s]);
                    printf("  │ Ocorrência: %-54d   │\n", oc->id);
                    printf("  │ Cidadão: %-58s  │\n", oc->cidadao->nomeCidadao);
                    printf("  │ Tempo previsto: %-50d   │\n", TEMPOS_ATENDIMENTO[s]);
                    printf("  └──────────────────────────────────────────────────────────────────────┘\n\n");
                }
            } else {
                // Exibir quando não tem atendimento
                printf("  ┌──────────────────────────────────────────────────────────────────────┐\n");
                printf("  │ Serviço: %-58s  │\n", NOME_SERVICOS[s]);
                printf("  │ >>> NENHUM ATENDIMENTO EM ANDAMENTO                                  │\n");
                printf("  └──────────────────────────────────────────────────────────────────────┘\n\n");
            }
        }
    }



      Ocorrencia* nova_ocorrencia = NULL;
    for (int i = 0; i < 1; i++) {
        nova_ocorrencia = criarOcorrenciaAleatoria(cidade, tabelaHashBairro, tabelaHashCidadao, true, MAXHASH);
        if (nova_ocorrencia) {
            // COLOCA a ocorrencia nas filas certas SEM EXIBIR
            int bairro_idx = nova_ocorrencia->bairro->id - 1;
            Tarefa* t = nova_ocorrencia->tarefas;
            while (t) {
                int servico_idx = idxservico(t->servico);
                if (servico_idx >= 0 && servico_idx < NUM_SERVICOS) {
                    filaPrioritaria(filas[bairro_idx][servico_idx], nova_ocorrencia);
                }
                t = t->prox;
            }
        }
    }
    printf("\n\n\n");
    // 5. Exibir estado das filas (com a nova ocorrencia ja inserida)
     char titulo[100];
    sprintf(titulo, "ESTADO DAS FILAS APOS O CICLO %s", horario_str);
    exibirEstadoPosCiclo(filas, tabelaHashBairro);

    // 6. So AQUI EXIBIR A NOVA OCORReNCIA
    if (nova_ocorrencia) {
        printf("\n[ NOVA OCORRÊNCIA GERADA E INSERIDA NA FILA ]\n");
        exibirOcorrenciaDetalhada(nova_ocorrencia);
    }

    // 7. Avançar o tempo global
    tempoGlobal += TEMPO_TICK;
}
