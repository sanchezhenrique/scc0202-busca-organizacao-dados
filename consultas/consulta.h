#ifndef CONSULTA_H
#define CONSULTA_H

#include "../tads/colecao.h"

typedef struct consulta CONSULTA;

CONSULTA *consulta_ler(const char *caminho, const TABELA *tabela,
                       char *erro, size_t capacidade);
void consulta_apagar(CONSULTA **consulta);
bool consulta_aceita(const CONSULTA *consulta, const REGISTRO *registro);
double consulta_similaridade(const CONSULTA *consulta, const REGISTRO *registro);
/* As estruturas recebem a consulta, sem ponteiros para funcoes de comparacao. */
bool consulta_estrutura(const CONSULTA *consulta, TIPO_COLECAO tipo);
int consulta_comparar(const CONSULTA *consulta, const REGISTRO *a, const REGISTRO *b);
const char *consulta_grupo(const CONSULTA *consulta, const REGISTRO *registro, unsigned int nivel);
int consulta_cruzar(const CONSULTA *consulta, const REGISTRO *a,
                    const REGISTRO *b, unsigned int eixo);

#endif
