//TODO colocar o vetor de nome de bairros no arquivo de constantes
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"../constantes/constantes.h"
#define MAXHASH 20
#include<time.h>

/* typedef struct Bairro{
    int id;
    char nomeDoBairro[30];
    struct Bairro *prox;
}Bairro;

Essa é uma alternativa, acredito que pior
int hashBairro(Bairro *teste){
    int chave = teste->id;

    return (chave%701)%MAXHASH; //Estou usando 701 para o espalhamento
}
*/


int hashBairro(int idBairro){
    int chave = idBairro;

    return (chave%701)%MAXHASH; //Estou usando 701 para o espalhamento
}


void inicializarTabelaBairro(Bairro *tabela){
    for(int i=0; i< MAXHASH; i++){
        tabela[i].prox = NULL;
    }
}



    void cadastrarBairro(int id, const char *nome, Bairro tabela[]) {
        int hash = id % MAXHASH;
        Bairro *novo = malloc(sizeof(Bairro));
        novo->id = id;
        strcpy(novo->nomeDoBairro, nome);

        // Definir coordenadas baseadas no ID
        switch(id) {
            case 1: // Centro
                novo->latitude = -22.9068;
                novo->longitude = -43.1729;
                break;
            case 2: // Norte
                novo->latitude = -22.8742;
                novo->longitude = -43.2973;
                break;
            case 3: // Sul
                novo->latitude = -22.9511;
                novo->longitude = -43.2105;
                break;
            case 4: // Leste
                novo->latitude = -22.8471;
                novo->longitude = -43.0971;
                break;
            default: // Coordenadas padrão
                novo->latitude = -22.9000 + (id-1)*0.01;
                novo->longitude = -43.2000 + (id-1)*0.01;
        }

        novo->prox = tabela[hash].prox;
        tabela[hash].prox = novo;
    }


void removerBairro(Bairro *tabela, int idBairro){ //Essa função me gerou uma dúvida, a gente mantêm os bairros usados em algum lugar fora da tabela hash tbm?
    int chave = hashBairro(idBairro); //Pq como eu teria o id do bairro para buscar ou remover da tabela? Usar um contados para cada struct?

    Bairro *leitor = tabela[chave].prox;
    Bairro *ant = NULL;

    while(leitor != NULL && leitor->id != idBairro){
        ant = leitor;
        leitor = leitor->prox;
    }

    if(leitor == NULL){
        printf("Erro, Bairro id %d nao encontrado na tabela hash\n", idBairro);
        return;
    }

    if(ant != NULL){
        ant->prox = leitor->prox;
    }else{
        tabela[chave].prox = NULL;
    }


    free(leitor);
}


Bairro* buscar_bairro_por_id(Bairro tabela[], int id, int maxHash) {
    int hash = id % maxHash;
    Bairro* atual = tabela[hash].prox;

    while (atual) {
        if (atual->id == id) return atual;
        atual = atual->prox;
    }
    return NULL; // Não encontrado
}


void resetarTabelaBairro(Bairro *tabela){
    for(int i= 0; i<MAXHASH; i++){
        while(tabela[i].prox != NULL){
            int idRemover = tabela[i].prox->id;
            removerBairro(tabela, idRemover);
        }
    }
}
