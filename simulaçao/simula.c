#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



//TODO colocar o vetor de nome de bairros no arquivo de constantes
#define MAX 40
#define HASH_SIZE 20
#define MAXHASH 20
#define HASH 20


#include"..\constantes\constantes.h"
#include"..\Cidadao\Cidadao.c"
#include"..\SAMU\Samu.c"
#include"..\bairro\Bairro.c"
#include"..\hash\hash.c"
#include"..\hospital\HospitalComHash.c"
#include"..\policia\PoliciaComHash.c"
#include"..\constantes\constantes.h"




typedef struct Ocorrencia {
    int id;
    char horario[6];
    int gravidade;
    int tipo;
    struct Bairro *bairro;
    struct Ocorrencia *prox;
} Ocorrencia;




int minutosTotais = 720; // 12:00

void formatarHorario(int minutos, char *destino) {
    int horas = minutos / 60;
    int mins = minutos % 60;
    sprintf(destino, "%02d:%02d", horas, mins);
}

Ocorrencia *inicioFila = NULL;

void filaAtendimento(Ocorrencia *nova) {
    if (!inicioFila || nova->gravidade > inicioFila->gravidade) {
        nova->prox = inicioFila;
        inicioFila = nova;
        return;
    }
    Ocorrencia *atual = inicioFila;
    while (atual->prox && nova->gravidade <= atual->prox->gravidade) {
        atual = atual->prox;
    }
    nova->prox = atual->prox;
    atual->prox = nova;
}

void enviarOcorrencia(Ocorrencia *oc) {
    filaAtendimento(oc);
}

void gerarOcorrencia(Bairro *tabelaHashBairro) {
    Ocorrencia *nova = malloc(sizeof(Ocorrencia));
    if (!nova) {
        printf("Erro de alocação na ocorrência.\n");
        return;
    }

    nova->id = rand() % 10000;
    nova->gravidade = rand() % 5 + 1;
    nova->tipo = rand() % 5 + 1;
    formatarHorario(minutosTotais, nova->horario);

int idBairroAleatorio = (rand() % 10) * 100;
    int chave = hashBairro(idBairroAleatorio);
    Bairro *b = tabelaHashBairro[chave].prox;

    while (b && b->id != idBairroAleatorio) {
        b = b->prox;
    }

    if (!b) {
        printf("Erro: bairro %d não encontrado. Ocorrência cancelada.\n", idBairroAleatorio);
        free(nova);
        return;
    }

    nova->bairro = b;
    nova->prox = NULL;

    char tipoTexto[20];
    if (nova->tipo == 1) {
        strcpy(tipoTexto, "Hospital");
    } else if (nova->tipo == 2) {
        strcpy(tipoTexto, "Polícia");
    } else if (nova->tipo == 3) {
        strcpy(tipoTexto, "Bombeiro");
    } else if (nova->tipo == 4) {
        strcpy(tipoTexto, "SAMU");
    } else {
        strcpy(tipoTexto, "Outro");
    }

    printf("Ocorrência gerada:\n");
    printf("ID: %d | Horário: %s | Gravidade: %d | Tipo: %d (%s) | Bairro: %s\n",
           nova->id, nova->horario, nova->gravidade, nova->tipo, tipoTexto, b->nomeDoBairro);

    enviarOcorrencia(nova);

    if (nova->gravidade == 1) {
        minutosTotais += rand() % 4 + 5;
    } else if (nova->gravidade == 2) {
        minutosTotais += rand() % 4 + 4;
    } else if (nova->gravidade == 3) {
        minutosTotais += rand() % 3 + 3;
    } else if (nova->gravidade == 4) {
        minutosTotais += rand() % 2 + 2;
    } else if (nova->gravidade == 5) {
        minutosTotais += rand() % 2 + 1;
    }
}

void mostrarFila() {
    Ocorrencia *temp = inicioFila;
    printf("\n--- FILA DE ATENDIMENTO ---\n");
    while (temp) {
        char tipoTexto[20];
        if (temp->tipo == 1) {
            strcpy(tipoTexto, "Hospital");
        } else if (temp->tipo == 2) {
            strcpy(tipoTexto, "Polícia");
        } else if (temp->tipo == 3) {
            strcpy(tipoTexto, "Bombeiro");
        } else if (temp->tipo == 4) {
            strcpy(tipoTexto, "SAMU");
        } else {
            strcpy(tipoTexto, "Outro");
        }

        printf("ID: %d | Horário: %s | Gravidade: %d | Tipo: %d (%s) | Bairro: %s\n",
               temp->id, temp->horario, temp->gravidade, temp->tipo, tipoTexto, temp->bairro->nomeDoBairro);
        temp = temp->prox;
    }
}

