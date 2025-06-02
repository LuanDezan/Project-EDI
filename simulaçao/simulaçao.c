#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define MAXHASH      20
#define NUM_BAIRROS  4
#define NUM_SERVICOS 4
#define TEMPO_TICK   10




// estrutura que representa um bairro, com id, nome e ponteiro para próximo bairro na lista encadeada
typedef struct Bairro {

    int id;
    char nomeDoBairro[30];
    struct Bairro *prox;
} Bairro;



// estrutura que representa uma ocorrência de emergência
// contém id, horários de chegada e atendimento, gravidade (1 a 5),
// tipo do serviço (hospital, polícia etc), ponteiro para bairro e próxima ocorrência
typedef struct Ocorrencia {

    int id;
    char horarioChegada[6];     // "HH:MM"
    char horarioAtendimento[6]; // "HH:MM"
    int gravidade;
    int tipo;                   // 1..4
    Bairro *bairro;
    struct Ocorrencia *prox;
} Ocorrencia;

// descritor para a fila de ocorrências, com ponteiros para início, fim e tamanho da fila
typedef struct {
    Ocorrencia *inicio;
    Ocorrencia *fim;
    int tamanho;
} DescritorFila;

time_t tempoGlobal = 720;

void formatarHorario(int minutos, char *destino) {
    int horas = minutos / 60;
    int mins  = minutos % 60;
    sprintf(destino, "%02d:%02d", horas, mins);
}

void inicializarDescritorFila(DescritorFila *fila) {
    fila->fim     = NULL;
    fila->inicio  = NULL;
    fila->tamanho = 0;
}

void filaAtendimento(DescritorFila *fila, Ocorrencia *nova)
{
    if (fila->inicio == NULL || nova->gravidade > fila->inicio->gravidade)
    {
        nova->prox   = fila->inicio;
        fila->inicio = nova;

        if (fila->tamanho == 0)
        {
            fila->fim = nova;
        }

        fila->tamanho++;

    } else {
        Ocorrencia *atual = fila->inicio;

        while (atual->prox && nova->gravidade <= atual->prox->gravidade)
        {
            atual = atual->prox;
        }

        nova->prox   = atual->prox;
        atual->prox  = nova;

        if (nova->prox == NULL)
        {
            fila->fim = nova;
        }

        fila->tamanho++;

    }
}




void enviarOcorrencia(DescritorFila *filas[], Ocorrencia *nova) {
    filaAtendimento(filas[nova->tipo - 1], nova);
}




Ocorrencia *criarOcorrenciaAleatoria(Bairro *tabelaHashBairro, int ehOcorrenciaNova) {
    Ocorrencia *nova = malloc(sizeof(Ocorrencia));
    if (nova == NULL) {
        return NULL;
    }

    nova->id        = rand() % 10000;
    nova->gravidade = rand() % 5 + 1;
    nova->tipo      = rand() % NUM_SERVICOS + 1;

    int chegada;
    if (ehOcorrenciaNova) {
        chegada = tempoGlobal + (rand() % TEMPO_TICK);
    } else {
        chegada = 720 - (rand() % 10 + 1);
    }
    formatarHorario(chegada, nova->horarioChegada);

    int idBairro = (rand() % NUM_BAIRROS) + 1;
    int chave    = idBairro % MAXHASH;

    Bairro *b = tabelaHashBairro[chave].prox;
    while (b && b->id != idBairro) {
        b = b->prox;
    }

    if (b == NULL) {
        free(nova);
        return NULL;
    }

    nova->bairro = b;
    nova->prox   = NULL;
    return nova;
}






void mostrarFilaAtual(DescritorFila *fila, const char *nomeFila) {
    printf("\n----------------- FILA: %s -----------------\n", nomeFila);
    printf("ID    | CHEGADA | GRAVIDADE | BAIRRO\n");
    printf("------------------------------------\n");

    Ocorrencia *atual = fila->inicio;
    int cont = 0;
    while (atual != NULL) {
        printf("%04d  | %s    | %d         | %s\n",
               atual->id,
               atual->horarioChegada,
               atual->gravidade,
               atual->bairro->nomeDoBairro);
        atual = atual->prox;
        cont++;
    }
    if (cont == 0) {
        printf("FILA VAZIA\n");
    }
  if (cont == 1) {
    printf("\ntotal: 1 ocorrencia \n\n\n");
} else {
    printf("\ntotal: %d ocorrencias \n\n\n",
           cont);
}
}




void processarFilas(DescritorFila *filas[], int tempoRestante[], Bairro *tabelaHashBairro, Ocorrencia *emAtendimento[]) {
    int tempoFinal = 18 * 60;

    if (tempoGlobal < 720) {
        tempoGlobal = 720;
    }

    // gerar ocorrencias pre-existentes para o ciclo 0 (chegaram antes das 12:00)
    if (tempoGlobal == 720) {
        int ocorrenciasPreExistentes = 3 + rand() % 5;
        for (int i = 0; i < ocorrenciasPreExistentes; i++) {
            Ocorrencia *nova = criarOcorrenciaAleatoria(tabelaHashBairro, 0); // 0 = pre-existente
            if (nova) {
                enviarOcorrencia(filas, nova);
            }
        }

        // mostrar estado das filas antes do atendimento (t = 12:00)
        const char *TIPOS_SERVICO[NUM_SERVICOS] = {"hospital", "policia", "bombeiro", "samu"};
        printf("\n=== estado das filas antes do atendimento ===\n");
        for (int i = 0; i < NUM_SERVICOS; i++) {
            mostrarFilaAtual(filas[i], TIPOS_SERVICO[i]);
        }

        // aviso de inicio de ciclo 0
        printf("\n======== iniciando ciclo de atendimento (t = 12:00) ========\n");
    }

    bool primeiroCiclo = true;

    while (tempoGlobal < tempoFinal) {
        char horarioAtual[6];
        formatarHorario(tempoGlobal, horarioAtual);

        const char *TIPOS_SERVICO[NUM_SERVICOS]       = {"hospital", "policia", "bombeiro", "samu"};
        const int   TEMPOS_ATENDIMENTO[NUM_SERVICOS] = {20, 10, 8, 15};

        int sistemaVazio = 1;

        // so imprime “novo ciclo” a partir do segundo passo em diante
        if (!primeiroCiclo) {
            printf("\n\n================= novo ciclo de atendimento =================\n");
            printf(">>> hora atual: %s\n", horarioAtual);
        } else {
            // no primeiro ciclo (tempoGlobal == 720), ja foi emitido “iniciando ciclo”
            printf(">>> hora atual: %s\n", horarioAtual);
            primeiroCiclo = false;
        }

        // 1) processar atendimento em cada servico
        for (int i = 0; i < NUM_SERVICOS; i++) {
            printf("\n %-10s| hora: %s\n", TIPOS_SERVICO[i], horarioAtual);

            // se nao ha ninguem em atendimento, tentar buscar alguem na fila
            if (emAtendimento[i] == NULL && filas[i]->inicio != NULL) {
                Ocorrencia *ant = NULL;
                Ocorrencia *aux = filas[i]->inicio;

                // varre a lista ate achar a primeira cuja chegada <= tempoGlobal
                while (aux) {
                    int hChegada, mChegada;
                    sscanf(aux->horarioChegada, "%2d:%2d", &hChegada, &mChegada);
                    int minutosChegada = hChegada * 60 + mChegada;

                    if (minutosChegada <= tempoGlobal) {
                        // achou ocorrencia disponivel para atendimento
                        if (ant == NULL) {
                            // e o proprio inicio da lista
                            filas[i]->inicio = aux->prox;
                        } else {
                            ant->prox = aux->prox;
                        }
                        if (aux == filas[i]->fim) {
                            filas[i]->fim = ant; // se era fim, atualiza ponteiro de fim
                        }
                        aux->prox = NULL;           // desconecta da lista
                        emAtendimento[i] = aux;     // manda para atendimento
                        tempoRestante[i] = TEMPOS_ATENDIMENTO[i];
                        strcpy(emAtendimento[i]->horarioAtendimento, horarioAtual);
                        filas[i]->tamanho--;
                        break;
                    }
                    ant = aux;
                    aux = aux->prox;
                }
            }

            // 2) se ha alguem em atendimento, diminui tempo e exibe status
            if (emAtendimento[i] != NULL) {
                int antes = tempoRestante[i];
                tempoRestante[i] -= TEMPO_TICK;

                int hChegada, mChegada, hAtendimento, mAtendimento;
                sscanf(emAtendimento[i]->horarioChegada,   "%2d:%2d", &hChegada,     &mChegada);
                sscanf(emAtendimento[i]->horarioAtendimento,"%2d:%2d", &hAtendimento,&mAtendimento);

                int minutosChegada     = hChegada * 60 + mChegada;
                int minutosAtendimento = hAtendimento * 60 + mAtendimento;
                int espera = minutosAtendimento - minutosChegada;
                if (espera < 0) espera = 0;

                // exibir dados sempre, mesmo no tick de conclusao
                printf("  -> id: %04d\n     gravidade: %d\n     bairro: %s\n     chegada: %s\n     espera: %d min\n",
                       emAtendimento[i]->id,
                       emAtendimento[i]->gravidade,
                       emAtendimento[i]->bairro->nomeDoBairro,
                       emAtendimento[i]->horarioChegada,
                       espera);

                if (antes > TEMPO_TICK) {
                    // ainda em atendimento
                    printf("     em atendimento... restante: %d min\n\n", tempoRestante[i]);
                    printf("     -----------------------------------\n");
                } else {
                    // atendimento concluido neste tick
                    int tempoTermino = tempoGlobal + TEMPO_TICK;
                    char horarioTermino[6];
                    formatarHorario(tempoTermino, horarioTermino);

                    printf("     atendimento concluido\n");
                    printf("     -----------------------------------\n");
                    printf(">> atendimento encerrado para ocorrencia %04d | bairro: %s | as %s\n\n",
                           emAtendimento[i]->id,
                           emAtendimento[i]->bairro->nomeDoBairro,
                           horarioTermino);

                    free(emAtendimento[i]);
                    emAtendimento[i] = NULL;
                }

                sistemaVazio = 0;
            } else {
                // nao ha ninguem em atendimento neste servico, mas ainda pode haver fila
                if (filas[i]->tamanho > 0) {
                    sistemaVazio = 0;
                }
                printf("  -> sem ocorrencias nesta rodada\n\n");
                printf("     -----------------------------------\n");
            }
        }

        // 3) gerar novas ocorrencias durante este ciclo
        int novasOcorrencias = 1 + rand() % 3;
        printf("\n--> novas ocorrencias geradas neste ciclo: %d\n", novasOcorrencias);
        for (int i = 0; i < novasOcorrencias; i++) {
            Ocorrencia *nova = criarOcorrenciaAleatoria(tabelaHashBairro, 1); // 1 = nova
            if (nova) {
                printf("  -> id: %04d | tipo: %s | gravidade: %d | bairro: %s | chegada: %s\n",
                       nova->id,
                       TIPOS_SERVICO[nova->tipo - 1],
                       nova->gravidade,
                       nova->bairro->nomeDoBairro,
                       nova->horarioChegada);
                enviarOcorrencia(filas, nova);
            }
        }

        // 4) avancar o tempo
        tempoGlobal += TEMPO_TICK;
        printf("\n\n================ fim do ciclo (t = %02d:%02d) ================\n\n",
               tempoGlobal / 60, tempoGlobal % 60);

        // 5) mostrar estado das filas ao fim do ciclo
        printf("\n\n============== estado das filas apos o ciclo ==============\n\n");
        for (int i = 0; i < NUM_SERVICOS; i++) {
            printf("\n------ fila: %s ------\n", TIPOS_SERVICO[i]);
            printf("id   | chegada | grav | bairro\n");
            printf("-------------------------------\n");

            int emAtendendo = (emAtendimento[i] != NULL);

            if (filas[i]->tamanho == 0) {
                if (emAtendendo) {
                    printf("em atendimento...\n");
                } else {
                    printf("fila vazia\n");
                }
            } else {
                Ocorrencia *aux = filas[i]->inicio;
                while (aux) {
                    printf("%04d |   %s   |  %d   | %s\n",
                           aux->id, aux->horarioChegada, aux->gravidade, aux->bairro->nomeDoBairro);
                    aux = aux->prox;
                }
            }

            // sem operador ternario: usar if-else para determinar o status
            char status[30];
            if (emAtendendo) {
                strcpy(status, "(ainda atendendo)");
            } else {
                if (tempoGlobal >= tempoFinal) {
                    strcpy(status, "(fim do horario)");
                } else {
                    strcpy(status, "(pronto para proximo atendimento)");
                }
            }

if (filas[i]->tamanho == 1) {
    printf("\ntotal: 1 ocorrencia | em atendimento: %d %s\n\n\n",
           emAtendendo,
           status);
} else {
    printf("\ntotal: %d ocorrencias | em atendimento: %d %s\n\n\n",
           filas[i]->tamanho,
           emAtendendo,
           status);
}
        }
        if (sistemaVazio) {
            printf("todas filas vazias. fim da simulacao.\n");
            break;
        }

        if (tempoGlobal >= tempoFinal) {
            printf("\nsimulacao finalizada. horario limite (18:00) atingido.\n");
        }
    }
}


