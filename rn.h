#ifndef _RN_H
#define _RN_H

typedef enum cor {
    PRETO, VERMELHO
} Cor;

typedef struct no {
    int valor;
    Cor cor;
    struct no* esq;
    struct no* dir;
    struct no* pai;
} NoRN;

typedef struct arvore {
    NoRN* raiz;
    NoRN* sentinela;
} ArvoreRN;

ArvoreRN* criarAvoreRN();
int inserirRN(ArvoreRN* T, const int valor);
int removerRN(ArvoreRN* T, const int valor);

#endif