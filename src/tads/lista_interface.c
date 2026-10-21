#include "../includes/lista_interface.h"
#include "../includes/lista_enc.h"
#include "../includes/lista_seq.h"
#include "../includes/lista_gen.h"
#include "../includes/lista_cabeca.h"
#include "../includes/lista_ord.h"

static void *lista_enc_criar_generico(bool ordenada){
    return lista_enc_criar(ordenada);
}

static bool lista_enc_inserir_generico(void *lista, ITEM *item){
    return lista_enc_inserir(lista, item);
}

static bool lista_enc_percorrer_generico(const void *lista,
                                         LISTA_VISITANTE visitante,
                                         void *contexto){
    return lista_enc_percorrer(lista, visitante, contexto);
}

static ITEM *lista_enc_buscar_generico(const void *lista, int chave){
    return lista_enc_buscar(lista, chave);
}

static void lista_enc_apagar_generico(void **lista){
    if(lista == NULL) return;
    LISTA_ENC *especifica = *lista;
    lista_enc_apagar(&especifica);
    *lista = NULL;
}

const LISTA_INTERFACE lista_enc_interface = {
    lista_enc_criar_generico,
    lista_enc_inserir_generico,
    lista_enc_percorrer_generico,
    lista_enc_buscar_generico,
    lista_enc_apagar_generico
};

static void *lista_seq_criar_generico(bool ordenada){
    return lista_seq_criar(ordenada);
}

static bool lista_seq_inserir_generico(void *lista, ITEM *item){
    return lista_seq_inserir(lista, item);
}

static bool lista_seq_percorrer_generico(const void *lista,
                                         LISTA_VISITANTE visitante,
                                         void *contexto){
    return lista_seq_percorrer(lista, visitante, contexto);
}

static ITEM *lista_seq_buscar_generico(const void *lista, int chave){
    return lista_seq_buscar(lista, chave);
}

static void lista_seq_apagar_generico(void **lista){
    if(lista == NULL) return;
    LISTA_SEQ *especifica = *lista;
    lista_seq_apagar(&especifica);
    *lista = NULL;
}

const LISTA_INTERFACE lista_seq_interface = {
    lista_seq_criar_generico,
    lista_seq_inserir_generico,
    lista_seq_percorrer_generico,
    lista_seq_buscar_generico,
    lista_seq_apagar_generico
};

static void *lista_gen_criar_generico(bool ordenada){
    (void)ordenada;
    return lista_gen_criar();
}

static bool lista_gen_inserir_generico(void *lista, ITEM *item){
    return lista_gen_inserir_atomo(lista, item);
}

static bool lista_gen_percorrer_generico(const void *lista,
                                         LISTA_VISITANTE visitante,
                                         void *contexto){
    return lista_gen_percorrer(lista, visitante, contexto);
}

static ITEM *lista_gen_buscar_generico(const void *lista, int chave){
    return lista_gen_buscar(lista, chave);
}

static void lista_gen_apagar_generico(void **lista){
    if(lista == NULL) return;
    LISTA_GEN *especifica = *lista;
    lista_gen_apagar(&especifica);
    *lista = NULL;
}

const LISTA_INTERFACE lista_gen_interface = {
    lista_gen_criar_generico,
    lista_gen_inserir_generico,
    lista_gen_percorrer_generico,
    lista_gen_buscar_generico,
    lista_gen_apagar_generico
};

static void *lista_cabeca_criar_generico(bool ordenada){
    (void)ordenada;
    return lista_cabeca_criar();
}

static bool lista_cabeca_inserir_generico(void *lista, ITEM *item){
    return lista_cabeca_inserir(lista, item);
}

static bool lista_cabeca_percorrer_generico(const void *lista,
                                           LISTA_VISITANTE visitante,
                                           void *contexto){
    return lista_cabeca_percorrer(lista, visitante, contexto);
}

static ITEM *lista_cabeca_buscar_generico(const void *lista, int chave){
    return lista_cabeca_buscar(lista, chave);
}

static void lista_cabeca_apagar_generico(void **lista){
    if(lista == NULL) return;
    LISTA_CABECA *especifica = *lista;
    lista_cabeca_apagar(&especifica);
    *lista = NULL;
}

const LISTA_INTERFACE lista_cabeca_interface = {
    lista_cabeca_criar_generico,
    lista_cabeca_inserir_generico,
    lista_cabeca_percorrer_generico,
    lista_cabeca_buscar_generico,
    lista_cabeca_apagar_generico
};

static void *lista_ord_criar_generico(bool ordenada){
    (void)ordenada;
    return lista_ord_criar("similarity", true);
}

static bool lista_ord_inserir_generico(void *lista, ITEM *item){
    return lista_ord_inserir(lista, item);
}

static bool lista_ord_percorrer_generico(const void *lista,
                                         LISTA_VISITANTE visitante,
                                         void *contexto){
    return lista_ord_percorrer(lista, visitante, contexto);
}

static ITEM *lista_ord_buscar_generico(const void *lista, int chave){
    return lista_ord_buscar(lista, chave);
}

static void lista_ord_apagar_generico(void **lista){
    if(lista == NULL) return;
    LISTA_ORD *especifica = *lista;
    lista_ord_apagar(&especifica);
    *lista = NULL;
}

const LISTA_INTERFACE lista_ord_interface = {
    lista_ord_criar_generico,
    lista_ord_inserir_generico,
    lista_ord_percorrer_generico,
    lista_ord_buscar_generico,
    lista_ord_apagar_generico
};
