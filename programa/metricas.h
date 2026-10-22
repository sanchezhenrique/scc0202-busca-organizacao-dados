#ifndef METRICAS_H
#define METRICAS_H

#include <stdbool.h>
#include <stdlib.h>

/* Contagens referem-se aos registros de dados, sem incluir a linha de cabecalho. */
typedef struct {
  size_t registros_entrada;
  size_t registros_aceitos;
  size_t registros_saida;
  double carga_ms;
  double busca_ms;
  double insercao_ms;
  double escrita_ms;
} METRICAS;

/* Instante monotonicamente crescente em ms; duracoes sao diferencas entre instantes. */
bool metricas_agora(double *milissegundos);
/* Formato chave=valor esperado pela correcao automatizada, inclusive nas falhas. */
void metricas_imprimir(const METRICAS *metricas, bool sucesso);

#endif
