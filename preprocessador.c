#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "preprocessador.h"

void remover_quebra_linha(char *linha){
    // Implementar função 
}

void remover_comentario(char *linha){
    // Implementar função 
}

void normalizar_espacos(char *linha){
    // Implementar função 
}

int linha_vazia(const char *linha){
    // Implementar função 
}

int preprocessar(FILE *entrada, FILE * saida){

    char linha[MAX_LINHA];
    int gravadas = 0;

    while (fgets(linha, MAX_LINHA, entrada) != NULL){
        
        remover_quebra_linha(linha);
        remover_comentario(linha);
        normalizar_espacos(linha);

        if(!linha_vazia(linha)){
            fprintf(saida, "%s\n", linha);
            gravadas++;
        }
    }
    
    return gravadas;
}