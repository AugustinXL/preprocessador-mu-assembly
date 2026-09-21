/* ==========================================================================
 *  tabela_simbolos.c - Tabela de Simbolos (Membro 2)
 *
 *  Estrutura
 *  ---------
 *  Um unico vetor "tabela" dividido em duas regioes:
 *
 *    [0 .. totalReservadas)      palavras reservadas (instrucoes, diretivas
 *                                e registradores), carregadas por
 *                                tsInicializar(). Sempre em minusculas.
 *    [totalReservadas .. total)  simbolos inseridos durante a analise
 *                                (identificadores/rotulos). Caixa preservada.
 *
 *  Regras
 *  ------
 *  - Palavras reservadas sao case-insensitive: "ADD", "Add" e "add" sao a
 *    mesma instrucao (a tabela guarda "add").
 *  - Rotulos preservam a caixa: "Main" e "main" sao IDs diferentes.
 *  - Nao ha duplicatas: inserir algo que ja existe (ou que colide com uma
 *    palavra reservada) e ignorado.
 *
 *  Uso pelo analisador lexico (Membro 3)
 *  -------------------------------------
 *    char cat[MAX_CATEGORIA], nome[MAX_NOME];
 *
 *    if (tsBuscar(lexema, cat, nome)) {
 *        // achou: 'nome' ja vem como TK_INSTRUCAO, TK_DIRETIVA,
 *        // TK_REGISTRADOR ou TK_ID
 *    } else {
 *        // nao e palavra reservada nem ID conhecido
 *        //   -> ID novo:  tsInserir(lexema, CAT_ID, linha, coluna);
 *        //   -> diretiva/registrador invalido: erroRegistrar(...)
 *    }
 * ========================================================================== */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexico.h"

#define TAM(v) (sizeof(v) / sizeof((v)[0]))

/* --------------------------------------------------------------------------
 *  Palavras reservadas
 *  ATENCAO: conferir estas listas com a especificacao do trabalho.
 * -------------------------------------------------------------------------- */

static const char *const INSTRUCOES[] = {
    /* aritmetica */
    "add", "addu", "addi", "addiu", "sub", "subu", "mult", "div",
    /* logicas */
    "and", "andi", "or", "ori", "xor", "xori", "nor",
    /* deslocamento */
    "sll", "srl", "sra",
    /* comparacao */
    "slt", "slti", "sltu", "sltiu",
    /* memoria */
    "lw", "sw", "lb", "sb", "lh", "sh",
    /* carga de constantes / enderecos / copia */
    "lui", "li", "la", "move", "mfhi", "mflo",
    /* desvios e saltos */
    "beq", "bne", "blt", "bgt", "ble", "bge", "j", "jal", "jr",
    /* sistema */
    "syscall", "nop"
};

static const char *const DIRETIVAS[] = {
    ".data", ".text", ".globl", ".align", ".space",
    ".byte", ".half", ".word", ".ascii", ".asciiz"
};

static const char *const REGISTRADORES[] = {
    "$zero", "$at", "$v0", "$v1",
    "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$s8",
    "$k0", "$k1", "$gp", "$sp", "$fp", "$ra",
    "$hi", "$lo"
};

/* --------------------------------------------------------------------------
 *  Estado da tabela
 * -------------------------------------------------------------------------- */

static Simbolo tabela[MAX_SIMBOLOS];
static int total = 0;
static int totalReservadas = 0;

/* --------------------------------------------------------------------------
 *  Auxiliares internas
 * -------------------------------------------------------------------------- */

/* Copia origem -> destino em minusculas (no maximo tam-1 caracteres). */
static void minusculas(const char *origem, char *destino, size_t tam){

    size_t i;

    for (i = 0; origem[i] != '\0' && i < tam - 1; i++) {

        destino[i] = (char)tolower((unsigned char)origem[i]);

    }

    destino[i] = '\0';

}

static int categoriaReservada(const char *categoria){

    return strcmp(categoria, CAT_INSTRUCAO)   == 0 ||
           strcmp(categoria, CAT_DIRETIVA)    == 0 ||
           strcmp(categoria, CAT_REGISTRADOR) == 0;

}

/* Converte categoria em nome de token: INSTRUCAO -> TK_INSTRUCAO, etc. */
static void nomeDoToken(const char *categoria, char *nomeToken){

    snprintf(nomeToken, MAX_NOME, "TK_%s", categoria);

}

/* Procura nas palavras reservadas (o argumento ja deve estar em minusculas). */
static int indiceReservada(const char *minuscula){

    int i;

    for (i = 0; i < totalReservadas; i++) {

        if (strcmp(tabela[i].lexema, minuscula) == 0) {

            return i;

        }

    }

    return -1;

}

/* Procura correspondencia exata (case-sensitive) nos simbolos inseridos. */
static int indiceExato(const char *lexema){

    int i;

    for (i = totalReservadas; i < total; i++) {

        if (strcmp(tabela[i].lexema, lexema) == 0) {

            return i;

        }

    }

    return -1;

}

/* Acrescenta uma entrada ao fim da tabela. Retorna 1 se coube, 0 se cheia. */
static int adicionar(const char *lexema, const char *categoria, int linha, int coluna){

    Simbolo *s;

    if (total >= MAX_SIMBOLOS) {

        fprintf(stderr, "AVISO: tabela de simbolos cheia (%d). Ignorado: %s\n",
                MAX_SIMBOLOS, lexema);

        return 0;

    }

    s = &tabela[total++];

    strncpy(s->lexema, lexema, MAX_LEXEMA - 1);
    s->lexema[MAX_LEXEMA - 1] = '\0';

    strncpy(s->categoria, categoria, MAX_CATEGORIA - 1);
    s->categoria[MAX_CATEGORIA - 1] = '\0';

    s->primeiraLinha = linha;
    s->primeiraColuna = coluna;

    return 1;

}

static void carregar(const char *const *lista, size_t n, const char *categoria){

    size_t i;

    for (i = 0; i < n; i++) {

        adicionar(lista[i], categoria, 0, 0);

    }

}

/* Escreve um campo CSV, com aspas se tiver virgula, aspas ou quebra de linha. */
static void gravarCampoCSV(FILE *out, const char *campo){

    const char *p;
    int precisaAspas = (strpbrk(campo, ",\"\r\n") != NULL);

    if (!precisaAspas) {

        fputs(campo, out);
        return;

    }

    fputc('"', out);

    for (p = campo; *p != '\0'; p++) {

        if (*p == '"') {

            fputc('"', out);

        }

        fputc(*p, out);

    }

    fputc('"', out);

}

/* --------------------------------------------------------------------------
 *  Interface publica (declarada em lexico.h)
 * -------------------------------------------------------------------------- */

/* Zera a tabela e carrega as palavras reservadas. Pode ser chamada varias vezes. */
void tsInicializar(void){

    total = 0;

    carregar(INSTRUCOES,    TAM(INSTRUCOES),    CAT_INSTRUCAO);
    carregar(DIRETIVAS,     TAM(DIRETIVAS),     CAT_DIRETIVA);
    carregar(REGISTRADORES, TAM(REGISTRADORES), CAT_REGISTRADOR);

    totalReservadas = total;

}

/*
 * Consulta um lexema.
 *   - Palavras reservadas: comparacao sem distinguir maiusculas/minusculas.
 *   - Demais simbolos (rotulos): comparacao exata.
 *
 * Retorna 1 se encontrou, 0 caso contrario.
 * Se encontrou, preenche (quando nao forem NULL):
 *   categoria : "INSTRUCAO", "DIRETIVA", "REGISTRADOR" ou "ID"
 *               (buffer com pelo menos MAX_CATEGORIA bytes)
 *   nomeToken : "TK_INSTRUCAO", "TK_DIRETIVA", "TK_REGISTRADOR" ou "TK_ID"
 *               (buffer com pelo menos MAX_NOME bytes)
 */
int tsBuscar(const char *lexema, char *categoria, char *nomeToken){

    char minuscula[MAX_LEXEMA];
    int i;

    if (lexema == NULL) {

        return 0;

    }

    minusculas(lexema, minuscula, MAX_LEXEMA);

    i = indiceReservada(minuscula);

    if (i < 0) {

        i = indiceExato(lexema);

    }

    if (i < 0) {

        return 0;

    }

    if (categoria != NULL) {

        snprintf(categoria, MAX_CATEGORIA, "%s", tabela[i].categoria);

    }

    if (nomeToken != NULL) {

        nomeDoToken(tabela[i].categoria, nomeToken);

    }

    return 1;

}

/*
 * Insere um simbolo (normalmente CAT_ID) com a posicao da primeira ocorrencia.
 * Duplicatas sao bloqueadas: se o lexema ja existe, ou se colide com uma
 * palavra reservada (ex.: um rotulo chamado "ADD"), nada e feito.
 */
void tsInserir(const char *lexema, const char *categoria, int linha, int coluna){

    char minuscula[MAX_LEXEMA];
    char normalizado[MAX_LEXEMA];

    if (lexema == NULL || categoria == NULL || lexema[0] == '\0') {

        return;

    }

    minusculas(lexema, minuscula, MAX_LEXEMA);

    if (indiceReservada(minuscula) >= 0) {

        return;

    }

    /* Categorias reservadas sao normalizadas; rotulos preservam a caixa. */
    if (categoriaReservada(categoria)) {

        snprintf(normalizado, MAX_LEXEMA, "%s", minuscula);

    } else {

        snprintf(normalizado, MAX_LEXEMA, "%s", lexema);

    }

    if (indiceExato(normalizado) >= 0) {

        return;

    }

    adicionar(normalizado, categoria, linha, coluna);

}

/* Grava a tabela inteira em CSV: LEXEMA,CATEGORIA,LINHA,COLUNA. */
void tsGravar(FILE *out){

    int i;

    fprintf(out, "LEXEMA,CATEGORIA,LINHA,COLUNA\n");

    for (i = 0; i < total; i++) {

        gravarCampoCSV(out, tabela[i].lexema);
        fputc(',', out);
        gravarCampoCSV(out, tabela[i].categoria);
        fprintf(out, ",%d,%d\n", tabela[i].primeiraLinha, tabela[i].primeiraColuna);

    }

}
