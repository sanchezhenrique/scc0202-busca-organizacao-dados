#ifndef LEITOR_H
#define LEITOR_H

#include "../dados/item.h"
#include "../dados/tabela.h"

typedef struct leitor LEITOR;

/* Aceita nasa ou datatran e identifica as colunas pelo cabecalho.
 * Retorna NULL e imprime ERRO com printf se a abertura falhar. */
LEITOR *leitor_abrir(const char *nome, const char *caminho);
/* A tabela pertence ao leitor e so permanece valida ate leitor_apagar. */
const TABELA *leitor_tabela(const LEITOR *leitor);
/*
 * true + item nao nulo: o chamador assume o item; true + NULL: EOF normal.
 * false indica falha de formato, leitura ou alocacao, explicada com printf.
 */
bool leitor_proximo(LEITOR *leitor, ITEM **item);
void leitor_apagar(LEITOR **leitor);

#endif
