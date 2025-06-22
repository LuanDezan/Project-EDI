#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include"../constantes/constantes.h"


// Nó da árvore AVL armazenando ponteiro para Ocorrencia
typedef struct noAVL {
    Ocorrencia *info;
    struct noAVL *esq, *dir;
} noAVL;

void inicializarAVL(noAVL **raiz){
    *raiz = NULL;
}

// Função para gerar chave de ordenação única
int gerarChave(Ocorrencia *oc) {
    // Garante que IDs menores tenham prioridade quando gravidades forem iguais
    return oc->gravidade * 1000000 + (1000000 - oc->id);
}

// Função recursiva que calcula a altura da árvore
int altura(noAVL *arvore) {
    if (arvore == NULL)
        return -1;
    int he = altura(arvore->esq); // Altura esquerda
    int hd = altura(arvore->dir); // Altura direita
    return (he > hd ? he : hd) + 1; // Verifica a maior altura e adiciona 1
}

// Calcula o fator de balanceamento (altura direita - altura esquerda)
int fatorBalanceamento(noAVL *arvore) {
    return altura(arvore->dir) - altura(arvore->esq);
}

// Rotação para direita
void rotacionaDir(noAVL **arvore) {
    noAVL *aux = (*arvore)->esq;
    (*arvore)->esq = aux->dir;
    aux->dir = *arvore;
    *arvore = aux;
}

// Rotação para esquerda
void rotacionaEsq(noAVL **arvore) {
    noAVL *aux = (*arvore)->dir;
    (*arvore)->dir = aux->esq;
    aux->esq = *arvore;
    *arvore = aux;
}

// Função recursiva de inserção na AVL com balanceamento
bool inserirRec(noAVL **arvore, Ocorrencia *ocorrencia) {
    if(ocorrencia == NULL) return false;

    int FB;
    int chave = gerarChave(ocorrencia);

    if (*arvore == NULL) {
        noAVL *novo = (noAVL *)malloc(sizeof(noAVL));
        if (novo == NULL)
            return false;
        novo->info = ocorrencia;
        novo->esq = novo->dir = NULL;
        *arvore = novo;
    } else {
        int chaveAtual = gerarChave((*arvore)->info);
        if (chave < chaveAtual)
            inserirRec(&(*arvore)->esq, ocorrencia);
        else if (chave > chaveAtual)
            inserirRec(&(*arvore)->dir, ocorrencia);
        else {
            return 0;
        }

        FB = fatorBalanceamento(*arvore);

        if (FB >= 2) {
            if (fatorBalanceamento((*arvore)->dir) > 0)
                rotacionaEsq(arvore);
            else {
                rotacionaDir(&(*arvore)->dir);
                rotacionaEsq(arvore);
            }
        } else if (FB <= -2) {
            if (fatorBalanceamento((*arvore)->esq) < 0)
                rotacionaDir(arvore);
            else {
                rotacionaEsq(&(*arvore)->esq);
                rotacionaDir(arvore);
            }
        }
    }
    return true;
}

// Retorna o nó com o menor valor (usado na remoção)
noAVL* minimo(noAVL *arvore) {
    noAVL *atual = arvore;
    while (atual->esq != NULL)
        atual = atual->esq;
    return atual;
}

// Função de remoção usando gravidade e ID como critério
noAVL* removerRec(noAVL *arvore, Ocorrencia *ocorrencia) {
    if (arvore == NULL)
        return NULL;

    // Gera a chave composta
    int chaveRemover = gerarChave(ocorrencia);
    int chaveAtual = gerarChave(arvore->info);

    // Compara a atual com a que vai ser removida
    if (chaveRemover < chaveAtual) {
        arvore->esq = removerRec(arvore->esq, ocorrencia);
    } else if (chaveRemover > chaveAtual) {
        arvore->dir = removerRec(arvore->dir, ocorrencia);
    } else {
        if (arvore->esq == NULL || arvore->dir == NULL) {
            noAVL *temp = (arvore->esq != NULL) ? arvore->esq : arvore->dir;
            free(arvore->info);
            free(arvore);
            return temp;
        } else {
            noAVL *temp = minimo(arvore->dir);
            *(arvore->info) = *(temp->info);
            arvore->dir = removerRec(arvore->dir, temp->info);
        }
    }

    // Ajuste de balanceamento após remoção
    int FB = fatorBalanceamento(arvore);

    if (FB >= 2) {
        if (fatorBalanceamento(arvore->dir) >= 0)
            rotacionaEsq(&arvore);
        else {
            rotacionaDir(&arvore->dir);
            rotacionaEsq(&arvore);
        }
    } else if (FB <= -2) {
        if (fatorBalanceamento(arvore->esq) <= 0)
            rotacionaDir(&arvore);
        else {
            rotacionaEsq(&arvore->esq);
            rotacionaDir(&arvore);
        }
    }

    return arvore;
}

Ocorrencia* proximaChamada(noAVL *arvore) {
    if (arvore == NULL) return NULL;

    noAVL *atual = arvore;
    while (atual->dir != NULL)
        atual = atual->dir;
    return atual->info;
}

// Percorre em ordem (esquerda, raiz, direita) - mostra gravidades em ordem crescente
void inOrdem(noAVL *arvore) {
    if (arvore != NULL) {
        inOrdem(arvore->esq);
        printf("ID: %d | Gravidade: %d | Chegada: %s | Atendimento: %s | Tipo: %d\n", arvore->info->id, arvore->info->gravidade, arvore->info->horarioChegada, arvore->info->horarioAtendimento, arvore->info->tipo);
        inOrdem(arvore->dir);
    }
}

// Percorre em ordem decrescente de gravidade
void inOrdemDecrescente(noAVL *arvore) {
    if (arvore != NULL) {
        inOrdemDecrescente(arvore->dir);
        printf("ID: %d | Gravidade: %d | Chegada: %s | Atendimento: %s | Tipo: %d\n", arvore->info->id, arvore->info->gravidade, arvore->info->horarioChegada, arvore->info->horarioAtendimento, arvore->info->tipo);
        inOrdemDecrescente(arvore->esq);
    }
}

// Função para liberar toda a árvore
void liberarArvoreAVL(noAVL **arvore) {
    if (*arvore == NULL) return;

    liberarArvoreAVL(&(*arvore)->esq);
    liberarArvoreAVL(&(*arvore)->dir);

    free((*arvore)->info);
    free(*arvore);
    *arvore = NULL;
}

// =================TESTE=================
int main() {
    noAVL *raiz;

    inicializarAVL(&raiz);

    // Criando algumas ocorrências
    Ocorrencia *oc1 = (Ocorrencia *)malloc(sizeof(Ocorrencia));
    oc1->id = 1; oc1->gravidade = 2; strcpy(oc1->horarioChegada, "08:00"); strcpy(oc1->horarioAtendimento, "08:30"); oc1->tipo = 1;
    Ocorrencia *oc2 = (Ocorrencia *)malloc(sizeof(Ocorrencia));
    oc2->id = 2; oc2->gravidade = 4; strcpy(oc2->horarioChegada, "09:00"); strcpy(oc2->horarioAtendimento, "09:10"); oc2->tipo = 2;
    Ocorrencia *oc3 = (Ocorrencia *)malloc(sizeof(Ocorrencia));
    oc3->id = 3; oc3->gravidade = 1; strcpy(oc3->horarioChegada, "09:45"); strcpy(oc3->horarioAtendimento, "10:10"); oc3->tipo = 1;
    Ocorrencia *oc4 = (Ocorrencia *)malloc(sizeof(Ocorrencia));
    oc4->id = 4; oc4->gravidade = 5; strcpy(oc4->horarioChegada, "10:20"); strcpy(oc4->horarioAtendimento, "10:40"); oc4->tipo = 3;
    Ocorrencia *oc5 = (Ocorrencia *)malloc(sizeof(Ocorrencia));
    oc5->id = 5; oc5->gravidade = 5; strcpy(oc5->horarioChegada, "10:40"); strcpy(oc5->horarioAtendimento, "11:00"); oc5->tipo = 3;

    // Inserindo na árvore
    inserirRec(&raiz, oc1);
    inserirRec(&raiz, oc2);
    inserirRec(&raiz, oc3);
    inserirRec(&raiz, oc4);
    inserirRec(&raiz, oc5);


    // Testando funções
    printf("\nChamadas em ordem de gravidade (In-Ordem):\n");
    inOrdem(raiz);

    printf("\nChamadas em ordem de gravidade decrescente:\n");
    inOrdemDecrescente(raiz);

    printf("\nProxima chamada a ser atendida:\n");
    Ocorrencia *prox = proximaChamada(raiz);
    if (prox != NULL) {
        printf("ID: %d | Gravidade: %d\n", prox->id, prox->gravidade);
    }

    printf("\nRemovendo ocorrencia atendida...\n");
    raiz = removerRec(raiz, prox);

    printf("\nChamadas em ordem de gravidade (In-Ordem) apos remocao:\n");
    inOrdem(raiz);

    printf("\nProxima chamada a ser atendida:\n");
    prox = proximaChamada(raiz);
    if (prox != NULL) {
        printf("ID: %d | Gravidade: %d\n", prox->id, prox->gravidade);
    }

    // Exemplo de reiniciar a árvore
    printf("\nReiniciando arvore...\n");
    liberarArvoreAVL(&raiz);




    return 0;
}
