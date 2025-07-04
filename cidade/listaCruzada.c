#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"../bairro/Bairro.h"
#include"../constantes/constantes.h"


// Constantes para limites máximos
#define MAX_BAIRROS 4

#define MAX_HISTORICO 100

//sistema cidade, ligando bairros a serviços




// Inicializa a cidade com todas as posições nulas
void inicializar_cidade(Cidade *cidade) {
    for (int i = 0; i < MAX_BAIRROS; i++) {
        cidade->linhas[i] = NULL;
    }
    for (int i = 0; i < MAX_SERVICOS; i++) {
        cidade->colunas[i] = NULL;
    }
}

// Cria e retorna um novo nó da lista cruzada
No* criar_no(Bairro *bairro, void *servico, TipoServico tipo) {
    No *novo = malloc(sizeof(No));
    if (!novo) return NULL;

    novo->bairro = bairro;
    novo->servico = servico;
    novo->tipo_servico = tipo;
    novo->direita = NULL;
    novo->baixo = NULL;
    return novo;
}

void inserir_servico(Cidade *cidade, Bairro *bairro, void *servico, TipoServico tipo) {
    if (!cidade || !bairro || !servico || bairro->id < 1 || bairro->id > MAX_BAIRROS) {
        printf("Parametros invalidos para insercao de servico! Bairro ID: %d\n", bairro->id);
        return;
    }

    No *novo = criar_no(bairro, servico, tipo);
    if (!novo) return;

    // Índice ajustado para arrays (bairro id 1 = índice 0)
    int idx_bairro = bairro->id - 1;

    // Insere na linha do bairro (ordenado por tipo de serviço)
    if (cidade->linhas[idx_bairro] == NULL) {
        cidade->linhas[idx_bairro] = novo;
    } else {
        No *atual = cidade->linhas[idx_bairro];
        No *anterior = NULL;

        while (atual && atual->tipo_servico < tipo) {
            anterior = atual;
            atual = atual->direita;
        }

        if (anterior == NULL) {
            novo->direita = cidade->linhas[idx_bairro];
            cidade->linhas[idx_bairro] = novo;
        } else {
            novo->direita = anterior->direita;
            anterior->direita = novo;
        }
    }

    // Insere na coluna do tipo de serviço (ordenado por id do bairro)
    if (cidade->colunas[tipo] == NULL) {
        cidade->colunas[tipo] = novo;
    } else {
        No *atual = cidade->colunas[tipo];
        No *anterior = NULL;

        while (atual && atual->bairro->id < bairro->id) {
            anterior = atual;
            atual = atual->baixo;
        }

        if (anterior == NULL) {
            novo->baixo = cidade->colunas[tipo];
            cidade->colunas[tipo] = novo;
        } else {
            novo->baixo = anterior->baixo;
            anterior->baixo = novo;
        }
    }
}

// Imprime todos os serviços organizados por bairro e por tipo
void imprimir_lista_cruzada(Cidade *cidade) {
    printf("=== Lista Cruzada ===\n");

    // Imprime por bairros (linhas)
    printf("\nPor Bairros:\n");
    for (int i = 0; i < MAX_BAIRROS; i++) {
        if (cidade->linhas[i]) {
            printf("Bairro %d (%s):\n", cidade->linhas[i]->bairro->id,
                   cidade->linhas[i]->bairro->nomeDoBairro);

            No *atual = cidade->linhas[i];
            while (atual) {
                printf("  - ");
                switch (atual->tipo_servico) {
                    case BOMBEIRO:
                        printf("Bombeiro: %s\n", ((bombeiros*)atual->servico)->nome);
                        break;
                    case HOSPITAL:
                        printf("Hospital: %s\n", ((hospital*)atual->servico)->nome);
                        break;
                    case POLICIA:
                        printf("Policia: %s\n", ((policia*)atual->servico)->nome);
                        break;
                }

                atual = atual->direita;
            }
        }
    }

    // Imprime por tipos de serviço (colunas)
    printf("\nPor Tipos de Servico:\n");
    const char *nomes_tipos[] = {"Bombeiro", "Hospital", "Ambulancia", "Policia"};

    for (int i = 0; i < MAX_SERVICOS; i++) {
        if (cidade->colunas[i]) {
            printf("%s:\n", nomes_tipos[i]);

            No *atual = cidade->colunas[i];
            while (atual) {
                printf("  - Bairro %d (%s)\n", atual->bairro->id,
                       atual->bairro->nomeDoBairro);
                atual = atual->baixo;
            }
        }
    }
}


// Libera toda a memória alocada para a cidade e seus serviços
void destruir_cidade(Cidade *cidade) {
    if (!cidade) return;

    // Array auxiliar para evitar liberar um mesmo serviço mais de uma vez (evitar dupla liberação)
    void **servicos_liberados = malloc(MAX_BAIRROS * MAX_SERVICOS * sizeof(void*));
    int num_servicos_liberados = 0;

    // Percorre as linhas e libera os nós e serviços
    for (int i = 0; i < MAX_BAIRROS; i++) {
        No *atual = cidade->linhas[i];
        while (atual) {
            No *prox = atual->direita;

            // Verifica se o serviço já foi liberado
            int ja_liberado = 0;
            for (int j = 0; j < num_servicos_liberados; j++) {
                if (servicos_liberados[j] == atual->servico) {
                    ja_liberado = 1;
                    break;
                }
            }

            // Libera o serviço se ainda não foi liberado
            if (!ja_liberado) {
                switch (atual->tipo_servico) {
                    case BOMBEIRO: free((bombeiros*)atual->servico); break;
                    case HOSPITAL: free((hospital*)atual->servico); break;
                    case POLICIA: free((policia*)atual->servico); break;

                }
                servicos_liberados[num_servicos_liberados++] = atual->servico;
            }

            free(atual);
            atual = prox;
        }
        cidade->linhas[i] = NULL;
    }

    // Limpa as colunas
    for (int i = 0; i < MAX_SERVICOS; i++) {
        cidade->colunas[i] = NULL;
    }

    free(servicos_liberados);
}


// Busca um serviço específico em um bairro
No* buscar_servico(Cidade *cidade, int id_bairro, TipoServico tipo) {
    if (!cidade || id_bairro < 0 || id_bairro >= MAX_BAIRROS) return NULL;

    // Percorre a linha do bairro procurando pelo tipo de serviço
    No *atual = cidade->linhas[id_bairro];
    while (atual) {
        if (atual->tipo_servico == tipo) {
            return atual;
        }
        atual = atual->direita;
    }

    return NULL; // Não encontrado
}

// Função de busca por nome de serviço
No* buscar_servico_por_nome(Cidade *cidade, const char *nome) {
    if (!cidade || !nome) return NULL;

    // Percorre todas as colunas
    for (int tipo = 0; tipo < MAX_SERVICOS; tipo++) {
        No *atual = cidade->colunas[tipo];
        while (atual) {
            const char *nome_servico = NULL;

            switch (tipo) {
                case BOMBEIRO: nome_servico = ((bombeiros*)atual->servico)->nome; break;
                case HOSPITAL: nome_servico = ((hospital*)atual->servico)->nome; break;
                case POLICIA: nome_servico = ((policia*)atual->servico)->nome; break;

            }

            if (nome_servico && strcmp(nome_servico, nome) == 0) {
                return atual;
            }

            atual = atual->baixo;
        }
    }

    return NULL; // Não encontrado
}
