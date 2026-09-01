#include <stdio.h>
#include "preprocessador.h"

#define SUCESSO 0
#define ERRO_ARGUMENTOS 1
#define ERRO_ENTRADA 2
#define ERRO_SAIDA 3

static void exibir_uso(const char *programa){

    fprintf(stderr, "Uso: %s <arquivo_entrada.asm> <arquivo_saida.pre> \n", programa);
    fprintf(stderr, "Exemplo: %s teste.asm teste.pre\n", programa);
}

int main(int argc, char *argv[]){

    FILE *entrada = NULL;
    FILE *saida = NULL;
    int linhas_gravadas = 0;

    if(argc != 3){
        
        fprintf(stderr, "ERRO: Quantidade de argumentos invalida");
        fprintf(stderr, "(esperado 2, recebido %d). \n", argc - 1);
        exibir_uso(argv[0]);

        return ERRO_ARGUMENTOS;

    }

    entrada = fopen(argv[1], "r");

    if (entrada == NULL){

        fprintf(stderr, "ERRO: Nao foi possivel abrir o arquivo de entrada.");
        fprintf(stderr, "\"%s\".\n", argv[1]);

        return ERRO_ENTRADA;
    }

    saida = fopen(argv[2], "w");

    if (saida == NULL){

        fprintf(stderr, "ERRO: Nao foi possivel abrir o arquivo de saida");
        fprintf(stderr, "\"%s\".\n", argv[2]);
        fclose(entrada);

        return ERRO_SAIDA;
    }


    linhas_gravadas = preprocessar(entrada, saida);

    fclose(entrada);
    fclose(saida);

    printf("Pre-processamento concluido.\n");
    printf("Entrada: %s\n", argv[1]);
    printf("Saida: %s (%d linha(s) gravada(s))\n", argv[2], linhas_gravadas);

    return SUCESSO;


}