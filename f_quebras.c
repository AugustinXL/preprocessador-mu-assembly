#include <string.h>
#include "preprocessador.h"

void remover_quebra_linha(char *linha)
{
    if (linha == NULL) {
        return;
    }

    size_t tamanho = strlen(linha);

    while (tamanho > 0 &&
           (linha[tamanho - 1] == '\n' ||
            linha[tamanho - 1] == '\r')) {

        linha[tamanho - 1] = '\0';
        tamanho--;
    }
}

int linha_vazia(const char *linha)
{
    if (linha == NULL) {
        return 1;
    }

    while (*linha != '\0') {

        if (*linha != ' ' &&
            *linha != '\t' &&
            *linha != '\r' &&
            *linha != '\n') {

            return 0;
        }

        linha++;
    }

    return 1;
}