#ifndef BUSCA_H
#define BUSCA_H

#include "../consultas/consulta.h"
#include "../programa/metricas.h"

/* A base conserva seus itens; resultado recebe apenas referencias aos aceitos. */
bool busca_executar(COLECAO *base, COLECAO *resultado,
                    const CONSULTA *consulta, METRICAS *metricas,
                    char *erro, size_t capacidade);

#endif
