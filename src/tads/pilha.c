#include <stdio.h>
#include <stdlib.h>
#include "Pilha.h"

/* No da pilha: anterior aponta para o elemento imediatamente abaixo. */
typedef struct no NO;
struct no {
    ITEM* item;
    NO* anterior;
};

struct pilha {
    /* topo == NULL e tamanho == 0 representam a pilha vazia. */
    NO* topo;
    int tamanho;
};

PILHA* pilha_criar(void) {
    PILHA* pilha = (PILHA* ) malloc(sizeof (PILHA));
    if (pilha != NULL) {
        pilha->topo = NULL;
        pilha->tamanho = 0;
    }
    return (pilha);
}

void pilha_apagar(PILHA** pilha) {
    NO* paux;
    if (pilha == NULL || *pilha == NULL) return;
    /* Destruir a pilha libera cada referencia mantida por seus nos. */
    if (!pilha_vazia(*pilha)) {
        while ((*pilha)->topo != NULL) {
            paux = (*pilha)->topo;
            (*pilha)->topo = (*pilha)->topo->anterior;
            item_apagar(&paux->item);
            paux->anterior = NULL;
            free(paux);
        }
    }
    free(*pilha);
    *pilha = NULL;
}

bool pilha_cheia(PILHA *pilha){
    /* Estrutura encadeada nao tem capacidade fixa; malloc sinaliza falta de memoria. */
    (void)pilha; /* A pilha encadeada só fica cheia quando uma alocação falha. */
    return false;
}

bool pilha_vazia(PILHA *pilha) {
    /* Um ponteiro nulo nao representa uma pilha utilizavel e e tratado como vazia. */
    return pilha == NULL || pilha->tamanho == 0;
}

int pilha_tamanho(PILHA *pilha) {
    return ((pilha != NULL) ? pilha->tamanho : -1);
}

ITEM* pilha_topo(PILHA *pilha) {
    if ((pilha!=NULL)&& (!pilha_vazia(pilha)) ) {
        return (pilha->topo->item);
    }
    return (NULL);
}

bool pilha_empilhar(PILHA *pilha, ITEM* item) {
    if(pilha != NULL && item != NULL){
        NO *pnovo = (NO *) malloc(sizeof (NO));
        if (pnovo != NULL) {
            if (!item_reter(item)) {
                free(pnovo);
                return false;
            }
            /* O no passa a manter uma referencia propria ao ITEM. */
            pnovo->item = item;
            pnovo->anterior = pilha->topo;
            pilha->topo = pnovo;
            pilha->tamanho++;
            return (true);
        }
    }
    return (false);
}

ITEM* pilha_desempilhar(PILHA* pilha) {
    if ( (pilha != NULL) && (!pilha_vazia(pilha)) ) {
        NO* pno = pilha->topo;
        /* Remove o no e devolve a referencia ao ITEM sem libera-lo. */
        ITEM* item = pilha->topo->item;
        pilha->topo = pilha->topo->anterior;
        pno->anterior=NULL; 
        free(pno); 
        pilha->tamanho--;
        return (item);
    }
    return (NULL);
}
