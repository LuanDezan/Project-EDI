#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX 30
#define MAXHASH 20

typedef struct policia{
    char nome[MAX];
    int localizacao, id;
    struct policia*prox;
}policia;

int hashPolicia(int idPolicia){
    int chave = idPolicia;

    return (chave%701)%MAXHASH; //Estou usando 701 para o espalhamento
}

void inicializarTabelaPolicia(policia *tabela){
    for(int i=0; i<MAXHASH; i++){
        tabela[i].prox = NULL;
    }
}

void cadastrarPolicia(char *nomeNovo, int localizacaoNovo, int idNovo, policia *tabela){
    int chave;

    policia *novo = malloc(sizeof(policia));
    if(novo == NULL){
        printf("Erro de alocação na tabela Hash de Policial\n");
        return;
    }

    strcpy(novo->nome, nomeNovo);
    novo->localizacao = localizacaoNovo;
    novo->id = idNovo;
    novo->prox = NULL; // Cadastro policia completo

    chave = hashPolicia(novo->id);

    if(tabela[chave].prox == NULL){
        tabela[chave].prox = novo;
    }else{
        novo->prox = tabela[chave].prox;
        tabela[chave].prox = novo;
    }
    //Insercao na tabela completa
}

void removerPolicia(policia *tabela, int idPolicia){ //Essa função me gerou uma dúvida, a gente mantêm as Policiais usados em algum lugar fora da tabela hash tbm?
    int chave = hashPolicia(idPolicia); //Pq como eu teria o id do Policial para buscar ou remover da tabela? Usar um contador para cada struct?

    policia *leitor = tabela[chave].prox;
    policia *ant = NULL;

    while(leitor != NULL && leitor->id != idPolicia){
        ant = leitor;
        leitor = leitor->prox;
    }

    if(leitor == NULL){
        printf("Erro, policial id %d nao encontrado na tabela hash\n", idPolicia);
        return;
    }

    if(ant != NULL){
        ant->prox = leitor->prox;
    }else{
        tabela[chave].prox = NULL;
    }


    free(leitor);
}

void buscaPolicia(policia *tabela, int idPolicia){
    int chave = hashPolicia(idPolicia);

    policia *leitor = tabela[chave].prox;

    while(leitor != NULL && leitor->id != idPolicia){
        leitor = leitor->prox;
    }
    if(leitor == NULL){
        printf("Policial nao encontrado\n");
        return;
    }
    printf("Policial %d: \nNome: %s\nSede: Bairro id %d\n\n",idPolicia, leitor->nome, leitor->localizacao); //Seria legal apresentar o nome do bairro, talvez passando a tabela Hash de bairro como parametro para chamar busca bairro
}

void resetarTabelaPolicia(policia *tabela){
    for(int i= 0; i<MAXHASH; i++){
        while(tabela[i].prox != NULL){
            int idRemover = tabela[i].prox->id;
            removerPolicia(tabela, idRemover);
        }
    }
}

