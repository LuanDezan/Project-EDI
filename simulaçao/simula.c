#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 40
#define HASH_SIZE 20
#define MAXHASH 20
#define HASH 20

#include "..\constantes\constantes.h"
#include "..\Cidadao\Cidadao.h"
#include "..\SAMU\Samu.h"
#include "..\bairro\Bairro.h"
#include "..\hospital\HospitalComHash.h"
#include "..\policia\PoliciaComHash.h"

// struct que representa uma ocorrência com informações como id, horário, gravidade, tipo, bairro e ponteiro para a próxima
typedef struct Ocorrencia {
    int id;
    char horario[6];
    int gravidade;
    int tipo;
    struct Bairro *bairro;
    struct Ocorrencia *prox;
} Ocorrencia;

// struct que representa o descritor da fila de ocorrências
typedef struct {
    Ocorrencia *inicio;
    Ocorrencia *fim;
    int tamanho;
} DescritorFila;

// inicializa o descritor da fila com valores nulos e tamanho zero
void inicializarDescritorFila(DescritorFila *fila) {
    fila->fim = NULL;
    fila->inicio = NULL;
    fila->tamanho = 0;
}

// variável global que representa os minutos totais desde 00:00, iniciando em 12:00 (720 minutos)
int minutosTotais = 720;

// formata os minutos totais em horário hh:mm e salva na string destino
void formatarHorario(int minutos, char *destino) {
    int horas = minutos / 60;
    int mins = minutos % 60;
    sprintf(destino, "%02d:%02d", horas, mins);
}

// insere uma ocorrência na fila de atendimento de acordo com a gravidade
void filaAtendimento(DescritorFila *fila, Ocorrencia *nova) {
    if (!fila->inicio || nova->gravidade > fila->inicio->gravidade) {
        // insere no início se a fila estiver vazia ou se a gravidade for maior
        nova->prox = fila->inicio;
        fila->inicio = nova;

        if (fila->tamanho == 0) {
            fila->fim = nova;
        }
    } else {
        // percorre a fila para encontrar o local correto
        Ocorrencia *atual = fila->inicio;
        while (atual->prox && nova->gravidade <= atual->prox->gravidade) {
            atual = atual->prox;
        }

        // insere a nova ocorrência na posição correta
        nova->prox = atual->prox;
        atual->prox = nova;

        // atualiza o ponteiro do fim da fila, se necessário
        if (!nova->prox) {
            fila->fim = nova;
        }
    }
    fila->tamanho++;
}

// envia uma ocorrência para a fila de atendimento
void enviarOcorrencia(DescritorFila *fila, Ocorrencia *oc) {
    filaAtendimento(fila, oc);
}

// gera uma nova ocorrência aleatória e a adiciona na fila
void gerarOcorrencia(DescritorFila *fila, Bairro *tabelaHashBairro) {
    Ocorrencia *nova = malloc(sizeof(Ocorrencia));

    if (!nova) {
        printf("erro de alocação na ocorrência.\n");
        return;
    }

    // gera dados aleatórios para a ocorrência
    nova->id = rand() % 10000;
    nova->gravidade = rand() % 5 + 1;
    nova->tipo = rand() % 5 + 1;
    formatarHorario(minutosTotais, nova->horario);

    // seleciona um bairro aleatório
    int idBairroAleatorio = (rand() % 10) * 100;
    int chave = hashBairro(idBairroAleatorio);
    Bairro *b = tabelaHashBairro[chave].prox;

    // procura o bairro com o id correspondente
    while (b && b->id != idBairroAleatorio) {
        b = b->prox;
    }

    if (!b) {
        printf("erro: bairro %d não encontrado. ocorrência cancelada.\n", idBairroAleatorio);
        free(nova);
        return;
    }

    nova->bairro = b;
    nova->prox = NULL;

    // define o tipo de emergência como texto
    char tipoTexto[20];
    if (nova->tipo == 1) {
        strcpy(tipoTexto, "hospital");
    } else if (nova->tipo == 2) {
        strcpy(tipoTexto, "polícia");
    } else if (nova->tipo == 3) {
        strcpy(tipoTexto, "bombeiro");
    } else if (nova->tipo == 4) {
        strcpy(tipoTexto, "samu");
    } else {
        strcpy(tipoTexto, "outro");
    }

    // imprime os dados da ocorrência
    printf("ocorrência gerada:\n");
    printf("id: %d | horário: %s | gravidade: %d | tipo: %d (%s) | bairro: %s\n", 
           nova->id, nova->horario, nova->gravidade, nova->tipo, tipoTexto, b->nomeDoBairro);

    // envia a ocorrência para a fila
    enviarOcorrencia(fila, nova);

    // atualiza o horário com base na gravidade da ocorrência
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

// mostra todas as ocorrências presentes na fila
void mostrarFila(DescritorFila *fila) {
    Ocorrencia *temp = fila->inicio;

    printf("\n--- fila de atendimento (%d ocorrência(s)) ---\n", fila->tamanho);

    while (temp) {
        char tipoTexto[20];

        if (temp->tipo == 1) {
            strcpy(tipoTexto, "hospital");
        } else if (temp->tipo == 2) {
            strcpy(tipoTexto, "polícia");
        } else if (temp->tipo == 3) {
            strcpy(tipoTexto, "bombeiro");
        } else if (temp->tipo == 4) {
            strcpy(tipoTexto, "samu");
        } else {
            strcpy(tipoTexto, "outro");
        }

        printf("id: %d | horário: %s | gravidade: %d | tipo: %d (%s) | bairro: %s\n", 
               temp->id, temp->horario, temp->gravidade, temp->tipo, tipoTexto, temp->bairro->nomeDoBairro);
        temp = temp->prox;
    }
}

// libera a memória alocada para a fila
void limparFila(DescritorFila *fila) {
    Ocorrencia *atual = fila->inicio;

    while (atual) {
        Ocorrencia *prox = atual->prox;
        free(atual);
        atual = prox;
    }

    fila->inicio = NULL;
    fila->fim = NULL;
    fila->tamanho = 0;
}
