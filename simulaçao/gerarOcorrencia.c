#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include"..\constantes\constantes.h"
#include"..\Cidadao\Cidadao.c"
#include"..\SAMU\Samu.c"
#include"..\bairro\Bairro.c"

#include"..\hospital\HospitalComHash.c"
#include"..\policia\PoliciaComHash.c"
#include"..\constantes\constantes.h"
#include".\ocorrencia.c"
#include".\MAIN\main.c"




int minutosTotais = 720; // 12:00

void formatarHorario(int minutos, char *destino)
{
    int horas = minutos / 60;
    int mins = minutos % 60;
    sprintf(destino, "%02d:%02d", horas, mins);
}



void enviarOcorrencia(Ocorrencia *oc)
{
    filaAtendimento(oc);
}



void gerarOcorrencia(Bairro *tabelaHashBairro)
{
    // aloca uma nova ocorrência
    Ocorrencia *nova = malloc(sizeof(Ocorrencia));

    if (!nova)
  {
      printf("Erro de alocação na ocorrência.\n");
      return;
  }


    // preenche dados aleatórios da ocorrência

    nova->id = rand() % 10000;                           // id da ocorrência
    nova->gravidade = rand() % 5 + 1;                  // gravidade entre 1 e 5
    nova->tipo = rand() % 5 + 1;                      // tipo da emergência entre 1 e 5
    formatarHorario(minutosTotais, nova->horario);    // define o horário atual


    // sorteia um id de bairro

    int idBairroAleatorio = (rand() % 10) * 100;

    int chave = hashBairro(idBairroAleatorio);

    Bairro *b = tabelaHashBairro[chave].prox;


    // percorre a lista encadeada da hash até achar o bairro certo
    while (b && b->id != idBairroAleatorio)
    {
        b = b->prox;
    }


    // se n achou, cancela a ocorrência
    if (!b)
    {
        printf("Erro: bairro %d não encontrado. Ocorrência cancelada.\n", idBairroAleatorio);
        free(nova);
        return;
    }


    // se achou, vincula o bairro à ocorrência
    nova->bairro = b;
    nova->prox = NULL;

    // define o nome do tipo de emergência para imprimir no final
    char tipoTexto[20];

    if (nova->tipo == 1)
    {
        strcpy(tipoTexto, "Hospital");

    } else if (nova->tipo == 2)
      {
          strcpy(tipoTexto, "Polícia");

        } else if (nova->tipo == 3)
          {
              strcpy(tipoTexto, "Bombeiro");

            } else if (nova->tipo == 4)
              {
                strcpy(tipoTexto, "SAMU");

                } else {
                          strcpy(tipoTexto, "Outro");
                        }


    // imprime os dados da ocorrência criada
    printf("Ocorrência gerada:\n");
    printf("ID: %d | Horário: %s | Gravidade: %d | Tipo: %d (%s) | Bairro: %s\n", nova->id, nova->horario, nova->gravidade, nova->tipo, tipoTexto, b->nomeDoBairro);


    // envia a ocorrência para a fila de atendimento
    enviarOcorrencia(nova);

    // define quanto tempo passa até a próxima ocorrência (depende da gravidade)
    if (nova->gravidade == 1)
   {
        minutosTotais += rand() % 4 + 5; // espera de 5 a 8 min

    } else if (nova->gravidade == 2)
      {
          minutosTotais += rand() % 4 + 4; // 4 a 7 min

      } else if (nova->gravidade == 3)
        {
            minutosTotais += rand() % 3 + 3; // 3 a 5 min

          } else if (nova->gravidade == 4)
            {
                minutosTotais += rand() % 2 + 2; // 2 a 3 min

               } else if (nova->gravidade == 5)
                {
                    minutosTotais += rand() % 2 + 1; // 1 a 2 min
                  }


}
