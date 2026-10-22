#ifndef EXPORTACAO_H
#define EXPORTACAO_H

#include "../dados/tabela.h"
#include "../tads/colecao.h"
#include "../programa/metricas.h"

/* Preserva as colunas e visita o resultado sem remove-lo; top_k zero exporta tudo. */
bool exportacao_gravar(const char *caminho, const TABELA *tabela,
                       COLECAO *resultado, size_t top_k,
                       METRICAS *metricas, char *erro, size_t capacidade);

#endif
