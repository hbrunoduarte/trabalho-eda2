#ifndef _AVL_H
#define _AVL_H

typedef int(comparator)(const void*, const void*);

typedef struct avlnode {
    struct avlnode *left;
    struct avlnode *right;
    void *key;
    int height;
} AVLnode;

typedef struct {
    AVLnode *root;
    comparator *comp;
} AVL;

int isAVLempty(AVL *t);
AVL* createAVL(comparator *f);
int insertAVL(AVL *t, void *key);
int removeAVL(AVL *t, void *key);
void destroyAVL(AVL **t);

#endif