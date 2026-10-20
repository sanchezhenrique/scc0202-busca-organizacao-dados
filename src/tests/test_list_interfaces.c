#include <assert.h>

#include "lista_enc.h"
#include "lista_gen.h"
#include "lista_interface.h"
#include "lista_seq.h"

static bool contar_item(ITEM *item, void *contexto){
    size_t *quantidade = contexto;
    assert(item != NULL);
    (*quantidade)++;
    return true;
}

int main(void){
    const LISTA_INTERFACE *implementacoes[] = {
        &lista_enc_interface,
        &lista_seq_interface,
        &lista_gen_interface
    };
    void *listas[sizeof(implementacoes) / sizeof(implementacoes[0])];
    ITEM *item = item_criar();
    assert(item != NULL);
    item_set_chave(item, 91);

    for(size_t i = 0; i < sizeof(listas) / sizeof(listas[0]); i++){
        listas[i] = implementacoes[i]->criar(false);
        assert(listas[i] != NULL);
        assert(implementacoes[i]->inserir(listas[i], item));
        assert(implementacoes[i]->buscar(listas[i], 91) == item);
        size_t quantidade = 0;
        assert(implementacoes[i]->percorrer(listas[i], contar_item,
                                            &quantidade));
        assert(quantidade == 1);
    }

    /* Uma insercao em cada TAD mantém o mesmo registro sem copia. */
    item_apagar(&item);
    assert(item == NULL);
    for(size_t i = 0; i < sizeof(listas) / sizeof(listas[0]); i++)
        assert(implementacoes[i]->buscar(listas[i], 91) != NULL);

    for(size_t i = 0; i < sizeof(listas) / sizeof(listas[0]); i++){
        implementacoes[i]->apagar(&listas[i]);
        assert(listas[i] == NULL);
    }

    return 0;
}
