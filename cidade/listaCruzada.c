#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"../bairro/Bairro.h"
#include"../constantes/constantes.h"
#include"../historico/historico.c"

// Constantes para limites máximos
#define MAX_BAIRROS 5
#define MAX_SERVICOS 4  // Bombeiro, Hospital, Ambulância, Polícia
#define MAX_HISTORICO 100

// Enumeração para os tipos de serviços
typedef enum {
    BOMBEIRO,
    HOSPITAL,
    AMBULANCIA,
    POLICIA
} TipoServico;

typedef struct policia{
    int id;
    char nome[MAX];
    struct policia*prox;
    historicoOcorrencias historico;

}policia;

typedef struct bombeiros{
    int id;
    char nome[MAX];
    struct bombeiros *prox;
    historicoOcorrencias historico;
}bombeiros;

typedef struct SAMU{
    int id,
        prioridade, //Prioridade da chamada ativa de atendimento Pensei em 1,2 e 3 sendo o menor numero menos prioridade
        vazia;      // Se a ambulância está sendo usada = 0, se esta vazia = 1
        historicoOcorrencias historico;
    struct SAMU *prox;
}SAMU;

typedef struct hospital{
    char nome[MAX];
    int id;
    struct hospital*prox;
    historicoOcorrencias historico;

}hospital;


// Estrutura do nó da lista cruzada
typedef struct No {
    struct Bairro *bairro;     // Ponteiro para o bairro na tabela hash
    void *servico;             // Ponteiro genérico para o serviço
    TipoServico tipo_servico;
    struct No *direita;        // Próximo serviço no mesmo bairro
    struct No *baixo;          // Próximo bairro com mesmo serviço
} No;


// Estrutura que representa toda a cidade
typedef struct {
    No *linhas[MAX_BAIRROS];     // Cada linha representa um bairro
    No *colunas[MAX_SERVICOS];   // Cada coluna representa um tipo de serviço
} Cidade;


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

// Insere um serviço na estrutura da cidade (lista cruzada)
void inserir_servico(Cidade *cidade, Bairro *bairro, void *servico, TipoServico tipo) {
    if (!cidade || !bairro || !servico || bairro->id < 0 || bairro->id >= MAX_BAIRROS) {
        printf("Parametros invalidos!\n");
        return;
    }

    No *novo = criar_no(bairro, servico, tipo);
    if (!novo) return;

    // Insere na linha correspondente ao bairro (ordenado por tipo de serviço)
    No **atual_linha = &(cidade->linhas[bairro->id]);

    while (*atual_linha && (*atual_linha)->tipo_servico < tipo) {
        atual_linha = &((*atual_linha)->direita);
    }

    novo->direita = *atual_linha;
    *atual_linha = novo;

    // Insere na coluna correspondente ao tipo de serviço (ordenado por id do bairro)
    No **atual_coluna = &(cidade->colunas[tipo]);

    while (*atual_coluna && (*atual_coluna)->bairro->id < bairro->id) {
        atual_coluna = &((*atual_coluna)->baixo);
    }

    novo->baixo = *atual_coluna;
    *atual_coluna = novo;
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
                    case AMBULANCIA:
                        printf("Ambulancia: %d (Prioridade: %d)\n",
                               ((SAMU*)atual->servico)->id,
                               ((SAMU*)atual->servico)->prioridade);
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
                    case AMBULANCIA: free((SAMU*)atual->servico); break;
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
                case AMBULANCIA: nome_servico = "SAMU"; break;
            }

            if (nome_servico && strcmp(nome_servico, nome) == 0) {
                return atual;
            }

            atual = atual->baixo;
        }
    }

    return NULL; // Não encontrado
}


// Exemplo de uso na simulação
int main() {
    Cidade cidade;
    inicializar_cidade(&cidade);

    // Criando alguns bairros (normalmente viriam da tabela hash)
    Bairro centro = {0, "Centro", NULL};
    Bairro jardim = {1, "Jardim das Flores", NULL};
    Bairro nova = {2, "Vila Nova", NULL};
    Bairro machadinho = {3, "Vila Machadinho", NULL};

    // Criando serviços
    bombeiros b1 = {1, "Quartel Central", NULL};
    hospital h1 = {"Hospital Municipal", 1, NULL};
    SAMU samu = {1, 2, 0, NULL};
    policia p1 = {1, "DP Central", NULL};

    // Registro de ocorrências no histórico
    inicializar_historico(&samu.historico);
    registrar_ocorrencia(&samu.historico, "09:00", "09:15", 2, AMBULANCIA, &centro);
    registrar_ocorrencia(&samu.historico, "09:30", "09:45", 1, AMBULANCIA, &jardim);

    // Inserção dos serviços na cidade
    inserir_servico(&cidade, &centro, &b1, BOMBEIRO);
    inserir_servico(&cidade, &centro, &h1, HOSPITAL);
    inserir_servico(&cidade, &jardim, &p1, POLICIA);
    inserir_servico(&cidade, &nova, &samu, AMBULANCIA);
    inserir_servico(&cidade, &machadinho, &h1, HOSPITAL);
    inserir_servico(&cidade, &machadinho, &samu, AMBULANCIA);
    inserir_servico(&cidade, &machadinho, &b1, BOMBEIRO);

    // Imprimindo a lista cruzada
    imprimir_lista_cruzada(&cidade);
    printf("\n");
    imprimir_historico(&samu.historico);

    return 0;
}
