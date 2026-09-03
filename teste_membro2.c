#include <stdio.h>
#include <string.h>
#include "preprocessador.h"

static int falhas = 0;

static void verificar_quebra(const char *nome, const char *entrada, const char *esperado)
{
    char linha[MAX_LINHA];
    snprintf(linha, sizeof(linha), "%s", entrada);
    remover_quebra_linha(linha);

    if (strcmp(linha, esperado) == 0) {
        printf("[OK] %s\n", nome);
    } else {
        printf("[FALHOU] %s | obtido=[%s] esperado=[%s]\n", nome, linha, esperado);
        falhas++;
    }
}

static void verificar_vazia(const char *nome, const char *entrada, int esperado)
{
    int obtido = linha_vazia(entrada);

    if (obtido == esperado) {
        printf("[OK] %s\n", nome);
    } else {
        printf("[FALHOU] %s | obtido=%d esperado=%d\n", nome, obtido, esperado);
        falhas++;
    }
}

int main(void)
{
    printf("=== TESTES DO MEMBRO 2 ===\n\n");

    verificar_quebra("LF", "add $t0, $t1, $t2\n", "add $t0, $t1, $t2");
    verificar_quebra("CRLF", "add $t0, $t1, $t2\r\n", "add $t0, $t1, $t2");
    verificar_quebra("Sem quebra final", "add $t0, $t1, $t2", "add $t0, $t1, $t2");
    verificar_quebra("So CR", "add $t0, $t1, $t2\r", "add $t0, $t1, $t2");

    verificar_vazia("String vazia", "", 1);
    verificar_vazia("Somente espacos", "   ", 1);
    verificar_vazia("Somente tabulacoes", "\t\t", 1);
    verificar_vazia("Espacos e tabulacoes", " \t  \t ", 1);
    verificar_vazia("Linha com instrucao", "add $t0, $t1, $t2", 0);

    printf("\n");
    if (falhas == 0) {
        printf("TODOS OS TESTES DO MEMBRO 2 PASSARAM.\n");
        return 0;
    }

    printf("%d teste(s) falharam.\n", falhas);
    return 1;
}
