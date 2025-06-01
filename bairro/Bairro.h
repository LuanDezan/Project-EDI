#ifndef BAIRRO_H
#define BAIRRO_H

typedef struct Bairro {
    int id;
    char nomeDoBairro[30];
    struct Bairro *prox;
} Bairro;

#endif
