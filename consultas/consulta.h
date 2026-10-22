#ifndef CONSULTA_H
#define CONSULTA_H

#include "../tads/colecao.h"
#include "../dados/tabela.h"

typedef struct consulta CONSULTA;

CONSULTA *consulta_ler(const char *caminho, const TABELA *tabela,
                       char *erro, size_t capacidade);
void consulta_apagar(CONSULTA **consulta);
bool consulta_aceita(const CONSULTA *consulta, const ITEM *item);
double consulta_similaridade(const CONSULTA *consulta, const ITEM *item);
/* As estruturas recebem a consulta, sem ponteiros para funcoes de comparacao. */
bool consulta_estrutura(const CONSULTA *consulta, TIPO_COLECAO tipo);
int consulta_comparar(const CONSULTA *consulta, const ITEM *a, const ITEM *b);
const char *consulta_grupo(const CONSULTA *consulta, const ITEM *item, unsigned int nivel);
int consulta_cruzar(const CONSULTA *consulta, const ITEM *a,
                    const ITEM *b, unsigned int eixo);

#endif
