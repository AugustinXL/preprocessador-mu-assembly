#include "preprocessador.h"

void remover_comentario(char *linha) {
    int dentro_string = 0;
    
    for (int i = 0; linha[i] != '\0'; i++) {
        if (linha[i] == '"') {
            dentro_string = !dentro_string;
        } else if (linha[i] == '#' && !dentro_string) {
            linha[i] = '\0';
            return;
        }
    }
}
