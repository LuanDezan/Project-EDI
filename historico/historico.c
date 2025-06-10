#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include"../simulacao/ocorrencia.c"

// Estrutura do histórico como pilha
typedef struct {
    Ocorrencia *topo;            // Topo da pilha
    int quantidade;              // Contador de ocorrências
    int proximo_id;              // Auto-incremento para IDs
} historicoOcorrencias;

// Inicializa o histórico
void inicializar_historico(historicoOcorrencias *h) {
    h->topo = NULL;             // Nenhuma ocorrência registrada inicialmente
    h->quantidade = 0;          // Começa com zero ocorrências
    h->proximo_id = 1;          // IDs começam a partir de 1
}

// Registra nova ocorrência (push)
bool registrar_ocorrencia(historicoOcorrencias *h, const char *h_chegada, const char *h_atendimento, int gravidade, int tipo, Bairro *bairro) {

    // Verifica parâmetros inválidos
    if (!h || !h_chegada || !h_atendimento || !bairro) {
        return false;
    }

    // Aloca memória para a nova ocorrência
    Ocorrencia *nova = malloc(sizeof(Ocorrencia));
    if (!nova) {
        return false;
    }

    // Preenche os dados
    nova->id = h->proximo_id++;
    strncpy(nova->horarioChegada, h_chegada, 6);
    strncpy(nova->horarioAtendimento, h_atendimento, 6);
    nova->gravidade = gravidade;
    nova->tipo = tipo;
    nova->bairro = bairro;

    // Insere a nova ocorrência no topo da pilha
    nova->prox = h->topo;
    h->topo = nova;
    h->quantidade++;    // Incrementa o contador de ocorrências

    return true;
}

// Consulta a última ocorrência (peek)
const Ocorrencia* consultar_ultima(const historicoOcorrencias *h) {
    return h->topo;
}

// Remove a última ocorrência (pop)
bool remover_ultima(historicoOcorrencias *h) {

    // Verifica histórico vazio ou inválido
    if (!h || !h->topo) {
        return false;
    }

    Ocorrencia *remover = h->topo;      // Armazena o ponteiro do topo
    h->topo = remover->prox;            // Atualiza o topo para o próximo
    free(remover);                      // Libera memória da ocorrência removida
    h->quantidade--;                    // Decrementa contador

    return true;
}

// Imprime o histórico completo
void imprimir_historico(const historicoOcorrencias *h) {
    if (!h) return;

    printf("Historico de Ocorrencias (%d registros):\n", h->quantidade);
    printf("============================================\n");

    Ocorrencia *atual = h->topo;
    while (atual) {
        printf("ID: %d\n", atual->id);
        printf("Bairro: %s\n", atual->bairro->nomeDoBairro);
        printf("Chegada: %s | Atendimento: %s\n",
               atual->horarioChegada, atual->horarioAtendimento);
        printf("Gravidade: %d\n\n",
               atual->gravidade);

        atual = atual->prox;
    }
    printf("============================================\n");
}

// Libera toda a memória do histórico
void destruir_historico(historicoOcorrencias *h) {
    if (!h) return;

    while (h->topo) {
        remover_ultima(h);  // Remove do topo até que esteja vazio
    }
    h->quantidade = 0;      // Reseta o contador
    h->proximo_id = 1;      // Reseta o ID
}

