#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "..\policia\policia.c"
#include "..\bombeiro\bombeiro.c"
#include "..\hospital\hospital.c"
#include "..\constantes\constantes.h"


int hash(int id) 
{
    
    return (id % 701) % HASH_SIZE;  
    
}



void initTabela(void *tabela[], size_t tamanho) 
{
    
    for (int i = 0; i < HASH_SIZE; i++) 
    {
        tabela[i] = NULL;
    }
    
}



//POLICIA
void cadastrarPolicia(int id, int localizacao, char *nome, policia *tabela[]) 
{
    int pos = hash(id);
    policia *novo;
    novo = malloc(sizeof(policia));
    
    
    if (!novo) 
    {
        printf("ATENCAO: Falha ao alocar policial (ID: %d)\n", id);
        return;
    }
    
    
    novo->id = id;
    novo->localizacao = localizacao;
    strcpy(novo->nome, nome);
    
    
    
    if (tabela[pos] == NULL)
    {
        tabela[pos] = novo;
    }else{
        
        // se der colisao insere no inicio
        novo->prox = tabela[pos];
        tabela[pos] = novo;
        }
    
    
}




policia* buscarPolicia(int id, policia *tabela[]) 
{
    policia *atual;
    atual= tabela[hash(id)];
    
    
    while (atual && atual->id != id) 
    {
        atual = atual->prox;
    }
    
    
    
    return atual;
    
}



void removerPolicia(int id, policia *tabela[]) 
{
    int pos = hash(id);
    policia *atual;
    policia*anterior;
    atual = tabela[pos];
    anterior = NULL;
    
    
    
    while (atual && atual->id != id) 
    {
        anterior = atual;
        atual = atual->prox;
    }
    

    if (!atual) 
    {
        printf("ATENCAO: policial (ID: %d) nao encontrado.\n", id);
        return;
    }
    
    
    if (anterior) 
    {
        anterior->prox = atual->prox;
    } else {
        tabela[pos] = atual->prox;
        
            }
            
            
            
    free(atual);
    
}



//HOSPITAL
void cadastrarHospital(int id, int localizacao, char *nome, hospital *tabela[]) 
{
    int pos = hash(id);
    hospital *novo;
    novo = malloc(sizeof(hospital));
    
    
    if (!novo) 
    {
        printf("ATENCAO: Falha ao alocar hospital (ID: %d)\n", id);
        return;
    }
    
    
    novo->id = id;
    novo->localizacao = localizacao;
    strcpy(novo->nome, nome);
    
    
    
    if (tabela[pos] == NULL)
    {
        tabela[pos] = novo;
    }else{
        
        // se der colisao insere no inicio
        novo->prox = tabela[pos];
        tabela[pos] = novo;
        }
    
    
}




hospital* buscarHospital(int id, hospital *tabela[]) 
{
    hospital *atual;
    atual = tabela[hash(id)];
    
    
    while (atual && atual->id != id) 
    {
        atual = atual->prox;
    }
    
    
    
    return atual;
    
}




void removerHospital(int id, hospital *tabela[]) 
{
    int pos = hash(id);
    hospital *atual;
    hospital*anterior;
    
    atual = tabela[pos];
    anterior = NULL;
    
    
    while (atual && atual->id != id) 
    {
        anterior = atual;
        atual = atual->prox;
    }
    
    
    
    if (!atual) 
    {
        printf("ATENCAO: Hospital (ID: %d) nao encontrado!!!.\n", id);
        return;
    }
    
    
    if (anterior) 
    {
        anterior->prox = atual->prox;
    }else{
    
        tabela[pos] = atual->prox;
        }
         
         
            
    free(atual);
    
}




//liimpa as duas tabelas
void limparTabela(void *tabela[], size_t tipo) 
{
    
    for (int i = 0; i < HASH_SIZE; i++) 
    {
        void *atual = tabela[i];
    
    
        while (atual) 
        {
            void *temp = atual;
            
            
            
            if (tipo == sizeof(policia)) 
            {
                atual = ((policia*)atual)->prox;
            }else{
                
                atual = ((hospital*)atual)->prox;
                
                    }
            
            
            free(temp);
            
        }
        
        
      tabela[i] = NULL;
      
    }
    
    
}
