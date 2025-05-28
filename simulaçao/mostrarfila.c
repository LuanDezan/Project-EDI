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
#include".\gerarOcorrencia.c"
#include".\colocarfila.c"
#include".\MAIN\main.c"



void mostrarFila()
{
    Ocorrencia *temp = inicioFila;

    printf("\n--- FILA DE ATENDIMENTO ---\n");
    while (temp)
    {
        char tipoTexto[20];

        if (temp->tipo == 1)
        {
            strcpy(tipoTexto, "Hospital");

        } else if (temp->tipo == 2)
          {
            strcpy(tipoTexto, "Polícia");

            } else if (temp->tipo == 3)
              {
                strcpy(tipoTexto, "Bombeiro");

                } else if (temp->tipo == 4)
                {
                  strcpy(tipoTexto, "SAMU");

                  } else {
                            strcpy(tipoTexto, "Outro");
                          }

        printf("ID: %d | Horário: %s | Gravidade: %d | Tipo: %d (%s) | Bairro: %s\n", temp->id, temp->horario, temp->gravidade, temp->tipo, tipoTexto, temp->bairro->nomeDoBairro);


        temp = temp->prox;
    }
}
