#include <stdio.h>
#include <stdlib.h>

// Regra 1: todo nó possui cor vermelha ou preta
// Regra 2: a raiz sempre será preta
// Regra 3: todo nó folha possui cor preta
// Regra 4: se um nó é vermelho então ambos os seus filhos são pretos
// Regra 5: para cada nó, todos os caminhos deste nó até os nós folha da
//          sua subárvore possuem o mesmo número de nós pretos

#include "rn.h"

void rotacionarEsquerda(ArvoreRN* T, NoRN* no) {
    NoRN* direita = no->dir;
    no->dir = direita->esq;

    if(direita->esq != T->sentinela) {
        direita->esq->pai = no;
    }

    direita->pai = no->pai;

    if(no->pai == T->sentinela) {
        T->raiz = direita;
    } else if(no == no->pai->esq) {
        no->pai->esq = direita;
    } else {
        no->pai->dir = direita;
    }

    direita->esq = no;
    no->pai = direita;
}

void rotacionarDireita(ArvoreRN* T, NoRN* no) {
    NoRN* esquerda = no->esq;
    no->esq = esquerda->dir;

    if(esquerda->dir != T->sentinela) {
        esquerda->dir->pai = no;
    }

    esquerda->pai = no->pai;

    if(no->pai == T->sentinela) {
        T->raiz = esquerda;
    } else if(no == no->pai->esq) {
        no->pai->esq = esquerda;
    } else {
        no->pai->dir = esquerda;
    }

    esquerda->dir = no;
    no->pai = esquerda;
}

int balancearInsercao(ArvoreRN* T, NoRN* no) {
    NoRN* tio;
    int count = 0;

    // vai balanceando todos os níveis
    while(no->pai->cor == VERMELHO) {

        count++;

        // CASO 1: O PAI É O FILHO ESQUERDO DO AVÔ
        if(no->pai == no->pai->pai->esq) {
            tio =  no->pai->pai->dir;
                        
            // CASO 1A: TIO VERMELHO (recoloração)
            if(tio->cor == VERMELHO) {
                tio->cor = PRETO;
                no->pai->cor = PRETO;
                no->pai->pai->cor = VERMELHO;
                no = no->pai->pai;
            } 
            // CASO 1B: TIO PRETO (rotação)
            else {
                // arruma o triangulo (se necessario)
                if(no == no->pai->dir) {
                    no = no->pai;
                    rotacionarEsquerda(T, no);
                    count++;
                }
                // arruma a linha
                no->pai->cor = PRETO;
                no->pai->pai->cor = VERMELHO;
                rotacionarDireita(T, no->pai->pai);
                count++;
            }
        }
        // CASO 2: O PAI É O FILHO DIREITO DO AVÔ
        else {
            tio =  no->pai->pai->esq;
            
            // CASO 2A: TIO VERMELHO (recoloração)
            if(tio->cor == VERMELHO) {
                tio->cor = PRETO;
                no->pai->cor = PRETO;
                no->pai->pai->cor = VERMELHO;
                no = no->pai->pai;
            } 
            // CASO 2B: TIO PRETO (rotação)
            else {
                // arruma o triangulo (se necessario)
                if(no == no->pai->esq) {
                    no = no->pai;
                    rotacionarDireita(T, no);
                    count++;
                }
                // arruma a linha
                no->pai->cor = PRETO;
                no->pai->pai->cor = VERMELHO;
                rotacionarEsquerda(T, no->pai->pai);
                count++;
            }
        }
    }
    // garante que a raiz é sempre preta
    T->raiz->cor = PRETO;

    return count;
}

NoRN* min(ArvoreRN* T, NoRN* no, int *count) {
    while(no->esq != T->sentinela) {
        (*count)++;
        no = no->esq;
    }
    return no;
}

void transplante(ArvoreRN* T, NoRN* u, NoRN* v) {
    if(u->pai == T->sentinela) {
        T->raiz = v;
    } else if(u == u->pai->esq) {
        u->pai->esq = v;
    } else {
        u->pai->dir = v;
    }
    v->pai = u->pai;
}

int balancearRemocao(ArvoreRN* T, NoRN* x) {
    NoRN* w;
    int count = 0;

    while(x != T->raiz && x->cor == PRETO) {

        count++;

        if(x == x->pai->esq) {
            w = x->pai->dir;

            if(w->cor == VERMELHO) {
                w->cor = PRETO;
                x->pai->cor = VERMELHO;
                rotacionarEsquerda(T, x->pai);
                w = x->pai->dir;
                count++;
            }

            if(w->esq->cor == PRETO && w->dir->cor == PRETO) {
                w->cor = VERMELHO;
                x = x->pai;
            }

            else {
                if(w->dir->cor == PRETO) {
                    w->esq->cor = PRETO;
                    w->cor = VERMELHO;
                    rotacionarDireita(T, w);
                    w = x->pai->dir;
                    count++;
                }

                w->cor = x->pai->cor;
                x->pai->cor = PRETO;
                w->dir->cor = PRETO;
                rotacionarEsquerda(T, x->pai);
                x = T->raiz;
                count++;
            }
        }

        else {
            w = x->pai->esq;

            if(w->cor == VERMELHO) {
                w->cor = PRETO;
                x->pai->cor = VERMELHO;
                rotacionarDireita(T, x->pai);
                w = x->pai->esq;
                count++;
            }

            if(w->dir->cor == PRETO && w->esq->cor == PRETO) {
                w->cor = VERMELHO;
                x = x->pai;
            }

            else {
                if(w->esq->cor == PRETO) {
                    w->dir->cor = PRETO;
                    w->cor = VERMELHO;
                    rotacionarEsquerda(T, w);
                    w = x->pai->esq;
                    count++;
                }

                w->cor = x->pai->cor;
                x->pai->cor = PRETO;
                w->esq->cor = PRETO;
                rotacionarDireita(T, x->pai);
                x = T->raiz;
                count++;
            }
        }
    }
    x->cor = PRETO;

    return count;
}

NoRN* criarNo(ArvoreRN* T, const int valor) {
    NoRN* novoNo = malloc(sizeof(NoRN));
    if(novoNo == NULL) {
        perror("Falha ao alocar nó");
        exit(1);
    }

    novoNo->cor = VERMELHO;
    novoNo->valor = valor;

    novoNo->dir = T->sentinela;
    novoNo->esq = T->sentinela;
    novoNo->pai = T->sentinela;

    return novoNo;
}

////////////////////////////////////////////////////////////////////

ArvoreRN* criarAvoreRN() {
    ArvoreRN* T = malloc(sizeof(ArvoreRN));
    if(T == NULL) {
        perror("Falha ao alocar árvore");
        exit(1);
    }

    NoRN* sentinela = malloc(sizeof(NoRN));
    if(sentinela == NULL) {
        perror("Falha ao alocar nó sentinela");
        free(T);
        exit(1);
    }

    sentinela->cor = PRETO;
    sentinela->valor = 0;

    sentinela->pai = sentinela;
    sentinela->esq = sentinela;
    sentinela->dir = sentinela;

    T->sentinela = sentinela;
    T->raiz = sentinela;

    return T;
}

int inserirRN(ArvoreRN* T, const int valor) {

    int count = 0;

    NoRN* pai = T->sentinela;
    NoRN* aux = T->raiz;

    // busca binaria
    while(aux != T->sentinela) {
        pai = aux;

        count++;

        if(valor < aux->valor) {
            aux = aux->esq;
        } else if(valor > aux->valor) {
            aux = aux->dir;
        } else {
            // duplicata
            return count;
        }
    }

    NoRN* novo = criarNo(T, valor);
    novo->pai = pai;

    if(pai == T->sentinela) {
        //arvore vazia
        T->raiz = novo;
    } else if(novo->valor < pai->valor) {
        pai->esq = novo;
    } else {
        pai->dir = novo;
    }

    count += balancearInsercao(T, novo);

    return count;
}

int removerRN(ArvoreRN* T, const int valor) {
    NoRN* z = T->raiz;
    int count = 0;

    // busca binária
    while(z != T->sentinela) {

        count++;

        if(valor < z->valor) {
            z = z->esq;
        } else if(valor > z->valor) {
            z = z->dir;
        } else {
            // achou
            break;
        }
    }

    if(z == T->sentinela) {
        return count; // no não encontrado
    }

    NoRN* y = z;
    int corOriginal = y->cor;
    NoRN* x;

    if(z->esq == T->sentinela) {
        x = z->dir;
        transplante(T, z, z->dir);
    } else if(z->dir == T->sentinela) {
        x = z->esq;
        transplante(T, z, z->esq);
    } else {
        y = min(T, z->dir, &count);
        corOriginal = y->cor;
        x = y->dir;

        if(y->pai == z) {
            x->pai = y;
        } else {
            transplante(T, y, y->dir);
            y->dir = z->dir;
            y->dir->pai = y;
        }

        transplante(T, z, y);
        y->esq = z->esq;
        y->esq->pai = y;
        y->cor = z->cor;
    }

    free(z);
    
    if(corOriginal == PRETO) {
        count += balancearRemocao(T, x);
    }

    return count;
}

void liberarNosRN(ArvoreRN* T, NoRN* no) {

    if (no == NULL || no == T->sentinela) {
        return;
    }

    liberarNosRN(T, no->esq);
    liberarNosRN(T, no->dir);

    free(no);
}

void destruirRN(ArvoreRN** T_ref) {
    ArvoreRN* T = *T_ref;
    
    if (T == NULL) return;

    liberarNosRN(T, T->raiz);

    if (T->sentinela != NULL) {
        free(T->sentinela);
    }

    free(T);
    
    *T_ref = NULL;
}
