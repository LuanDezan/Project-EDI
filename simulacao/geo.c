#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include"../constantes/constantes.h"
#define MAXHASH      20
#define NUM_BAIRROS  4
#define NUM_SERVICOS 3
#define TEMPO_TICK   10
#include <float.h>
#include <math.h>

#include <math.h>
#include <float.h>


void inicializarCidade(Cidade *cidade, Bairro tabelaHashBairro[], int maxHash) {
    // Inicializa linhas e colunas com NULL
    for(int i = 0; i < MAX_BAIRROS; i++) {
        cidade->linhas[i] = NULL;
    }
    for(int j = 0; j < MAX_SERVICOS; j++) {
        cidade->colunas[j] = NULL;
    }

for(int hash_idx = 0; hash_idx < maxHash; hash_idx++) {
    Bairro* b = tabelaHashBairro[hash_idx].prox;
    while(b != NULL) {
        for(int servico_id = 0; servico_id < MAX_SERVICOS; servico_id++) {
            No* novo = (No*)malloc(sizeof(No));
            novo->bairro = b;
            novo->tipo_servico = servico_id;
            novo->servico = NULL;
            novo->direita = NULL;
            novo->baixo = NULL;

                // Inserir na linha do bairro
                novo->direita = cidade->linhas[b->id % MAX_BAIRROS];
                cidade->linhas[b->id % MAX_BAIRROS] = novo;

                // Inserir na coluna do serviço
                novo->baixo = cidade->colunas[servico_id];
                cidade->colunas[servico_id] = novo;
            }
            b = b->prox;
        }
    }
}

// Funao para selecionar um cidadzao aleatorio
Cidadao* selecionar_cidadao_aleatorio(Cidadao *tabelaHashCidadao[], int maxHash) {
    int total = 0;
    // Conta o num total de cidadaos
    for (int i = 0; i < maxHash; i++) {
        Cidadao *c = tabelaHashCidadao[i];
        while (c != NULL) {
            total++;
            c = c->prox;
        }
    }

    if (total == 0) return NULL;

    int indice_aleatorio = rand() % total;
    Cidadao *selecionado = NULL;

    // Percorre a tabela hash para encontrar o cidadao no indice aleatorio
    for (int i = 0; i < maxHash; i++) {
        Cidadao *c = tabelaHashCidadao[i];
        while (c != NULL) {
            if (indice_aleatorio == 0) {
                selecionado = c;
                break;
            }
            indice_aleatorio--;
            c = c->prox;
        }
        if (selecionado != NULL) break;
    }

    return selecionado;
}

// Funcao para calcular distancia entre coordenadas
float calcular_distancia(float lat1, float lon1, float lat2, float lon2) {
    // converso para radianos
    float lat1_rad = lat1 * M_PI / 180.0f;
    float lon1_rad = lon1 * M_PI / 180.0f;
    float lat2_rad = lat2 * M_PI / 180.0f;
    float lon2_rad = lon2 * M_PI / 180.0f;

    // diferenças
    float dlat = lat2_rad - lat1_rad;
    float dlon = lon2_rad - lon1_rad;

    // formula de Haversine
    float a = sin(dlat/2) * sin(dlat/2) +
              cos(lat1_rad) * cos(lat2_rad) *
              sin(dlon/2) * sin(dlon/2);

    float c = 2 * atan2(sqrt(a), sqrt(1-a));

    // rraioo
    const float R = 6371.0f;
    return R * c;
}

// Funcao para encontrar o bairro mais prerto
Bairro* encontrar_bairro_mais_proximo(float lat, float lon, Bairro tabela[], int maxHash) {
    Bairro *mais_proximo = NULL;
    float menor_distancia = FLT_MAX;

    for (int i = 0; i < maxHash; i++) {
        Bairro *b = tabela[i].prox;
        while (b != NULL) {
            float distancia = calcular_distancia(lat, lon, b->latitude, b->longitude);
            if (distancia < menor_distancia) {
                menor_distancia = distancia;
                mais_proximo = b;
            }
            b = b->prox;
        }
    }
    return mais_proximo;
}

void gerarCidadaosAleatorios(Cidadao *tabelaHashCidadao[], Bairro tabelaHashBairro[], int maxHash) {
    const char *nomes[] = {
        "Joao Silva", "Maria Souza", "Carlos Oliveira", "Ana Costa",
        "Jonathan Calleri", "Lucas Moura", "Luciano Neves", "Micaella Fusca",
        "Larissa Saad", "Marcela Andrade"
    };
    const char *cpfs[] = {
        "111.222.333-44", "222.333.444-55", "333.444.555-66", "444.555.666-77",
        "123.320.223-88", "222.345.245-60", "908.379.239-00", "789.302.102-12",
        "283.109.102-44", "334.890.467-11"
    };
    int totalNomes = sizeof(nomes) / sizeof(nomes[0]);  // = 10
    int cidadaoId = 1;

    for (int i = 0; i < totalNomes; i++) {
        // Cria o cidad
        Cidadao *novo = malloc(sizeof(Cidadao));
        if (!novo) continue;

        // Nome e CPF unicos
        const char *nomeAtual = nomes[i];
        const char *cpfAtual  = cpfs[i];
        strncpy(novo->nomeCidadao, nomeAtual, sizeof(novo->nomeCidadao));
        strncpy(novo->CPF,        cpfAtual,  sizeof(novo->CPF));
        novo->CPF[sizeof(novo->CPF)-1] = '\0';

        //  email: NomeSobrenome@gmail.com (sem espacos)
        char emailBuf[100] = "";
        for (const char *p = nomeAtual; *p; p++) {
            if (*p != ' ') strncat(emailBuf, (char[]){*p,0}, 1);
        }
        strncat(emailBuf, "@gmail.com", sizeof(emailBuf) - strlen(emailBuf) - 1);
        strncpy(novo->email, emailBuf, sizeof(novo->email));
        novo->email[sizeof(novo->email)-1] = '\0';

        // vai distribuindo em bairros em circulo
        int bairroId = (i % NUM_BAIRROS) + 1;
        Bairro *b = buscar_bairro_por_id(tabelaHashBairro, bairroId, maxHash);
        if (!b) {
            // fallback ao primeiro bairro
            b = buscar_bairro_por_id(tabelaHashBairro, 1, maxHash);
        }
        novo->idBairroDeResidencia = b->id;
        novo->latitude_atual  = b->latitude  + (rand() % 100 - 50) / 1000.0;
        novo->longitude_atual = b->longitude + (rand() % 100 - 50) / 1000.0;

        // prrenche campos
        novo->id = cidadaoId++;
        novo->emMovimento     = rand() % 3;
        novo->ultimaAtualizacao = time(NULL);
        novo->prox = NULL;

        // coloca na hash
        int hash = novo->id % maxHash;
        novo->prox = tabelaHashCidadao[hash];
        tabelaHashCidadao[hash] = novo;

        // log
        printf(
            "Cidadao gerado: %s | Bairro: %d | Coord: (%.4f, %.4f) "
            "| CPF: %s | EMAIL: %s\n\n",
            novo->nomeCidadao,
            novo->idBairroDeResidencia,
            novo->latitude_atual,
            novo->longitude_atual,
            novo->CPF,
            novo->email
        );
    }
}







void atualizar_localizacao_cidadaos(Cidadao *tabelaHashCidadao[], int maxHash) {
    for (int i = 0; i < maxHash; i++) {
        Cidadao *c = tabelaHashCidadao[i];
        while (c != NULL) {
            if (c->emMovimento > 0) {
                // fator de movimento baseado no tipo (caminhada/veículo)
                float fator = c->emMovimento == 1 ? 0.001f : 0.01f;

                // atualiza posicao com pequena variacao aleatoria
                c->latitude_atual += (rand() % 200 - 100) * fator / 1000.0f;
                c->longitude_atual += (rand() % 200 - 100) * fator / 1000.0f;

                // Atualiza tempo
                c->ultimaAtualizacao = time(NULL);
            }
            c = c->prox;
        }
    }
}

void criarServicosParaBairros(Cidade *cidade, Bairro tabelaHashBairro[], int maxHash) {
    for(int hash_idx = 0; hash_idx < maxHash; hash_idx++) {
        Bairro* b = tabelaHashBairro[hash_idx].prox;
        while(b != NULL) {
            printf("Criando servicos para bairro %d: %s\n", b->id, b->nomeDoBairro);

            for (int servico_id = 0; servico_id < NUM_SERVICOS; servico_id++) {
                No* atual = cidade->linhas[b->id % MAX_BAIRROS];
                bool encontrado = false;

                while(atual != NULL && !encontrado) {
                    if(atual->bairro && atual->bairro->id == b->id) {
                        // Criar serviço apenas para o tipo correspondente
                        if(atual->tipo_servico == servico_id) {
                            switch(servico_id) {
                                case BOMBEIRO: {
                                    bombeiros* bmb = malloc(sizeof(bombeiros));
                                    bmb->id = b->id * 10 + servico_id;
                                    sprintf(bmb->nome, "Bombeiros %s", b->nomeDoBairro);
                                    atual->servico = bmb;
                                    encontrado = true;
                                    break;
                                }
                                case HOSPITAL: {
                                    hospital* hosp = malloc(sizeof(hospital));
                                    hosp->id = b->id * 10 + servico_id;
                                    sprintf(hosp->nome, "Hospital %s", b->nomeDoBairro);
                                    atual->servico = hosp;
                                    encontrado = true;
                                    break;
                                }
                                case POLICIA: {
                                    policia* pol = malloc(sizeof(policia));
                                    pol->id = b->id * 10 + servico_id;
                                    sprintf(pol->nome, "Delegacia %s", b->nomeDoBairro);
                                    atual->servico = pol;
                                    encontrado = true;
                                    break;
                                }
                            }
                        }
                    }
                    atual = atual->direita;
                }
            }
            b = b->prox;
        }
    }
}


void verificarConexoes(Cidade *cidade) {
    printf("\nVerificacao da Estrutura:\n");
    for (int i = 0; i < MAX_BAIRROS; i++) {
        int contador = 0;
        No *atual = cidade->linhas[i];
        // conta quantos servicos ligados a esse bairro
        while (atual) {
            contador++;
            atual = atual->direita;
        }
        // i+1 para exibir bairros de 1..4
        printf("Bairro %d tem %d servicos conectados\n", i + 1, contador);
    }
}

