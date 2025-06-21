#include<stdio.h>
#include<stdlib.h>

#define MAXHASH 20

typedef struct SAMU{
    int id,
        prioridade, //Prioridade da chamada ativa de atendimento Pensei em 1,2 e 3 sendo o menor numero menos prioridade
        vazia, // Se a ambulância está sendo usada = 0, se esta vazia = 1
        localizacao; // Localização vai depender da implementacao do sistema (talvez id de bairro)
    struct SAMU *prox;
}SAMU;

int hashSAMU(int idSAMU){
    int chave = idSAMU;

    return (chave%701)%MAXHASH; //Estou usando 701 para o espalhamento
}

void inicializarTabelaSAMU(SAMU *tabela){
    for(int i=0; i<MAXHASH; i++){
        tabela[i].prox = NULL;
    }
}

void cadastrarSAMU(int idNovo, int prioridadeNovo, int vaziaNovo, int localizacaoNovo, SAMU *tabela){
    int chave;

    SAMU *novo = malloc(sizeof(SAMU));
    if(novo == NULL){
        printf("Erro de alocação na tabela Hash de Ambulancias\n");
        return;
    }

    novo->id = idNovo;
    novo->prioridade = prioridadeNovo;
    novo->vazia = vaziaNovo;
    novo->localizacao = localizacaoNovo;
    novo->prox = NULL; // Cadastro ambulancia completo

    chave = hashSAMU(novo->id);

    if(tabela[chave].prox == NULL){
        tabela[chave].prox = novo;
    }else{
        novo->prox = tabela[chave].prox;
        tabela[chave].prox = novo;
    }
    //Insercao na tabela completa
}
//a partir daqui estou adaptando
void removerSAMU(SAMU *tabela, int idSAMU){ //Essa função me gerou uma dúvida, a gente mantêm os SAMUs usados em algum lugar fora da tabela hash tbm?
    int chave = hashSAMU(idSAMU); //Pq como eu teria o id do SAMU para buscar ou remover da tabela? Usar um contados para cada struct?

    SAMU *leitor = tabela[chave].prox;
    SAMU *ant = NULL;

    while(leitor != NULL && leitor->id != idSAMU){
        ant = leitor;
        leitor = leitor->prox;
    }

    if(leitor == NULL){
        printf("Erro, SAMU id %d nao encontrado na tabela hash\n", idSAMU);
        return;
    }

    if(ant != NULL){
        ant->prox = leitor->prox;
    }else{
        tabela[chave].prox = NULL;
    }


    free(leitor);
}

void buscaSAMU(SAMU *tabela, int idSAMU){
    int chave = hashSAMU(idSAMU);

    SAMU *leitor = tabela[chave].prox;

    while(leitor != NULL && leitor->id != idSAMU){
        leitor = leitor->prox;
    }

    printf("SAMU %d: \nVazio: %d\nOnde esta: Bairro id %d\n\n",idSAMU, leitor->vazia, leitor->localizacao); //Seria legal apresentar o nome do bairro, talvez passando a tabela Hash de bairro como parametro para chamar busca bairro
}

void resetarTabelaSAMU(SAMU *tabela){
    for(int i= 0; i<MAXHASH; i++){
        while(tabela[i].prox != NULL){
            int idRemover = tabela[i].prox->id;
            removerSAMU(tabela, idRemover);
        }
    }
}
