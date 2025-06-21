#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX 30
#define MAXHASH 20

typedef struct hospital{
    char nome[MAX];
    int id;
    int localizacao;
    struct hospital*prox;

}hospital;

int hashHospital(int idHospital){
    int chave = idHospital;

    return (chave%701)%MAXHASH; //Estou usando 701 para o espalhamento
}

void inicializarTabelaHospital(hospital *tabela){
    for(int i=0; i<MAXHASH; i++){
        tabela[i].prox = NULL;
    }
}

void cadastrarHospital(char *nomeNovo, int idNovo, int localizacaoNovo, hospital *tabela){
    int chave;

    hospital *novo = malloc(sizeof(hospital));
    if(novo == NULL){
        printf("Erro de alocação na tabela Hash de Hospital\n");
        return;
    }

    strcpy(novo->nome, nomeNovo);
    novo->id = idNovo;
    novo->localizacao = localizacaoNovo;
    novo->prox = NULL; // Cadastro policia completo

    chave = hashHospital(novo->id);

    if(tabela[chave].prox == NULL){
        tabela[chave].prox = novo;
    }else{
        novo->prox = tabela[chave].prox;
        tabela[chave].prox = novo;
    }
    //Insercao na tabela completa
}

void removerHospital(hospital *tabela, int idHospital){ //Essa função me gerou uma dúvida, a gente mantêm os Hospitais usados em algum lugar fora da tabela hash tbm?
    int chave = hashHospital(idHospital); //Pq como eu teria o id do Hospital para buscar ou remover da tabela? Usar um contador para cada struct?

    hospital *leitor = tabela[chave].prox;
    hospital *ant = NULL;

    while(leitor != NULL && leitor->id != idHospital){
        ant = leitor;
        leitor = leitor->prox;
    }

    if(leitor == NULL){
        printf("Erro, hospital id %d nao encontrado na tabela hash\n", idHospital);
        return;
    }

    if(ant != NULL){
        ant->prox = leitor->prox;
    }else{
        tabela[chave].prox = NULL;
    }


    free(leitor);
}

void buscaHospital(hospital *tabela, int idHospital){
    int chave = hashHospital(idHospital);

    hospital *leitor = tabela[chave].prox;

    while(leitor != NULL && leitor->id != idHospital){
        leitor = leitor->prox;
    }
    if(leitor == NULL){
        printf("Hospital nao encontrado\n");
        return;
    }
    printf("Hospital %d: \nNome: %s\nSede: Bairro id %d\n\n",idHospital, leitor->nome, leitor->localizacao); //Seria legal apresentar o nome do bairro, talvez passando a tabela Hash de bairro como parametro para chamar busca bairro
}

void resetarTabelaHospital(hospital *tabela){
    for(int i= 0; i<MAXHASH; i++){
        while(tabela[i].prox != NULL){
            int idRemover = tabela[i].prox->id;
            removerHospital(tabela, idRemover);
        }
    }
}
