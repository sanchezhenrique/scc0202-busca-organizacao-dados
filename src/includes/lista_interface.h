#ifndef LISTA_INTERFACE_H
#define LISTA_INTERFACE_H

#include <stdbool.h>
#include "item.h"

/* Retorna true para continuar o percurso e false para interrompe-lo. */
typedef bool (*LISTA_VISITANTE)(ITEM *item, void *contexto);

/* Operacoes comuns para selecionar dinamicamente qualquer TAD de lista. */
typedef struct lista_interface {
    void *(*criar)(bool ordenada);
    bool (*inserir)(void *lista, ITEM *item);
    /* Percurso e busca entregam referências emprestadas, sem retê-las. */
    bool (*percorrer)(const void *lista, LISTA_VISITANTE visitante,
                      void *contexto);
    ITEM *(*buscar)(const void *lista, int chave);
    void (*apagar)(void **lista);
} LISTA_INTERFACE;

extern const LISTA_INTERFACE lista_enc_interface;
extern const LISTA_INTERFACE lista_seq_interface;
extern const LISTA_INTERFACE lista_gen_interface;
extern const LISTA_INTERFACE lista_cabeca_interface;
extern const LISTA_INTERFACE lista_ord_interface;

#endif
