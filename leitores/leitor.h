#ifndef LEITOR_H
#define LEITOR_H

#include "../dados/registro.h"

typedef struct leitor LEITOR;

/* Aceita nasa ou datatran e identifica as colunas pelo cabecalho.
 * Retorna NULL e descreve a falha em erro quando a abertura nao e concluida. */
LEITOR *leitor_abrir(const char *nome, const char *caminho,
                     char *erro, size_t capacidade);
/* A tabela pertence ao leitor e so permanece valida ate leitor_apagar. */
const TABELA *leitor_tabela(const LEITOR *leitor);
/*
 * true + registro nao nulo: o chamador assume o item; true + NULL: EOF normal.
 * false indica falha de formato, leitura ou alocacao, explicada em erro.
 */
bool leitor_proximo(LEITOR *leitor, REGISTRO **registro,
                     char *erro, size_t capacidade);
void leitor_apagar(LEITOR **leitor);

#endif
