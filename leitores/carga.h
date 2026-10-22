#ifndef CARGA_H
#define CARGA_H

#include "leitor.h"
#include "../programa/metricas.h"
#include "../tads/colecao.h"

/* Carrega diretamente na base; limite zero le ate EOF, sem contar o cabecalho. */
bool carga_executar(LEITOR *leitor, COLECAO *base, size_t limite,
                    METRICAS *metricas);

#endif
