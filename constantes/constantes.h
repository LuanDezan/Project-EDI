#ifndef CONSTANTE
#define CONSTANTE

#define MAX 40
#define HASH_SIZE 20
#define MAX_BAIRROS 4
#define MAX_SERVICOS 4 // Atualizado para 4 servi�os
#define MAX_HISTORICO 100
#include<stdbool.h>
#include<time.h>
#define TEMPO_TICK   10
#define MAXHASH      20
#define NUM_BAIRROS  4
#define NUM_SERVICOS 4  // Atualizado: hospital, pol�cia, bombeiro e ambul�ncia

extern time_t tempoGlobal;
extern const char *NOME_SERVICOS[];
extern const int TEMPOS_ATENDIMENTO[];
extern time_t tempoGlobal;


typedef struct Bairro {
    int id;
    char nomeDoBairro[30];
    float latitude;     // coordenada geogr�fica
    float longitude;    //  coordenada geogr�fica
    struct Bairro *prox;
} Bairro;

typedef struct Cidadao {
    char CPF[15];
    char nomeCidadao[35];
    char email[40];
    int id;
    int idBairroDeResidencia;
    float latitude_atual;  // NOVO: localiza��o atual
    float longitude_atual; // NOVO: localiza��o atual
    int emMovimento;       // NOVO: 0=parado,1=caminhando,2=ve�culo
    time_t ultimaAtualizacao; // NOVO
     bool          emFila;
      bool emOcorrencia;
    struct Cidadao *prox;
} Cidadao;

typedef enum {
    INCENDIO_RESIDENCIAL = 40,    // gravidade 4 - bombeiro + hospital + pol�cia
    INCENDIO_COMERCIAL   = 50,    // gravidade 5 - bombeiro + hospital + pol�cia
    INCENDIO_VEICULAR    = 40,    // gravidade 4 - bombeiro + hospital + pol�cia

    ACIDENTE_TRANSITO    = 30,    // gravidade 3 - pol�cia + bombeiro
    ACIDENTE_VITIMA      = 40,    // gravidade 4 - pol�cia + bombeiro + hospital

    ASSALTO              = 20,    // gravidade 2 - pol�cia
    FURTO                = 10,    // gravidade 1 - pol�cia
    HOMICIDIO            = 50,    // gravidade 5 - pol�cia + hospital
    VIOLENCIA_DOMESTICA  = 30,    // gravidade 3 - pol�cia + hospital
    PERTURBACAO_SOSSEGO  = 10,    // gravidade 1 - pol�cia
    TRAFICO_DROGAS       = 30,    // gravidade 3 - pol�cia
    VANDALISMO           = 10,    // gravidade 1 - pol�cia

    MAL_SUBITO           = 30,    // gravidade 3 - hospital
    QUEDA_ALTURA         = 30,    // gravidade 3 - hospital + bombeiro
    INTOXICACAO          = 30,    // gravidade 3 - hospital
    PARTO                = 40,    // gravidade 4 - hospital
    PARADA_CARDIACA      = 50,    // gravidade 5 - hospital
    CONVULSAO            = 30,    // gravidade 3 - hospital

    RESGATE_ALTURA       = 40,    // gravidade 4 - bombeiro + hospital
    SALVAMENTO_AQUATICO  = 50,    // gravidade 5 - bombeiro + hospital
    VAZAMENTO_GAS        = 40,    // gravidade 4 - bombeiro + pol�cia
    DESABAMENTO          = 50,    // gravidade 5 - bombeiro + hospital + pol�cia
    ANIMAL_RISCO         = 20     // gravidade 2 - bombeiro

} TipoOcorrencia;

typedef struct Tarefa {
    int servico;        // 0=bombeiro, 1=hospital, 2=policia, 3=ambulancia
    bool concluida;     // flag de conclus�o da tarefa
    char horarioConclusao[6]; // formato "HH:MM"
    struct Tarefa* prox; // pr�xima tarefa na lista
} Tarefa;

typedef struct Ocorrencia {
    int id;
    char horarioChegada[6];     // "HH:MM"
    char horarioAtendimento[6]; // "HH:MM"
    int gravidade;              // 1-5
    int tipo;                   // 1-4 (correspondente aos servi�os)

    Bairro* bairro;             // Bairro onde ser� atendida
    Cidadao* cidadao;           // Cidad�o que reportou

    Tarefa* tarefas;            // Lista de tarefas necess�rias
    char descricao[100];        // Descri��o detalhada
    int tarefas_pendentes;
    // Flags para servi�os adicionais
    bool requer_hospital;
    bool requer_bombeiro;
    bool requer_policia;
    bool requer_ambulancia;
     bool finalizada;
    struct Ocorrencia* prox;    // Pr�xima ocorr�ncia na fila
    char motivoEscolhaBairro[50];
} Ocorrencia;


typedef struct {
    Ocorrencia *ocorrencia;
    int tempo_restante;
    char horario_inicio[6];
      int tarefasCount;
} Atendimento;

extern Atendimento atendimentos_em_curso[NUM_BAIRROS][NUM_SERVICOS];

// funcoes de manipulacao de tarefas
void adicionarTarefa(Ocorrencia* oc, int servico);
bool todasTarefasConcluidas(const Ocorrencia* oc);
void marcarTarefaConcluida(Ocorrencia* oc, int servico, const char* horario);

typedef struct {
    Ocorrencia *topo;            // Topo da pilha
    int quantidade;              // Contador de ocorr�ncias
    int proximo_id;              // Auto-incremento para IDs
} historicoOcorrencias;

typedef struct NoFila {
    Ocorrencia *ocorrencia;
    struct NoFila *prox;
} NoFila;

typedef struct DescritorFila {
    NoFila *inicio;
    NoFila *fim;
    int tamanho;
} DescritorFila;

typedef enum {
    BOMBEIRO,
    HOSPITAL,
    POLICIA,
    AMBULANCIA  // Novo servi�o adicionado
} TipoServico;

typedef struct policia{
    int id;
    char nome[MAX];
    struct policia*prox;
    historicoOcorrencias historico;
} policia;

typedef struct bombeiros{
    int id;
    char nome[MAX];
    struct bombeiros *prox;
    int localizacao;
    historicoOcorrencias historico;
} bombeiros;

typedef struct SAMU{
    int id;
    int prioridade; // Prioridade da chamada ativa de atendimento
    int vazia;      // 0 = em uso, 1 = dispon�vel
    historicoOcorrencias historico;
    struct SAMU *prox;
} SAMU;

typedef struct hospital{
    char nome[MAX];
    int id;
    struct hospital*prox;
    historicoOcorrencias historico;
} hospital;

// Estrutura do n� da lista cruzada
typedef struct No {
    struct Bairro *bairro;     // Ponteiro para o bairro na tabela hash
    void *servico;             // Ponteiro gen�rico para o servi�o
    TipoServico tipo_servico;
    struct No *direita;        // Pr�ximo servi�o no mesmo bairro
    struct No *baixo;          // Pr�ximo bairro com mesmo servi�o
} No;

// Estrutura que representa toda a cidade
typedef struct {
    No *linhas[MAX_BAIRROS];     // Cada linha representa um bairro
    No *colunas[MAX_SERVICOS];   // Cada coluna representa um tipo de servi�o
} Cidade;

extern DescritorFila *filas[NUM_BAIRROS][NUM_SERVICOS];



#endif // CONSTANTE
