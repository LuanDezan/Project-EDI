#include<stdio.h>
#include <stdlib.h>
#include<string.h>
#include"../bombeiro/bombeiro.c"
#include"../constantes/constantes.h"
#define HASH 30



// Função de hash simples para distribuir os IDs nas posições da tabela hash
int hash(int id){
    return (id%701)%HASH;
}

// Inicializa todos os ponteiros da tabela hash como NULL
void inicializarBombeiro(bombeiros *tabela){
    for(int i=0; i < HASH; i++){
        tabela[i].prox = NULL;
    }
}

// Cadastra um bombeiro na tabela hash
void cadastrarBombeiro(int id, char *nome, int localizacao, bombeiros *tabela){

    int chave = hash(id);

    // Aloca memória para o novo bombeiro
    bombeiros *novo = malloc(sizeof(bombeiros));
    if(novo == NULL){
        printf("Erro ao alocar memoria para bombeiro!!!!\n");
        return;
    }

    // Inicializa os dados do novo bombeiro
    novo->id = id;
    strcpy(novo->nome, nome);
    novo->localizacao = localizacao;
    novo->prox = NULL;

    // Insere o novo bombeiro na lista encadeada (tratamento de colisões por encadeamento)
    if(tabela[chave].prox == NULL){
        tabela[chave].prox = novo;
    }else{
        novo->prox = tabela[chave].prox;
        tabela[chave].prox = novo;
    }
}

// Remove um bombeiro da tabela hash com base no ID
void removerBombeiro(bombeiros *tabela, int id){
    int chave = hash(id);

    bombeiros *ler = tabela[chave].prox;
    bombeiros *ant = NULL;

    // Percorre a lista procurando o ID
    while(ler != NULL && ler->id != id){
        ant = ler;
        ler = ler->prox;
    }

    if(ler == NULL){
        printf("Nao foi possivel encontrar o id %d na tabela hash\n",id);
        return;
    }

    // Remove o nó da lista
    if(ant != NULL){
        ant->prox = ler->prox;
    }else{
        tabela[chave].prox = ler->prox;
    }

    free(ler); // Libera a memória do nó removido

}

// Busca um bombeiro e imprime seus dados
void buscaBombeiro(bombeiros *tabela, int id){
    int chave = hash(id);

    bombeiros *ler = tabela[chave].prox;

    // Percorre a lista buscando o ID
    while(ler != NULL && ler->id != id){
        ler = ler->prox;
    }

    if(ler == NULL){
        printf("Bombeiro com ID %d nao encontrado!\n",id);
        return;
    }

    // Exibe os dados do bombeiro
    printf("Bombeiro %d:\nNome: %s\nLocalizacao: %d\n",ler->id,ler->nome,ler->localizacao);
}

// Remove todos os bombeiros da tabela (limpa a tabela)
void resetarTabelaBombeiro(bombeiros *tabela){
    for(int i=0;i<HASH;i++){
        while(tabela[i].prox !=  NULL){
            removerBombeiro(tabela, tabela[i].prox->id);
        }
    }
}

// Função de cadastro de policial (a ser implementada futuramente)
//void cadastrarPolicia(){

//}

// Função de cadastro de hospital (a ser implementada futuramente)
//void cadastrarHospital(){

//}
