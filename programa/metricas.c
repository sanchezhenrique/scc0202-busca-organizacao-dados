#include "metricas.h"

#include <stdio.h>
#include <time.h>

/* O relogio monotonicamente crescente nao acompanha ajustes do horario do sistema. */
bool metricas_agora(double *milissegundos) {
  struct timespec instante;

  if (milissegundos == NULL || clock_gettime(CLOCK_MONOTONIC, &instante) != 0) {
    return false;
  }
  *milissegundos = (double)instante.tv_sec * 1000.0 +
                   (double)instante.tv_nsec / 1000000.0;
  return true;
}

void metricas_imprimir(const METRICAS *metricas, bool sucesso) {
  if (metricas == NULL) {
    return;
  }
  printf("STATUS=%s\n", sucesso ? "OK" : "ERROR");
  printf("INPUT_RECORDS=%zu\n", metricas->registros_entrada);
  printf("MATCHED_RECORDS=%zu\n", metricas->registros_aceitos);
  printf("OUTPUT_RECORDS=%zu\n", metricas->registros_saida);
  printf("LOAD_MS=%.3f\n", metricas->carga_ms);
  printf("SEARCH_MS=%.3f\n", metricas->busca_ms);
  printf("RESULT_INSERT_MS=%.3f\n", metricas->insercao_ms);
  printf("WRITE_MS=%.3f\n", metricas->escrita_ms);
}
