#include "busca.h"

#include <stdio.h>

bool busca_executar(COLECAO *base, COLECAO *resultado,
                    const CONSULTA *consulta, METRICAS *metricas,
                    char *erro, size_t capacidade) {
  REGISTRO *registro;
  double inicio;
  double fim;
  double antes;
  double depois;
  bool sucesso = base != NULL && resultado != NULL && consulta != NULL && metricas != NULL;

  sucesso = sucesso && metricas_agora(&inicio) && colecao_reiniciar(base, 0);
  while (sucesso && (registro = colecao_proximo(base)) != NULL) {
    if (consulta_aceita(consulta, registro)) {
      metricas->registros_aceitos++;
      sucesso = metricas_agora(&antes);
      if (sucesso) {
        sucesso = colecao_inserir(resultado, registro);
        if (metricas_agora(&depois)) {
          metricas->insercao_ms += depois - antes;
        } else {
          sucesso = false;
        }
      }
    }
  }
  if (metricas != NULL && metricas_agora(&fim) && sucesso) {
    /* Desconta a insercao para que ela nao seja contada duas vezes. */
    metricas->busca_ms = fim - inicio - metricas->insercao_ms;
  } else {
    sucesso = false;
  }
  if (!sucesso && erro != NULL && capacidade > 0) {
    snprintf(erro, capacidade, "Falha na busca, insercao de resultado ou medicao.");
  }
  return sucesso;
}
