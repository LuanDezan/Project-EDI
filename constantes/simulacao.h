// simulacao.h
#ifndef SIMULACAO_H
#define SIMULACAO_H

void adicionarTarefa(Ocorrencia* oc, int servico);
void marcarTarefaConcluida(Ocorrencia* oc, int servico, const char* horario);
bool todasTarefasConcluidas(const Ocorrencia* oc);

Ocorrencia *criarOcorrenciaAleatoria(Bairro tabelaHashBairro[], Cidadao *tabelaHashCidadao[], bool ehNova, int maxHash);

void cadastrarBairro();
void inicializarCidade();
void criarServicosParaBairros();
void verificarConexoes();
void gerarCidadaosAleatorios();
void processarFilas();

Cidadao* selecionar_cidadao_aleatorio(Cidadao *tabelaHashCidadao[], int maxHash);
float calcular_distancia(float lat1, float lon1, float lat2, float lon2);
Bairro* encontrar_bairro_mais_proximo(float lat, float lon, Bairro tabela[], int maxHash);

void atualizar_localizacao_cidadaos(Cidadao *tabelaHashCidadao[], int maxHash);


bool remover_ultima(historicoOcorrencias *h);
const Ocorrencia* consultar_ultima(const historicoOcorrencias *h);



void inicializarDescritorFila(DescritorFila *fila);

void exibirEstadoInicialFilas(DescritorFila *filas[NUM_BAIRROS][NUM_SERVICOS], Bairro bairros[]);

void filaAtendimento(DescritorFila *fila, Ocorrencia *oc);



Bairro* buscar_bairro_por_id(Bairro tabela[], int idBairro, int maxHash);

Ocorrencia *criarOcorrenciaAleatoria(Bairro tabelaHashBairro[], Cidadao *tabelaHashCidadao[],
                                     bool ehNova, int maxHash);

// Adicione estes protótipos para o histórico
void inicializar_historico(historicoOcorrencias *h);
bool registrar_ocorrencia(historicoOcorrencias *h, const char *h_chegada, const char *h_atendimento, int gravidade, int tipo, Bairro *bairro);
void imprimir_historico(const historicoOcorrencias *h);
Ocorrencia* buscar_ocorrencia_por_id(historicoOcorrencias *h, int id);
void imprimir_detalhes_ocorrencia(Ocorrencia *oc);
void destruir_historico(historicoOcorrencias *h);

// Protótipos atualizados com histórico
int gerarOcorrenciasIniciais(Bairro bairros[], Cidadao *cidadaos[],
                            DescritorFila *filas[NUM_BAIRROS][NUM_SERVICOS],
                            int maxHash);

void processarCicloAtendimento(DescritorFila *filas[NUM_BAIRROS][NUM_SERVICOS], Bairro tabelaHashBairro[], Cidadao *tabelaHashCidadao[], historicoOcorrencias *historico);



#endif // SIMULACAO_H

