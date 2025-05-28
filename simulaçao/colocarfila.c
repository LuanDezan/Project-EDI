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
#include".\MAIN\main.c"



Ocorrencia *inicioFila = NULL;

void filaAtendimento(Ocorrencia *nova)
{
    if (!inicioFila || nova->gravidade > inicioFila->gravidade)
    {
        nova->prox = inicioFila;
        inicioFila = nova;
        return;
    }

    Ocorrencia *atual = inicioFila;
    while (atual->prox && nova->gravidade <= atual->prox->gravidade)
    {
        atual = atual->prox;
    }

    nova->prox = atual->prox;
    atual->prox = nova;
}
