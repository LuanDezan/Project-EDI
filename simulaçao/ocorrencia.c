#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


typedef struct Ocorrencia
{
    int id;
    char horario[6];
    int gravidade;
    int tipo;
    struct Bairro *bairro;
    struct Ocorrencia *prox;
} Ocorrencia;


