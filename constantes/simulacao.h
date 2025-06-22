// simulacao.h
#ifndef SIMULACAO_H
#define SIMULACAO_H

void adicionarTarefa(Ocorrencia* oc, int servico);
void marcarTarefaConcluida(Ocorrencia* oc, int servico, const char* horario);
bool todasTarefasConcluidas(const Ocorrencia* oc);
void exibirEstadoPosCiclo(DescritorFila *filas[][NUM_SERVICOS], Bairro tabelaHashBairro[]);


void cadastrarBairro();
void inicializarCidade();
void criarServicosParaBairros();
void verificarConexoes();
void gerarCidadaosAleatorios();
void processarFilas();

Cidadao* selecionar_cidadao_aleatorio(Cidadao *tabelaHashCidadao[], int maxHash);

Bairro* encontrar_bairro_mais_proximo(float lat, float lon, Bairro tabela[], int maxHash);

void atualizar_localizacao_cidadaos(Cidadao *tabelaHashCidadao[], int maxHash);


bool remover_ultima(historicoOcorrencias *h);
const Ocorrencia* consultar_ultima(const historicoOcorrencias *h);


void inicializarDescritorFila(DescritorFila *fila);

void exibirEstadoInicialFilas(DescritorFila *filas[NUM_BAIRROS][NUM_SERVICOS], Bairro bairros[]);

void filaAtendimento(DescritorFila *fila, Ocorrencia *oc);



Bairro* buscar_bairro_por_id(Bairro tabela[], int idBairro, int maxHash);
;

// Adicione estes protótipos para o histórico
void inicializar_historico(historicoOcorrencias *h);
bool registrar_ocorrencia(historicoOcorrencias *h, Ocorrencia *oc, const char *horarioConclusao);
void imprimir_historico(const historicoOcorrencias *h);
Ocorrencia* buscar_ocorrencia_por_id(historicoOcorrencias *h, int id);
void imprimir_detalhes_ocorrencia(Ocorrencia *oc);
void destruir_historico(historicoOcorrencias *h);

// Protótipos atualizados com histórico
int gerarOcorrenciasIniciais(Cidade *cidade, Bairro bairros[], Cidadao *cidadaos[], DescritorFila *filas[][NUM_SERVICOS], int maxHash);
void processarCicloAtendimento(DescritorFila *filas[][NUM_SERVICOS], Cidade *cidade, Bairro tabelaHashBairro[], Cidadao *tabelaHashCidadao[], historicoOcorrencias *historico);
float calcular_distancia(float lat1, float lon1, float lat2, float lon2);

void exibirOcorrenciaDetalhada(Ocorrencia* oc);
Ocorrencia *criarOcorrenciaAleatoria(Cidade *cidade, Bairro tabelaHashBairro[], Cidadao *tabelaHashCidadao[], bool ehNova, int maxHash);

// Declarar novas funções
bool servico_disponivel(Cidade *cidade, int id_bairro, TipoServico tipo);
Bairro* encontrar_bairro_com_servico(Cidade *cidade, float lat, float lon, TipoServico tipo, Bairro tabela[], int maxHash);


No* criar_no(Bairro *bairro, void *servico, TipoServico tipo);
void inicializar_cidade(Cidade *cidade);
void inserir_servico(Cidade *cidade, Bairro *bairro, void *servico, TipoServico tipo);

void imprimir_lista_cruzada(Cidade *cidade);
void destruir_cidade(Cidade *cidade);
No* buscar_servico(Cidade *cidade, int id_bairro, TipoServico tipo);
No* buscar_servico_por_nome(Cidade *cidade, const char *nome);




#endif // SIMULACAO_H


