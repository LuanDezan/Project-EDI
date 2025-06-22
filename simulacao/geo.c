#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "../constantes/constantes.h"
#include "../constantes/simulacao.h"

#define MAXHASH 20
#define NUM_BAIRROS 4
#define NUM_SERVICOS 3
#define TEMPO_TICK 10

bool servico_disponivel(Cidade *cidade, int id_bairro, TipoServico tipo) {
    if (id_bairro < 1 || id_bairro > MAX_BAIRROS)
        return false;

    int idx_bairro = id_bairro - 1;

    // Verifica se o bairro existe na cidade
    if (idx_bairro >= MAX_BAIRROS || !cidade->linhas[idx_bairro]) {
        return false;
    }

    No *atual = cidade->linhas[idx_bairro];
    while (atual) {
        if (atual->tipo_servico == tipo)
            return true;
        atual = atual->direita;
    }
    return false;
}

// Encontra o bairro mais próximo com um serviço específico
Bairro* encontrar_bairro_com_servico(Cidade *cidade, float lat, float lon, TipoServico tipo, Bairro tabela[], int maxHash) {
    Bairro *melhor_bairro = NULL;
    float menor_distancia = FLT_MAX;

    for (int i = 0; i < maxHash; i++) {
        Bairro *b = tabela[i].prox;
        while (b) {
            if (servico_disponivel(cidade, b->id, tipo)) {
                float dist = calcular_distancia(lat, lon, b->latitude, b->longitude);
                if (dist < menor_distancia) {
                    menor_distancia = dist;
                    melhor_bairro = b;
                }
            }
            b = b->prox;
        }
    }
    return melhor_bairro;
}

void inicializarCidade(Cidade *cidade, Bairro tabelaHashBairro[], int maxHash) {
    for(int i = 0; i < MAX_BAIRROS; i++) {
        cidade->linhas[i] = NULL;
    }
    for(int j = 0; j < MAX_SERVICOS; j++) {
        cidade->colunas[j] = NULL;
    }
}

Cidadao* selecionar_cidadao_aleatorio(Cidadao *tabelaHashCidadao[], int maxHash) {
    int total = 0;
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

float calcular_distancia(float lat1, float lon1, float lat2, float lon2) {
    float lat1_rad = lat1 * M_PI / 180.0f;
    float lon1_rad = lon1 * M_PI / 180.0f;
    float lat2_rad = lat2 * M_PI / 180.0f;
    float lon2_rad = lon2 * M_PI / 180.0f;

    float dlat = lat2_rad - lat1_rad;
    float dlon = lon2_rad - lon1_rad;

    float a = sin(dlat/2) * sin(dlat/2) +
              cos(lat1_rad) * cos(lat2_rad) *
              sin(dlon/2) * sin(dlon/2);
    float c = 2 * atan2(sqrt(a), sqrt(1-a));

    const float R = 6371.0f;
    return R * c;
}

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

    // CORREÇÃO: totalNomes em vez de totalNoms
    for (int i = 0; i < totalNomes; i++) {
        Cidadao *novo = malloc(sizeof(Cidadao));
        if (!novo) continue;

        const char *nomeAtual = nomes[i];
        const char *cpfAtual  = cpfs[i];
        strncpy(novo->nomeCidadao, nomeAtual, sizeof(novo->nomeCidadao));
        strncpy(novo->CPF,        cpfAtual,  sizeof(novo->CPF));
        novo->CPF[sizeof(novo->CPF)-1] = '\0';

        char emailBuf[100] = "";
        for (const char *p = nomeAtual; *p; p++) {
            if (*p != ' ') strncat(emailBuf, (char[]){*p,0}, 1);
        }
        strncat(emailBuf, "@gmail.com", sizeof(emailBuf) - strlen(emailBuf) - 1);
        strncpy(novo->email, emailBuf, sizeof(novo->email));
        novo->email[sizeof(novo->email)-1] = '\0';

        int bairroId = (i % NUM_BAIRROS) + 1;
        Bairro *b = buscar_bairro_por_id(tabelaHashBairro, bairroId, maxHash);
        if (!b) {
            b = buscar_bairro_por_id(tabelaHashBairro, 1, maxHash);
        }
        novo->idBairroDeResidencia = b->id;
        novo->latitude_atual  = b->latitude  + (rand() % 100 - 50) / 1000.0;
        novo->longitude_atual = b->longitude + (rand() % 100 - 50) / 1000.0;

        novo->id = cidadaoId++;
        novo->emMovimento     = rand() % 3;
        novo->ultimaAtualizacao = time(NULL);
        novo->prox = NULL;

        int hash = novo->id % maxHash;
        novo->prox = tabelaHashCidadao[hash];
        tabelaHashCidadao[hash] = novo;

printf("\n┌───────────────────── NOVO CIDADAO GERADO ────────────────────┐\n");
printf("│ %-58s   │\n", novo->nomeCidadao);
printf("├──────────────────────────────────────────────────────────────┤\n");
printf("│   Bairro: %-50d │\n", novo->idBairroDeResidencia);
printf("│   Coordenadas: (%-8.4f, %-8.4f)                          │\n",
       novo->latitude_atual, novo->longitude_atual);
printf("│   CPF: %-54s│\n", novo->CPF);
printf("│   Email: %-40s            │\n", novo->email);
printf("└──────────────────────────────────────────────────────────────┘\n\n\n");

    }
}

void atualizar_localizacao_cidadaos(Cidadao *tabelaHashCidadao[], int maxHash) {
    for (int i = 0; i < maxHash; i++) {
        Cidadao *c = tabelaHashCidadao[i];
        while (c != NULL) {
            if (c->emMovimento > 0) {
                float fator = c->emMovimento == 1 ? 0.001f : 0.01f;
                c->latitude_atual += (rand() % 200 - 100) * fator / 1000.0f;
                c->longitude_atual += (rand() % 200 - 100) * fator / 1000.0f;
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
            // Serviços fixos (hospital e polícia em todos bairros)
            hospital* hosp = malloc(sizeof(hospital));
            hosp->id = b->id * 10 + HOSPITAL;
            inserir_servico(cidade, b, hosp, HOSPITAL);

            policia* pol = malloc(sizeof(policia));
            pol->id = b->id * 10 + POLICIA;
            inserir_servico(cidade, b, pol, POLICIA);

            // Serviços aleatórios (bombeiro e ambulância - 50% chance cada)
            if (rand() % 2 == 0) {
                bombeiros* bmb = malloc(sizeof(bombeiros));
                bmb->id = b->id * 10 + BOMBEIRO;
                inserir_servico(cidade, b, bmb, BOMBEIRO);
            }

            if (rand() % 2 == 0) {
                SAMU* amb = malloc(sizeof(SAMU));
                amb->id = b->id * 10 + AMBULANCIA;
                inserir_servico(cidade, b, amb, AMBULANCIA);
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
        while (atual) {
            contador++;
            atual = atual->direita;
        }
        printf("Bairro %d tem %d servicos conectados\n", i + 1, contador);
    }
}





