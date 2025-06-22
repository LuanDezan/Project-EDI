#include<stdio.h>
#include<stdlib.h>

typedef struct no{
    Ocorrencia* elemento;
    struct no* dir;
    struct no* esq;
}no;

void inicializar(no **raiz){
    *raiz = NULL;
}

void inserir(no **raiz, Ocorrencia *nova){
    no *buscador = (*raiz);
    no *aux = NULL;
    while(buscador != NULL && buscador->elemento->id != nova->id){
        aux = buscador;
        (buscador->elemento->id > nova->id) ? (buscador = buscador->esq) : (buscador = buscador->dir); // Operador ternário simplificando if-else
    }
    if(buscador == NULL){ // Se buscador chegou em NULL cria um nó naquele lugar com a ocorrência, caso contrário a ocorrencia já está cadastrada, então descarta-a
        buscador = malloc(sizeof(no));
        if(buscador == NULL){
            printf("Erro na criacao de no para arvore binaria de busca: HEAP CHEIO\n");
            return;
        }
        buscador->elemento = nova;
        buscador->esq = NULL;
        buscador->dir = NULL;

        if(aux == NULL){ // Se a inserção for na raiz
            (*raiz) = buscador;
        }else{
            (aux->elemento->id > buscador->elemento->id) ? (aux->esq = buscador) : (aux->dir = buscador);
        }
    }
}

no* buscar(no **raiz, int idBuscado){
    no *buscador = (*raiz);
    while(buscador != NULL && buscador->elemento->id != idBuscado){
        (buscador->elemento->id > idBuscado) ? (buscador = buscador->esq) : (buscador = buscador->dir);
    }
    return buscador; // Retorna NULL se n encontrou o no ou o no com o idBuscado
}

Ocorrencia* remover(no **raiz, int idRemocao){
    no *buscador = (*raiz);
    no *aux = NULL;

    while(buscador != NULL && buscador->elemento->id != idRemocao){ // Encontra o nó a ser removido;
        aux = buscador;
        (buscador->elemento->id > idRemocao) ? (buscador = buscador->esq) : (buscador = buscador->dir);
    }
    if(buscador == NULL){
        return NULL; //retorna NULL caso não encontrar uma Ocorrencia com o id passado
    }
    Ocorrencia *popped = buscador->elemento;

    if(buscador->esq == NULL && buscador->dir == NULL){ // Caso em que o no tem 0 filhos
        if(aux == NULL){ // O no é raiz
            (*raiz) = NULL;
        }else{
            (aux->esq == buscador) ? (aux->esq = NULL) : (aux->dir = NULL);
        }
        free(buscador);
        return popped;
    }else{
        if(buscador->esq == NULL || buscador->dir == NULL){ // Caso em que o no tem 1 filho
            if(aux == NULL){ // O nó é raiz
                (buscador->esq == NULL) ? ((*raiz) = buscador->dir) : ((*raiz) = buscador->esq);
            }else{
                if(aux->esq == buscador){ // Se o no da esquerda do buscador for o no a ser removido
                    (buscador->esq == NULL) ? (aux->esq = buscador->dir) : (aux->esq = buscador->esq);
                }else{
                    (buscador->esq == NULL) ? (aux->dir = buscador->dir) : (aux->dir = buscador->esq);
                }
            }
            free(buscador);
            return popped;
        }else{ // Caso em que o no tem 2 filhos (nesse caso se busca o sucessor, sobrescreve o no a ser removido com conteúdo do sucessor e remove sucessor)
            no *sucessor = buscador->dir;
            no *auxSucessor = buscador;

            while(sucessor->esq != NULL){ // Encontra o menor elemento da subarvore da direita do no a ser removido;
                auxSucessor = sucessor;
                sucessor = sucessor->esq;
            }
            if(auxSucessor == buscador){
                buscador->dir = sucessor->dir; // Se o sucessor é o primeiro à direita do no a ser removido
            }else{
                auxSucessor->esq = sucessor->dir; // Isso liga a subarvore direita de sucessor ao seu anterior, desligando sucessor da arvore;
            }
            buscador->elemento = sucessor->elemento; // Sobreescrevendo
            free(sucessor);
            return popped;
        }
    }
}

void reiniciarArvore(no **raiz){
    while((*raiz) != NULL){
        remover(raiz, (*raiz)->elemento->id);
    }
}
