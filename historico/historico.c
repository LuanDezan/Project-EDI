
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include"../constantes/constantes.h"








/* Estrutura do histórico como pilha
typedef struct {
    Ocorrencia *topo;            // Topo da pilha
    int quantidade;              // Contador de ocorrências
    int proximo_id;              // Auto-incremento para IDs
} historicoOcorrencias;
*/
// Inicializa o histórico
void inicializar_historico(historicoOcorrencias *h) {
    h->topo = NULL;             // Nenhuma ocorrência registrada inicialmente
    h->quantidade = 0;          // Começa com zero ocorrências
    h->proximo_id = 1;          // IDs começam a partir de 1
}

// Registra nova ocorrência (push)
bool registrar_ocorrencia(historicoOcorrencias *h, Ocorrencia *oc, const char *horarioConclusao) {
    if (!h || !oc || !horarioConclusao) return false;

    Ocorrencia *nova = malloc(sizeof(Ocorrencia));
    if (!nova) return false;

    nova->id = oc->id;
    strncpy(nova->horarioChegada, oc->horarioChegada, sizeof(nova->horarioChegada));
    strncpy(nova->horarioAtendimento, horarioConclusao, sizeof(nova->horarioAtendimento));
    nova->gravidade = oc->gravidade;
    nova->tipo = oc->tipo;
    nova->bairro = oc->bairro;  // Ponteiro para bairro

    // Não copia tarefas pois não são necessárias no histórico
    nova->tarefas = NULL;
    nova->finalizada = true;  // Marca como finalizada

    // Insere no histórico
    nova->prox = h->topo;
    h->topo = nova;
    h->quantidade++;

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
void imprime_historico(const historicoOcorrencias *h) {
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





void imprimir_historico(const historicoOcorrencias* h) {
    printf("\n\n=== HISTÓRICO DE ATENDIMENTOS ENTRE 12h-18h ===\n");
    printf("=========================================================\n");
    printf("%-5s | %-15s | %-8s | %-11s | %-8s \n",
           "ID", "Bairro", "Chegada", "Atendimento", "Grav");
    printf("---------------------------------------------------------\n");

    Ocorrencia* atual = h->topo;
    while(atual) {
        printf("%-5d | %-15s | %-8s | %-11s | %-8d \n",
               atual->id,
               atual->bairro->nomeDoBairro,
               atual->horarioChegada,
               atual->horarioAtendimento,
               atual->gravidade,
               atual->descricao);
        atual = atual->prox;
    }
    printf("===========================================================\n");
}
