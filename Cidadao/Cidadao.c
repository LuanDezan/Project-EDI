#include <stdio.h>
#define MAXHASH 20
#include"../constantes/constantes.h"

/*typedef struct Cidadao{
    char CPF[14],
        nomeCidadao[35],
        email[40];
        // endereco[40]; //acho q esse n precisa
    int id,
        idBairroDeResidencia; // Isso seria o endereco
    struct Cidadao *prox;
}Cidadao;*/

int hashCidadao(int idCidadao){
    int chave = idCidadao;

    return (chave%701)%MAXHASH; //Estou usando 701 para o espalhamento
}


void inicializarTabelaCidadao(Cidadao *tabela){
    for(int i=0; i< MAXHASH; i++){
        tabela[i].prox = NULL;
    }
}

void cadastrarCidadao(char *CPFnovo, char *nomeNovo, char *emailNovo, int idNovo, int idEndereco, Cidadao *tabela){
    int chave;

    Cidadao *novo = malloc(sizeof(Cidadao));
    if(novo == NULL){
        printf("Erro de alocação na tabela Hash de Cidadao\n");
        return;
    }

    strcpy(novo->CPF, CPFnovo);
    strcpy(novo->nomeCidadao, nomeNovo);
    strcpy(novo->email, emailNovo);
    novo->idBairroDeResidencia = idEndereco;
    novo->id = idNovo;
    novo->prox = NULL; // Até aqui se cria um novo Cidadao;

    chave = hashCidadao(novo->id);

    if(tabela[chave].prox == NULL){
        tabela[chave].prox = novo;
    }else{
        novo->prox = tabela[chave].prox;
        tabela[chave].prox = novo;
    }
    //Insercao concluida
}

void removerCidadao(Cidadao *tabela, int idCidadao){ //Essa função me gerou uma dúvida, a gente mantêm os Cidadaos usados em algum lugar fora da tabela hash tbm?
    int chave = hashCidadao(idCidadao); //Pq como eu teria o id do Cidadao para buscar ou remover da tabela? Usar um contados para cada struct?

    Cidadao *leitor = tabela[chave].prox;
    Cidadao *ant = NULL;

    while(leitor != NULL && leitor->id != idCidadao){
        ant = leitor;
        leitor = leitor->prox;
    }

    if(leitor == NULL){
        printf("Erro, Cidadao id %d nao encontrado na tabela hash\n", idCidadao);
        return;
    }

    if(ant != NULL){
        ant->prox = leitor->prox;
    }else{
        tabela[chave].prox = leitor->prox;
    }


    free(leitor);
}

void buscaCidadao(Cidadao *tabela, int idCidadao){
    int chave = hashCidadao(idCidadao);

    Cidadao *leitor = tabela[chave].prox;

    while(leitor != NULL && leitor->id != idCidadao){
        leitor = leitor->prox;
    }

    printf("Cidadao %d: \nCPF: %s\nNome: %s\nEmail: %s\nEndereco: Bairro id %d\n\n",leitor->id, leitor->CPF,leitor->nomeCidadao, leitor->email, leitor->idBairroDeResidencia);
}

void resetarTabelaCidadao(Cidadao *tabela){
    for(int i= 0; i<MAXHASH; i++){
        while(tabela[i].prox != NULL){
            int idRemover = tabela[i].prox->id;
            removerCidadao(tabela, idRemover);
        }
    }
}
