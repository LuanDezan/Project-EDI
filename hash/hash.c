#include<stdio.h>
#include <stdlib.h>
#include<string.h>
#include"..\bombeiro\bombeiro.c"
#include"..\constantes\constantes.h"

//fun��o hash
int hash(int id){
    return (id%701)%HASH;
}

//cadastro na tabela hash bombeiro
void cadastrarBombeiro(int id, char *nome, int localizacao, bombeiros *tabela[]){
    int x = hash(id);

    //alocar novo n�
    bombeiros *novo = malloc(sizeof(bombeiros));
    if(novo == NULL){
        printf("Erro ao alocar memoria para bombeiro!!!!\n");
        return;
    }

    novo->id = id;
    strcpy(novo->nome, nome);
    novo->localizacao = localizacao;
    novo->prox = NULL;

    if(tabela[x] == NULL){
        tabela[x] = novo;
    }else{
        //colis�o
        novo->prox = tabela[x];
        tabela[x] = novo;
    }
}

void cadastrarPolicia(){

}

void cadastrarHospital(){

}
