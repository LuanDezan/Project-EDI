#ifndef CONSTANTE
#define CONSTANTE

#define MAX 40
#define HASH_SIZE 20
#define MAX_BAIRROS 4
#define MAX_SERVICOS 4 // Atualizado para 4 serviços
#define MAX_HISTORICO 100
#include<stdbool.h>
#include<time.h>
#define TEMPO_TICK   10
#define MAXHASH      20
#define NUM_BAIRROS  4
#define NUM_SERVICOS 4  // Atualizado: hospital, polícia, bombeiro e ambulância

extern time_t tempoGlobal;
extern const char *NOME_SERVICOS[];
extern const int TEMPOS_ATENDIMENTO[];
extern time_t tempoGlobal;


typedef struct Bairro {
    int id;
    char nomeDoBairro[30];
    float latitude;     // coordenada geográfica
    float longitude;    //  coordenada geográfica
    struct Bairro *prox;
} Bairro;

typedef struct Cidadao {
    char CPF[15];
    char nomeCidadao[35];
    char email[40];
    int id;
    int idBairroDeResidencia;
    float latitude_atual;  // NOVO: localização atual
    float longitude_atual; // NOVO: localização atual
    int emMovimento;       // NOVO: 0=parado,1=caminhando,2=veículo
    time_t ultimaAtualizacao; // NOVO
     bool          emFila;
      bool emOcorrencia;
    struct Cidadao *prox;
} Cidadao;

typedef enum {
    INCENDIO_RESIDENCIAL = 40,    // gravidade 4 - bombeiro + hospital + polícia
    INCENDIO_COMERCIAL   = 50,    // gravidade 5 - bombeiro + hospital + polícia
    INCENDIO_VEICULAR    = 40,    // gravidade 4 - bombeiro + hospital + polícia

    ACIDENTE_TRANSITO    = 30,    // gravidade 3 - polícia + bombeiro
    ACIDENTE_VITIMA      = 40,    // gravidade 4 - polícia + bombeiro + hospital

    ASSALTO              = 20,    // gravidade 2 - polícia
    FURTO                = 10,    // gravidade 1 - polícia
    HOMICIDIO            = 50,    // gravidade 5 - polícia + hospital
    VIOLENCIA_DOMESTICA  = 30,    // gravidade 3 - polícia + hospital
    PERTURBACAO_SOSSEGO  = 10,    // gravidade 1 - polícia
    TRAFICO_DROGAS       = 30,    // gravidade 3 - polícia
    VANDALISMO           = 10,    // gravidade 1 - polícia

    MAL_SUBITO           = 30,    // gravidade 3 - hospital
    QUEDA_ALTURA         = 30,    // gravidade 3 - hospital + bombeiro
    INTOXICACAO          = 30,    // gravidade 3 - hospital
    PARTO                = 40,    // gravidade 4 - hospital
    PARADA_CARDIACA      = 50,    // gravidade 5 - hospital
    CONVULSAO            = 30,    // gravidade 3 - hospital

    RESGATE_ALTURA       = 40,    // gravidade 4 - bombeiro + hospital
    SALVAMENTO_AQUATICO  = 50,    // gravidade 5 - bombeiro + hospital
    VAZAMENTO_GAS        = 40,    // gravidade 4 - bombeiro + polícia
    DESABAMENTO          = 50,    // gravidade 5 - bombeiro + hospital + polícia
    ANIMAL_RISCO         = 20     // gravidade 2 - bombeiro

} TipoOcorrencia;

typedef struct Tarefa {
    int servico;        // 0=bombeiro, 1=hospital, 2=policia, 3=ambulancia
    bool concluida;     // flag de conclusão da tarefa
    char horarioConclusao[6]; // formato "HH:MM"
    struct Tarefa* prox; // próxima tarefa na lista
} Tarefa;

typedef struct Ocorrencia {
    int id;
    char horarioChegada[6];     // "HH:MM"
    char horarioAtendimento[6]; // "HH:MM"
    int gravidade;              // 1-5
    int tipo;                   // 1-4 (correspondente aos serviços)

    Bairro* bairro;             // Bairro onde será atendida
    Cidadao* cidadao;           // Cidadão que reportou

    Tarefa* tarefas;            // Lista de tarefas necessárias
    char descricao[100];        // Descrição detalhada
    int tarefas_pendentes;
    // Flags para serviços adicionais
    bool requer_hospital;
    bool requer_bombeiro;
    bool requer_policia;
    bool requer_ambulancia;
     bool finalizada;
    struct Ocorrencia* prox;    // Próxima ocorrência na fila
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
    int quantidade;              // Contador de ocorrências
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
    AMBULANCIA  // Novo serviço adicionado
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
    int vazia;      // 0 = em uso, 1 = disponível
    historicoOcorrencias historico;
    struct SAMU *prox;
} SAMU;

typedef struct hospital{
    char nome[MAX];
    int id;
    struct hospital*prox;
    historicoOcorrencias historico;
} hospital;

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

extern DescritorFila *filas[NUM_BAIRROS][NUM_SERVICOS];



#endif // CONSTANTE
