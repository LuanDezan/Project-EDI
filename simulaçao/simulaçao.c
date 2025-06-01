#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 40
#define MAXHASH 20
#define NUM_BAIRROS 4
#define NUM_SERVICOS 4
#define TEMPO_TICK 10

// estrutura que representa um bairro, com id, nome e ponteiro para próximo bairro na lista encadeada
typedef struct Bairro
{
    int id;
    char nomeDoBairro[30];
    struct Bairro *prox;

} Bairro;


// estrutura que representa uma ocorrencia de emergencia
// contem id, horários de chegada e atendimento, gravidade (1 a 5), tipo do serviço (hospital, policia, etc), ponteiro para bairro e próxima ocorrencia
typedef struct Ocorrencia
{
    int id;
    char horarioChegada[6];
    char horarioAtendimento[6];
    int gravidade;
    int tipo;
    Bairro *bairro;
    struct Ocorrencia *prox;
} Ocorrencia;


// descritor para a fila de ocorrencias, com ponteiros para inicio, fim e tamanho da fila
typedef struct
{
    Ocorrencia *inicio;
    Ocorrencia *fim;
    int tamanho;
} DescritorFila;


// global que guarda o tempo em min, começando em 720 = 12:00
time_t tempoGlobal = 720;



// funcap auxiliar que recebe minutos e converte para formato 12:00 fica mais bonito
void formatarHorario(int minutos, char *destino)
{
    int horas = minutos / 60;
    int mins = minutos % 60;
    sprintf(destino, "%02d:%02d", horas, mins);

}



// inicializa uma fila deixando inicio e fim como NULL e tamanho zerado
void inicializarDescritorFila(DescritorFila *fila)
{
    fila->fim = NULL;
    fila->inicio = NULL;
    fila->tamanho = 0;

}



// funcao para colocar uma ocorrencia na fila de atendimento mantendo a fila ordenada por gravidade (mais grave na frente sempre
void filaAtendimento(DescritorFila *fila, Ocorrencia *nova)
{
    // se fila vazia ou gravidade maior que o inicio, coloca no iniciio
    if (fila->inicio == NULL || nova->gravidade > fila->inicio->gravidade)
    {
        nova->prox = fila->inicio;
        fila->inicio = nova;

        if (fila->tamanho == 0)
        {
            fila->fim = nova;
        }

    } else {
        // procura a poscao certa para colocar pela ordem da gravidade
        Ocorrencia *atual = fila->inicio;

        while (atual->prox && nova->gravidade <= atual->prox->gravidade)
        {
            atual = atual->prox;
        }
            nova->prox = atual->prox;
            atual->prox = nova;


        // se inseriu no fim, atualiza o ponteiro fim
        if (nova->prox == NULL)
        {
            fila->fim = nova;
        }

    }

        // aumenra tamanho da fila
    fila->tamanho++;

}


// envia a ocorrencia para a fila do serviço dela (tipo - 1 p indice, so tipo da errado)
void enviarOcorrencia(DescritorFila *filas[], Ocorrencia *nova)
{
    filaAtendimento(filas[nova->tipo - 1], nova);
}




// cria uma ocorrencia aleatoria para simular emergencias
// sorteia id, gravidade, tipo de serviço, h de chegada (com desvio para antes do tempo global!!)
// e liga um bairro da tabela hash
Ocorrencia *criarOcorrenciaAleatoria(Bairro *tabelaHashBairro)
{
    Ocorrencia *nova = malloc(sizeof(Ocorrencia));
    if (nova == NULL)
    {
        return NULL;
    }

    nova->id = rand() % 10000;

    nova->gravidade = rand() % 5 + 1; // gravidade entre 1 e 5
    nova->tipo = rand() % NUM_SERVICOS + 1; // tipo entre 1 e 4



    // desvio aleatorio de no max 9 minutos para a hora de chegada p ficar mais realista
    int desvio = rand() % TEMPO_TICK;
    formatarHorario(tempoGlobal - desvio, nova->horarioChegada);


    // escolhe aleatoriamente o bairro pela tabela hash (id e chave)
    int idBairro = (rand() % NUM_BAIRROS) + 1;
    int chave = idBairro % MAXHASH;

    Bairro *b = tabelaHashBairro[chave].prox;

    while (b && b->id != idBairro)
    {
        b = b->prox;
    }

    if (b== NULL)
    {
        free(nova);
        return NULL;
    }

    nova->bairro = b;
    nova->prox = NULL;
    return nova;


}



void processarFilas(DescritorFila *filas[], int tempoRestante[], Bairro *tabelaHashBairro, Ocorrencia *emAtendimento[])
{
    // define o tempo final da simulacao (18:00 = 1080 minutos)
    int tempoFinal = 18 * 60;

    // garante que o tempoGlobal comece em 12:00 (720 minutos)
    if (tempoGlobal < 720)
    {
        tempoGlobal = 720;
    }

    // loop principal da simulacao: continua enquanto nn chegar no tempo final
    while (tempoGlobal < tempoFinal)
    {
        printf("\n================= novo ciclo de atendimento =================\n");

        // formata o h atual p "hh:mm"
        char horarioAtual[6];
        formatarHorario(tempoGlobal, horarioAtual);

        // constantes p  nomes dos servicos e tempos de atendimento
        const char *TIPOS_SERVICO[NUM_SERVICOS] = {"hospital", "policia", "bombeiro", "samu"};
        const int TEMPOS_ATENDIMENTO[NUM_SERVICOS] = {20, 10, 8, 15};

        // simulacao de novas ocorrencias chegando no ciclo 1 no minimo 3 no max
        int novasOcorrencias = 1 + rand() % 3;
        for (int i = 0; i < novasOcorrencias; i++)
        {
            Ocorrencia *nova = criarOcorrenciaAleatoria(tabelaHashBairro);

            if (nova)
            {
                formatarHorario(tempoGlobal, nova->horarioChegada);
                enviarOcorrencia(filas, nova);
            }
        }



        // controla se todas as filas e atendimentos estao vazios
        int sistemaVazio = 1;

        // processamento do atendimento para cada servico
        for (int i = 0; i < NUM_SERVICOS; i++)
        {
            printf("\n%-10s | hora: %s\n", TIPOS_SERVICO[i], horarioAtual);

            // se nao tiver ocorrencia sendo atendida, tenta iniciar um atendimento da fila
            if (emAtendimento[i] == NULL && filas[i]->inicio != NULL)
            {
                emAtendimento[i] = filas[i]->inicio;
                filas[i]->inicio = filas[i]->inicio->prox;
                tempoRestante[i] = TEMPOS_ATENDIMENTO[i];
                strcpy(emAtendimento[i]->horarioAtendimento, horarioAtual);
                filas[i]->tamanho--;
            }


            // se tiver ocorrencia em atendimento, processa o tempo restante
            if (emAtendimento[i])
            {
                tempoRestante[i] -= TEMPO_TICK;

                int hC, mC, hA, mA;
                sscanf(emAtendimento[i]->horarioChegada, "%2d:%2d", &hC, &mC);
                sscanf(emAtendimento[i]->horarioAtendimento, "%2d:%2d", &hA, &mA);
                int espera = (hA * 60 + mA) - (hC * 60 + mC);

                printf("  -> id: %04d\n     gravidade: %d\n     bairro: %s\n     chegada: %s\n     espera: %d min\n",emAtendimento[i]->id, emAtendimento[i]->gravidade,emAtendimento[i]->bairro->nomeDoBairro,emAtendimento[i]->horarioChegada, espera);

                if (tempoRestante[i] <= 0)
                {
                    printf("     atendimento concluido\n");
                    printf("     -----------------------------------\n\n");
                    free(emAtendimento[i]);
                    emAtendimento[i] = NULL;

                } else {

                    printf("     em atendimento... restante: %d min\n", tempoRestante[i]);
                    printf("     -----------------------------------\n\n");

                        }

                // ainda tem atendimento em andamento, sistema nao esta vazio
                sistemaVazio = 0;

            } else {
                // sem ocorrencia em atendimento nesta rodada, mas verifica se fila nao esta vazia

                if (filas[i]->tamanho > 0)
                {
                    sistemaVazio = 0;
                }

                printf("  -> sem ocorrencias nesta rodada\n");
                printf("     -----------------------------------\n\n");

                }
        }

        // incrementa o tempo global para proximo ciclo
        tempoGlobal += TEMPO_TICK;

        printf("\n================ fim do ciclo (t = %02d:%02d) ================\n",
               tempoGlobal / 60, tempoGlobal % 60);

        // imprime o estado atual das filas apos o ciclo
        printf("\n============== estado das filas apos o ciclo ==============\n\n");


            for (int i = 0; i < NUM_SERVICOS; i++)
            {
                printf("----------------- fila: %s -----------------\n", TIPOS_SERVICO[i]);
                printf("id     | chegada  | gravidade | bairro\n");
                printf("-----------------------------------------------\n");

                if (filas[i]->tamanho == 0)
                {
                    if (emAtendimento[i] != NULL && tempoRestante[i] > 0)
                    {

                        printf("em atendimento...\n");

                    } else {

                        printf("fila vazia\n");

                            }
                } else {

                    Ocorrencia *aux = filas[i]->inicio;
                    while (aux != NULL)
                    {
                        printf("%04d   | %s    | %d         | %s\n",aux->id, aux->horarioChegada,aux->gravidade, aux->bairro->nomeDoBairro);
                        aux = aux->prox;
                    }
                }

                printf("total: %d ocorrencia", filas[i]->tamanho);

                if (filas[i]->tamanho == 1)
                {
                    printf("");

                } else {
                    printf("s");

                    }


                printf(" | em atendimento: ");
                if (emAtendimento[i] != NULL)
                {
                    printf("1\n\n\n");

                } else {

                    printf("0\n\n\n");

                        }
                        printf("\n");

            }


        // se o sistema estiver vazio para todas filas e atendimentos, encerra o loop
        if (sistemaVazio)
        {
            printf("todas as filas foram esvaziadas e nenhum atendimento esta em andamento.\n");
            printf("encerrando a simulacao.\n");
            break;

        }

    }


    // se o tempo final foi atingido, informa o fim da simulacao
    if (tempoGlobal >= tempoFinal)
    {
        printf("\nsimulacao finalizada pois horario limite (%02d:%02d) foi atingido.\n",tempoFinal / 60, tempoFinal % 60);
    }


}




// funcao para mostrar o estado atual de uma fila especifica, mostra todas ocorencias nela
void mostrarFilaAtual(DescritorFila *fila, const char *nomeFila)
{
    printf("\n----------------- fila: %s -----------------\n", nomeFila);
    printf("%-6s | %-8s | %-9s | %-10s\n", "id", "chegada", "gravidade", "bairro");
    printf("-----------------------------------------------\n");
    Ocorrencia *temp = fila->inicio;
    int cont = 0;


    while (temp)
    {
        printf("%04d   | %-8s | %-9d | %-10s\n",
               temp->id, temp->horarioChegada, temp->gravidade, temp->bairro->nomeDoBairro);
        temp = temp->prox;
        cont++;
    }

    if (cont == 0) printf("fila vazia\n");
    printf("total: %d ocorrencias\n", cont);

}
