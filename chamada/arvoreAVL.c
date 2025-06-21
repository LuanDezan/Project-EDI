#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include"../constantes/constantes.h"


// Nó da árvore AVL armazenando ponteiro para Ocorrencia
typedef struct noAVL {
    Ocorrencia *info;
    struct noAVL *esq, *dir;
} NoAVL;

// Função recursiva que calcula a altura da árvore
int Altura(NoAVL *arvore) {
    if (arvore == NULL)
        return -1;
    int he = Altura(arvore->esq); // Altura esquerda
    int hd = Altura(arvore->dir); // Altura direita
    return (he > hd ? he : hd) + 1; // Verifica a maior altura e adiciona 1
}

// Calcula o fator de balanceamento (altura direita - altura esquerda)
int FatorBalanceamento(NoAVL *arvore) {
    return Altura(arvore->dir) - Altura(arvore->esq);
}

// Rotação para direita
void RotacionaDir(NoAVL **arvore) {
    NoAVL *aux = (*arvore)->esq;
    (*arvore)->esq = aux->dir;
    aux->dir = *arvore;
    *arvore = aux;
}

// Rotação para esquerda
void RotacionaEsq(NoAVL **arvore) {
    NoAVL *aux = (*arvore)->dir;
    (*arvore)->dir = aux->esq;
    aux->esq = *arvore;
    *arvore = aux;
}

// Função recursiva de inserção na AVL com balanceamento
bool InserirRec(NoAVL **arvore, Ocorrencia *ocorrencia) {
    int FB;

    if (*arvore == NULL) {
        // Inserção direta quando encontra posição vazia
        NoAVL *novo = (NoAVL *)malloc(sizeof(NoAVL));
        if (novo == NULL)
            return false;
        novo->info = ocorrencia;
        novo->esq = novo->dir = NULL;
        *arvore = novo;
    } else {
        // Comparação principal: gravidade
        if (ocorrencia->gravidade < (*arvore)->info->gravidade)
            InserirRec(&(*arvore)->esq, ocorrencia);
        else if (ocorrencia->gravidade > (*arvore)->info->gravidade)
            InserirRec(&(*arvore)->dir, ocorrencia);
        else {
            // Gravidade repetida: desempata usando o ID
            if (ocorrencia->id < (*arvore)->info->id)
                InserirRec(&(*arvore)->esq, ocorrencia);
            else
                InserirRec(&(*arvore)->dir, ocorrencia);
        }

        // Verificação de balanceamento após inserção
        FB = FatorBalanceamento(*arvore);

        // Rotações necessárias para manter AVL balanceada
        if (FB >= 2) {
            if (FatorBalanceamento((*arvore)->dir) > 0)
                RotacionaEsq(arvore);
            else {
                RotacionaDir(&(*arvore)->dir);
                RotacionaEsq(arvore);
            }
        } else if (FB <= -2) {
            if (FatorBalanceamento((*arvore)->esq) < 0)
                RotacionaDir(arvore);
            else {
                RotacionaEsq(&(*arvore)->esq);
                RotacionaDir(arvore);
            }
        }
    }
    return true;
}

// Retorna o nó com o menor valor (usado na remoção)
NoAVL* Minimo(NoAVL *arvore) {
    NoAVL *atual = arvore;
    while (atual->esq != NULL)
        atual = atual->esq;
    return atual;
}

// Função de remoção usando gravidade e ID como critério
NoAVL* RemoverRec(NoAVL *arvore, int gravidade, int id) {
    if (arvore == NULL)
        return NULL;

    // Primeiro compara gravidade
    if (gravidade < arvore->info->gravidade) {
        arvore->esq = RemoverRec(arvore->esq, gravidade, id);
    } else if (gravidade > arvore->info->gravidade) {
        arvore->dir = RemoverRec(arvore->dir, gravidade, id);
    } else {
        // Gravidade igual, desempata por ID
        if (id < arvore->info->id) {
            arvore->esq = RemoverRec(arvore->esq, gravidade, id);
        } else if (id > arvore->info->id) {
            arvore->dir = RemoverRec(arvore->dir, gravidade, id);
        } else {
            // Nó encontrado para remoção
            if (arvore->esq == NULL || arvore->dir == NULL) {
                NoAVL *temp = (arvore->esq != NULL) ? arvore->esq : arvore->dir;
                free(arvore->info);
                free(arvore);
                return temp;
            } else {
                NoAVL *temp = Minimo(arvore->dir);
                *(arvore->info) = *(temp->info);
                arvore->dir = RemoverRec(arvore->dir, temp->info->gravidade, temp->info->id);
            }
        }
    }

    // Ajuste de balanceamento após remoção
    int FB = FatorBalanceamento(arvore);

    if (FB >= 2) {
        if (FatorBalanceamento(arvore->dir) >= 0)
            RotacionaEsq(&arvore);
        else {
            RotacionaDir(&arvore->dir);
            RotacionaEsq(&arvore);
        }
    } else if (FB <= -2) {
        if (FatorBalanceamento(arvore->esq) <= 0)
            RotacionaDir(&arvore);
        else {
            RotacionaEsq(&arvore->esq);
            RotacionaDir(&arvore);
        }
    }

    return arvore;
}

// Função auxiliar recursiva para encontrar a próxima chamada
// Ela percorre toda a árvore e atualiza o ponteiro "melhor" sempre que encontra uma ocorrência com gravidade maior, ou com mesmo nível de gravidade mas ID menor
void BuscarProxima(NoAVL *arvore, Ocorrencia **melhor) {
    if (arvore == NULL) return;

    // Se ainda não há candidato ou encontramos uma ocorrência melhor:
    // 1. Gravidade maior
    // 2. Ou mesma gravidade mas ID menor (critério de desempate)
    if (*melhor == NULL ||
        arvore->info->gravidade > (*melhor)->gravidade ||
        (arvore->info->gravidade == (*melhor)->gravidade && arvore->info->id < (*melhor)->id)) {
        *melhor = arvore->info;
    }

    // Percorre recursivamente toda a árvore
    BuscarProxima(arvore->esq, melhor);
    BuscarProxima(arvore->dir, melhor);
}

// Função principal que retorna a próxima chamada
// Inicia com ponteiro nulo e chama a função recursiva auxiliar
Ocorrencia* ProximaChamada(NoAVL *arvore) {
    Ocorrencia *melhor = NULL;
    BuscarProxima(arvore, &melhor);
    return melhor;
}

// Percorre em ordem (esquerda, raiz, direita) - mostra gravidades em ordem crescente
void InOrdem(NoAVL *arvore) {
    if (arvore != NULL) {
        InOrdem(arvore->esq);
        printf("ID: %d | Gravidade: %d | Chegada: %s | Atendimento: %s | Tipo: %d\n", arvore->info->id, arvore->info->gravidade, arvore->info->horarioChegada, arvore->info->horarioAtendimento, arvore->info->tipo);
        InOrdem(arvore->dir);
    }
}

// Percorre em ordem decrescente de gravidade
void InOrdemDecrescente(NoAVL *arvore) {
    if (arvore != NULL) {
        InOrdemDecrescente(arvore->dir);
        printf("ID: %d | Gravidade: %d | Chegada: %s | Atendimento: %s | Tipo: %d\n", arvore->info->id, arvore->info->gravidade, arvore->info->horarioChegada, arvore->info->horarioAtendimento, arvore->info->tipo);
        InOrdemDecrescente(arvore->esq);
    }
}

// =================TESTE=================
int main() {
    NoAVL *raiz = NULL;

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
    InserirRec(&raiz, oc1);
    InserirRec(&raiz, oc2);
    InserirRec(&raiz, oc3);
    InserirRec(&raiz, oc4);
    InserirRec(&raiz, oc5);


    // Testando funções
    printf("\nChamadas em ordem de gravidade (In-Ordem):\n");
    InOrdem(raiz);

    printf("\nChamadas em ordem de gravidade decrescente:\n");
    InOrdemDecrescente(raiz);

    printf("\nProxima chamada a ser atendida:\n");
    Ocorrencia *prox = ProximaChamada(raiz);
    if (prox != NULL) {
        printf("ID: %d | Gravidade: %d\n", prox->id, prox->gravidade);
    }

    printf("\nRemovendo ocorrencia atendida...\n");
    raiz = RemoverRec(raiz, prox->gravidade, prox->id);

    printf("\nChamadas em ordem de gravidade (In-Ordem) apos remocao:\n");
    InOrdem(raiz);

    return 0;
}
