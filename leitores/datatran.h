#ifndef DATATRAN_H
#define DATATRAN_H

#include "../dados/registro.h"

/* Confere os campos que identificam o arquivo da Parte 2. */
bool datatran_validar(const TABELA *tabela);
/* Adapta os valores de consulta; a linha original do registro nao e modificada. */
bool datatran_tratar(char **campos, size_t quantidade, const TABELA *tabela);

#endif
