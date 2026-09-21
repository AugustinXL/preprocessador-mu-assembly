#include <stdio.h>
#include <string.h>
#include <ctype.h> 
#include "lexico.h"

int scannerLer(Scanner *s){
    int c = fgetc(s->arq);

    if(c == '\n'){

        s->linha++;
        s->coluna = 0;

    }else if(c != '\r' && c != EOF) {

        s->coluna++;

    }

    return c;

}

void scannerDevolver(Scanner *s, int c){

    if (c == EOF ) {
        return;
    }

    ungetc(c, s-> arq);

    if (c == '\n'){
        
        s->linha--;

    } else if(c != '\r'){

        s->coluna--;

    }
}

void montarToken(Token *t, const char *nome, const char *lexema, Scanner *s){

    strncpy(t->nome, nome, MAX_NOME - 1);
    t->nome[MAX_NOME - 1] = '\0';

    strncpy(t->lexema, lexema, MAX_LEXEMA - 1);
    t->lexema[MAX_LEXEMA - 1] = '\0';
    
    t->linha =  s->tokenLinha;
    t->coluna = s->tokenColuna;
}

Token proximoToken(Scanner *s){

    Token t;
    int c;

    do {

        c = scannerLer(s);

    }while (c == ' ' || c == '\t' || c == '\n' || c == '\r');

    s->tokenLinha = s->linha;
    s->tokenColuna = s->coluna;

    if (c == EOF){

        montarToken(&t, "TK_EOF", "EOF", s);
        
        return t;

    }

    if(isalpha((unsigned char)c) || c == '_'){

        afdIdentificador(s, c, &t);

    } else if (c == '.') {

        afdDiretiva(s, c, &t);

    } else if (c == '"') {

        afdString(s, c, &t);

    } else if (c == '$'){

        afdRegistrador(s, c, &t);

    } else if (isdigit((unsigned char)c) || c == '-'){
               
        afdNumero(s, c, &t);

    } else {

        afdSimbolo(s, c, &t);

    }

    return t;

}

void AnaliseLexica(FILE *in, FILE *out){

    Scanner s; 
    Token t;

    s.arq = in;
    s.linha = 1;
    s.coluna = 0;
    s.tokenLinha = 1;
    s.tokenColuna = 1;

    tsInicializar();

    do {

        t = proximoToken(&s);
        fprintf(out, "<%s, %s> %d %d\n", t.nome, t.lexema, t.linha, t.coluna);

    } while (strcmp(t.nome, "TK_EOF") != 0);
}

void erroRegistrar(const char *tipo, const char *lexema, int linha, int coluna){

    (void)tipo; (void)lexema; (void)linha; (void)coluna;

}

void errosGravar(FILE *out){

    fprintf(out, "Nenhum erro lexico encontrado.\n");

}

void afdIdentificador(Scanner *s, int primeiro, Token *t){

    char lex[2];

    lex[0] = (char)primeiro;
    lex[1] = '\0';

    montarToken(t, "TODO_ID", lex, s);

}

void afdDiretiva(Scanner *s, int primeiro, Token *t){

    char lex[2];

    lex[0] = (char)primeiro;
    lex[1] = '\0';

    montarToken(t, "TODO_DIR", lex, s);

}

void afdString(Scanner *s, int primeiro, Token *t){

    char lex[2];

    lex[0] = (char)primeiro;
    lex[1] = '\0';


    montarToken(t, "TODO_STRING", lex, s);

}

void afdRegistrador(Scanner *s, int primeiro, Token *t){

    char lex[2];

    lex[0] = (char)primeiro;
    lex[1] = '\0';

    montarToken(t, "TODO_REG", lex, s);

}

void afdNumero(Scanner *s, int primeiro, Token *t){

    char lex[2];

    lex[0] = (char)primeiro;
    lex[1] = '\0';

    montarToken(t, "TODO_NUM", lex, s);

}

void afdSimbolo(Scanner *s, int primeiro, Token *t){

    char lex[2];

    lex[0] = (char)primeiro;
    lex[1] = '\0';

    montarToken(t, "TODO_SMB", lex, s);

}