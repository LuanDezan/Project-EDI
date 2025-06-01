//TODO colocar o vetor de nome de bairros no arquivo de constantes
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAXHASH 20
/* n é mais necessario com o .h
typedef struct Bairro{
    int id;
    char nomeDoBairro[30];
    struct Bairro *prox;
}Bairro;*/

/* Essa é uma alternativa, acredito que pior
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

void cadastrarBairro(int idNovo, char *nomeNovo, Bairro *tabela){
    int chave;

    Bairro *novo = malloc(sizeof(Bairro));
    if(novo == NULL){
        printf("Erro de alocação na tabela Hash de Bairro\n");
        return;
    }

    novo->id = idNovo;
    strcpy(novo->nomeDoBairro, nomeNovo);
    novo->prox = NULL; // Até aqui se cria um novo bairro;

    chave = hashBairro(novo->id);

    if(tabela[chave].prox == NULL){
        tabela[chave].prox = novo;
    }else{
        novo->prox = tabela[chave].prox;
        tabela[chave].prox = novo;
    }
    //Insercao concluida
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

void buscaBairro(Bairro *tabela, int idBairro){
    int chave = hashBairro(idBairro);

    Bairro *leitor = tabela[chave].prox;

    while(leitor != NULL && leitor->id != idBairro){
        leitor = leitor->prox;
    }

    printf("Bairro %d: %s\n",idBairro, leitor->nomeDoBairro);
}

void resetarTabelaBairro(Bairro *tabela){
    for(int i= 0; i<MAXHASH; i++){
        while(tabela[i].prox != NULL){
            int idRemover = tabela[i].prox->id;
            removerBairro(tabela, idRemover);
        }
    }
}
