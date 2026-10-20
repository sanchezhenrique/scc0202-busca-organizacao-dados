#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
BUILD_DIR=$(mktemp -d)
trap 'rm -rf "$BUILD_DIR"' EXIT HUP INT TERM

CC=${CC:-cc}
CFLAGS=${CFLAGS:-"-std=c99 -Wall -Wextra -pedantic"}
CFLAGS="$CFLAGS -I$ROOT/src/includes"
ITEM="$ROOT/src/tads/item.c"

$CC $CFLAGS "$ROOT/src/tests/test_item_ownership.c" \
  "$ROOT/src/tads/pilha.c" "$ROOT/src/tads/fila.c" \
  "$ROOT/src/tads/lista_enc.c" "$ITEM" -lm -o "$BUILD_DIR/test_item_ownership"
$CC $CFLAGS "$ROOT/src/tests/test_lista_seq_ownership.c" \
  "$ROOT/src/tads/lista_seq.c" "$ITEM" -lm -o "$BUILD_DIR/test_lista_seq_ownership"
$CC $CFLAGS "$ROOT/src/tests/test_lista_gen_ownership.c" \
  "$ROOT/src/tads/lista_gen.c" "$ITEM" -lm -o "$BUILD_DIR/test_lista_gen_ownership"
$CC $CFLAGS "$ROOT/src/tests/test_list_interfaces.c" \
  "$ROOT/src/tads/lista_enc.c" "$ROOT/src/tads/lista_seq.c" \
  "$ROOT/src/tads/lista_gen.c" "$ROOT/src/tads/lista_interface.c" \
  "$ITEM" -lm -o "$BUILD_DIR/test_list_interfaces"

"$BUILD_DIR/test_item_ownership"
"$BUILD_DIR/test_lista_seq_ownership"
"$BUILD_DIR/test_lista_gen_ownership"
"$BUILD_DIR/test_list_interfaces"
printf '%s\n' "Testes de propriedade dos ITEMs passaram."
