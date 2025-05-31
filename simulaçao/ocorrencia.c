#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


// struct que representa uma ocorrência com informações como id, horário, gravidade, tipo, bairro e ponteiro para a próxima
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



