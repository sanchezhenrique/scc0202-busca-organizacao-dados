#include "carga.h"

#include <stdio.h>

bool carga_executar(LEITOR *leitor, COLECAO *base, size_t limite,
                    METRICAS *metricas, char *erro, size_t capacidade) {
  double inicio;
  double fim;
  bool sucesso = true;

  if (leitor == NULL || base == NULL || metricas == NULL ||
      !metricas_agora(&inicio)) {
    if (erro != NULL && capacidade > 0) {
      snprintf(erro, capacidade, "Carga invalida ou relogio indisponivel.");
    }
    return false;
  }
  /* Verifica o limite antes de ler para nao criar um registro alem do solicitado. */
  while (limite == 0 || metricas->registros_entrada < limite) {
    REGISTRO *registro = NULL;

    if (!leitor_proximo(leitor, &registro, erro, capacidade)) {
      sucesso = false;
      break;
    }
    if (registro == NULL) {
      break;
    }
    if (!colecao_inserir(base, registro)) {
      /* A base assume o item somente depois da insercao bem-sucedida. */
      registro_apagar(&registro);
      if (erro != NULL && capacidade > 0) {
        snprintf(erro, capacidade, "Sem memoria para inserir um registro na base.");
      }
      sucesso = false;
      break;
    }
    metricas->registros_entrada++;
  }
  if (!metricas_agora(&fim)) {
    if (erro != NULL && capacidade > 0) {
      snprintf(erro, capacidade, "Relogio indisponivel ao concluir a carga.");
    }
    return false;
  }
  /* A carga inclui leitura, criacao dos registros e insercao na estrutura da base. */
  metricas->carga_ms = fim - inicio;
  return sucesso;
}
