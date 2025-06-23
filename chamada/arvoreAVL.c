#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include"../constantes/constantes.h"




void inicializarAVL(noAVL **raiz){
    *raiz = NULL;
}

// Fun��o para gerar chave de ordena��o �nica
int gerarChave(Ocorrencia *oc) {
    // Garante que IDs menores tenham prioridade quando gravidades forem iguais
    return oc->gravidade * 1000000 + (1000000 - oc->id);
}

// Fun��o recursiva que calcula a altura da �rvore
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

// Rota��o para direita
void rotacionaDir(noAVL **arvore) {
    noAVL *aux = (*arvore)->esq;
    (*arvore)->esq = aux->dir;
    aux->dir = *arvore;
    *arvore = aux;
}

// Rota��o para esquerda
void rotacionaEsq(noAVL **arvore) {
    noAVL *aux = (*arvore)->dir;
    (*arvore)->dir = aux->esq;
    aux->esq = *arvore;
    *arvore = aux;
}

// Fun��o recursiva de inser��o na AVL com balanceamento
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

// Retorna o n� com o menor valor (usado na remo��o)
noAVL* minimo(noAVL *arvore) {
    noAVL *atual = arvore;
    while (atual->esq != NULL)
        atual = atual->esq;
    return atual;
}

// Fun��o de remo��o usando gravidade e ID como crit�rio
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

            free(arvore);
            return temp;
        } else {
            noAVL *temp = minimo(arvore->dir);
           arvore->info = temp->info;
            arvore->dir = removerRec(arvore->dir, temp->info);
        }
    }

    // Ajuste de balanceamento ap�s remo��o
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

// Fun��o para liberar toda a �rvore
void liberarArvoreAVL(noAVL **arvore) {
    if (*arvore == NULL) return;

    liberarArvoreAVL(&(*arvore)->esq);
    liberarArvoreAVL(&(*arvore)->dir);

    free(*arvore);
    *arvore = NULL;
}


