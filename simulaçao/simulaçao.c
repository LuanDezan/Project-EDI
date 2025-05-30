#include<string.h>
#include <string.h>
#include <time.h>

//TODO colocar o vetor de nome de bairros no arquivo de constantes
#define MAX 40
#define HASH_SIZE 20
#define MAXHASH 20
#define HASH 20


#include"..\constantes\constantes.h"
#include"..\Cidadao\Cidadao.c"
#include"..\SAMU\Samu.c"
#include"..\bairro\Bairro.c"
#include"..\hash\hash.c"
#include"..\hospital\HospitalComHash.c"
#include"..\policia\PoliciaComHash.c"
#include".\ocorrencia.c"

typedef struct
{
    Ocorrencia *inicio;
    Ocorrencia *fim;
    int tamanho;

} DescritorFila;


// inicializa o descritor da fila com valores nulos e tamanho zero
void inicializarDescritorFila(DescritorFila *fila)
{
    fila->fim = NULL;
    fila->inicio = NULL;
    fila->tamanho = 0;
}

// variavel global que representa os minutos totais desde 00:00, iniciando em 12:00 (720 minutos)
int minutosTotais = 720;

// formata os minutos totais em horario hh:mm e salva na string destino
void formatarHorario(int minutos, char *destino) {
    int horas = minutos / 60;
    int mins = minutos % 60;
    sprintf(destino, "%02d:%02d", horas, mins);
}

// insere uma ocorrencia na fila de atendimento de acordo com a gravidade
void filaAtendimento(DescritorFila *fila, Ocorrencia *nova)
{
    if (fila->inicio == NULL|| nova->gravidade > fila->inicio->gravidade)
    {
        // insere no inicio se a fila estiver vazia ou se a gravidade for maior
        nova->prox = fila->inicio;
        fila->inicio = nova;

        if(fila->tamanho == 0)
        {
            fila->fim = nova;
        }

    } else {
        // percorre a fila para encontrar o local correto
            Ocorrencia *atual = fila->inicio;

        while (atual->prox != NULL && nova->gravidade <= atual->prox->gravidade)
        {
            atual = atual->prox;
        }

        // insere a nova ocorrencia na posicao correta
        nova->prox = atual->prox;
        atual->prox = nova;

        // atualiza o ponteiro do fim da fila, se necessario
        if (nova->prox == NULL)
        {
            fila->fim = nova;
        }
    }

    fila->tamanho++;
}

// envia uma ocorrencia para a fila de atendimento
void enviarOcorrencia(DescritorFila *fila, Ocorrencia *oc)
{
    filaAtendimento(fila, oc);
}





// gera uma nova ocorrencia aleatoria e a adiciona na fila
void gerarOcorrencia(DescritorFila *fila, Bairro *tabelaHashBairro)
{
    Ocorrencia *nova = malloc(sizeof(Ocorrencia));

    if (nova == NULL)
    {
        printf("erro de alocacao na ocorrencia.\n");
        return;
    }

    // gera dados aleatorios para a ocorrencia
    nova->id = rand() % 10000;
    nova->gravidade = rand() % 5 + 1;
    nova->tipo = rand() % 5 + 1;
    formatarHorario(minutosTotais, nova->horario);

    // seleciona um bairro aleatorio
    int idBairroAleatorio = (rand() % 10) * 100;
    int chave = hashBairro(idBairroAleatorio);
    Bairro *b = tabelaHashBairro[chave].prox;


    // procura o bairro com o id correspondente
    while (b && b->id != idBairroAleatorio)
    {
        b = b->prox;
    }


    if (b == NULL)
    {
        printf("erro: bairro %d nao encontrado. ocorrencia cancelada.\n", idBairroAleatorio);
        free(nova);
        return;
    }

    nova->bairro = b;
    nova->prox = NULL;

    // define o tipo de emergência como texto
    char tipoTexto[20];
    if (nova->tipo == 1)
    {
        strcpy(tipoTexto, "hospital");

    } else if (nova->tipo == 2)
        {
            strcpy(tipoTexto, "policia");

        } else if (nova->tipo == 3)
            {
                  strcpy(tipoTexto, "bombeiro");

                } else if (nova->tipo == 4)
                    {
                        strcpy(tipoTexto, "samu");
                        } else {
                                  strcpy(tipoTexto, "outro");
                                }
                                
                                

    // imprime os dados da ocorrencia
    printf("ocorrencia gerada:\n");
    printf("id: %d | horario: %s | gravidade: %d | tipo: %d (%s) | bairro: %s\n", nova->id, nova->horario, nova->gravidade, nova->tipo, tipoTexto, b->nomeDoBairro);

    // envia a ocorrnecia para a fila
    enviarOcorrencia(fila, nova);

    // atualiza o horario com base na gravidade da ocorrencia
    if (nova->gravidade == 1) 
    {
        minutosTotais += rand() % 4 + 5;
    
    } else if (nova->gravidade == 2)
      {
          
         minutosTotais += rand() % 4 + 4;

       } else if (nova->gravidade == 3)
          {
              
            minutosTotais += rand() % 3 + 3;
            
            } else if (nova->gravidade == 4)
                {
                    
                    minutosTotais += rand() % 2 + 2;
                    
                    } else if (nova->gravidade == 5)
                          {
                              
                              minutosTotais += rand() % 2 + 1;
                            }
}



// mostra todas as ocorrências presentes na fila
void mostrarFila(DescritorFila *fila)
{
    Ocorrencia *temp = fila->inicio;

    printf("\n--- fila de atendimento (%d ocorrencia(s)) ---\n", fila->tamanho);

    while (temp)
    {
        char tipoTexto[20];

        if (temp->tipo == 1)
        {
            strcpy(tipoTexto, "hospital");

            } else if (temp->tipo == 2)
              {
                strcpy(tipoTexto, "policia");

              } else if (temp->tipo == 3)
                {
                  strcpy(tipoTexto, "bombeiro");

                    } else if (temp->tipo == 4)
                    {
                      strcpy(tipoTexto, "samu");

                    } else {
                              strcpy(tipoTexto, "outro");
                            }


        printf("id: %d | horario: %s | gravidade: %d | tipo: %d (%s) | bairro: %s\n", temp->id, temp->horario, temp->gravidade, temp->tipo, tipoTexto, temp->bairro->nomeDoBairro);

        temp = temp->prox;

    }
}



// libera a mem alocada para a fila
void limparFila(DescritorFila *fila)
{
    Ocorrencia *atual = fila->inicio;

    while (atual)
    {
        Ocorrencia *prox = atual->prox;
        free(atual);
        atual = prox;
    }

    fila->inicio = NULL;
    fila->fim = NULL;
    fila->tamanho = 0;
}

