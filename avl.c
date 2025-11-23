#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "avl.h"

#define MIN_LIMIT -1
#define MAX_LIMIT 1

typedef enum {
    FALSE,
    TRUE
} bool;

typedef void(procedure)(void*);

int isAVLempty(AVL *t) {
    if (!t) return 0;
    return t->root == NULL;
}

int isAVLleaf(AVLnode *n) {
    if (!n) return 0;
    return !n->left && !n->right;
}

int isHalfLeaf(AVLnode *n) {
    if (!n) return 0;
    return (!n->left && n->right) || (n->left && !n->right);
}

AVLnode* createAVLnode(void *key) {

    AVLnode *new = malloc(sizeof(AVLnode));
    if (!new) return NULL;

    new->left = NULL;
    new->right = NULL;
    new->key = key;
    new->height = 1;

    return new;
}

AVL* createAVL(comparator *f) {

    AVL *tree = malloc(sizeof(AVL));
    if (!tree) return NULL;

    tree->comp = f;
    tree->root = NULL;

    return tree;
}

int balanceFactor(AVLnode *n) {
    if (!n) return 0;
    if (isAVLleaf(n)) return 0;
    if (!n->left) return -n->right->height;
    if (!n->right) return n->left->height;
    return n->left->height - n->right->height;
}

void updateParent(AVLnode *parent, AVLnode *n, AVLnode *substitute) {

    if (!parent) return;

    if (n == parent->left)
        parent->left = substitute;
    else if (n == parent->right)
        parent->right = substitute;

}

void updateHeight(AVLnode *n) {

    if (!n) return;

    if (isAVLleaf(n)) {
        n->height = 1;
    } else if (!n->left) {
        n->height = n->right->height + 1;
    } else if (!n->right) {
        n->height = n->left->height + 1;
    } else {
        n->height = n->left->height > n->right->height ? n->left->height + 1 : n->right->height + 1;
    }
}

AVLnode* slr(AVLnode *n) {

    if (!n) return NULL;

    AVLnode *right = n->right;
    if (!right) return NULL;

    n->right = right->left;
    right->left = n;

    updateHeight(n);
    updateHeight(right);

    return right;
}

AVLnode* srr(AVLnode *n) {

    if (!n) return NULL;

    AVLnode *left = n->left;
    if (!left) return NULL;

    n->left = left->right;
    left->right = n;

    updateHeight(n);
    updateHeight(left);

    return left;
}

AVLnode* dlr(AVLnode *n) {
    if (!n) return NULL;
    n->right = srr(n->right);
    return slr(n);
}

AVLnode* drr(AVLnode *n) {
    if (!n) return NULL;
    n->left = slr(n->left);
    return srr(n);
}

bool checkRebalance(AVL *t, AVLnode *parent, AVLnode *n) {
    if (!n) return FALSE;

    int fb = balanceFactor(n);

    if (fb >= MIN_LIMIT && fb <= MAX_LIMIT) return FALSE;

    AVLnode *substitute = NULL;

    if (fb > MAX_LIMIT) {
        if (balanceFactor(n->left) >= 0)
            substitute = srr(n);
        else
            substitute = drr(n);
    
    } else {
        if (balanceFactor(n->right) <= 0)
            substitute = slr(n);
        else
            substitute = dlr(n); 
    }

    if (parent) {
        if (parent->left == n)
            parent->left = substitute;
        else
            parent->right = substitute;
    } else {
        t->root = substitute;
    }
    
    return TRUE;
}

int __insertAVL__(AVL *t, AVLnode *parent, AVLnode *actual, AVLnode *new, bool *update) {

    if (isAVLempty(t)) {
        t->root = new;
        return 0;
    }

    int count = 0;

    if (!actual) {

        if (t->comp(new->key, parent->key) < 0)
            parent->left = new;
        else
            parent->right = new;
        
        *update = TRUE;
        return count;

    } else {

        int compResult = t->comp(new->key, actual->key);

        if (compResult < 0) {
            count += 1 + __insertAVL__(t, actual, actual->left, new, update);

        } else if (compResult > 0) {
            count += 1 + __insertAVL__(t, actual, actual->right, new, update);

        } else {
            *update = FALSE;
            free(new);
            return count;
        }
    }

    count++;

    if (*update) {
        updateHeight(actual);
        if (checkRebalance(t, parent, actual)) {
            *update = FALSE;
        }
    }

    return count;
}

int insertAVL(AVL *t, void *key) {

    if (!t || !key) return 0;
    
    AVLnode *new = createAVLnode(key);
    if (!new) return 0;

    bool update = TRUE;
    return __insertAVL__(t, NULL, t->root, new, &update);
}

AVLnode* removeMinimum(AVL *t, AVLnode *parent, AVLnode *actual, bool *update, int *i) {

    if (!actual) return NULL;

    (*i)++;

    AVLnode *minimum = NULL;

    if (actual->left) {

        minimum = removeMinimum(t, actual, actual->left, update, i);

        updateHeight(actual);
        checkRebalance(t, parent, actual);

    } else {
        *update = TRUE;
        updateParent(parent, actual, actual->right);
        return actual;
    }

    return minimum;
}

int __removeAVL__(AVL *t, AVLnode *parent, AVLnode *actual, void *target, bool *update) {

    if (!actual) {
        *update = FALSE;
        return 0;
    }

    int count = 0;

    int compResult = t->comp(target, actual->key);

    if (compResult < 0) {
        count += 1 + __removeAVL__(t, actual, actual->left, target, update);

    } else if (compResult > 0) {
        count += 1 + __removeAVL__(t, actual, actual->right, target, update);

    } else {

        *update = TRUE;

        if (isAVLleaf(actual)) {

            if (parent) {
                updateParent(parent, actual, NULL);
            } else {
                t->root = NULL;
            }
            
            free(actual);
            actual = NULL;

        } else if (isHalfLeaf(actual)) {

            AVLnode *son = actual->left ? actual->left : actual->right;

            if (parent) {
                updateParent(parent, actual, son);
            } else {
                t->root = son;
            }

            free(actual);
            actual = son;

        } else {

            AVLnode *substitute = removeMinimum(t, actual, actual->right, update, &count);

            substitute->left = actual->left;

            if (substitute != actual->right)
                substitute->right = actual->right;

            if (parent) {
                updateParent(parent, actual, substitute);
            } else {
                t->root = substitute;
            }

            free(actual);
            actual = substitute;
        }
    }

    count++;

    if (*update) {
        updateHeight(actual);
        checkRebalance(t, parent, actual);
    }

    return count;
}

int removeAVL(AVL *t, void *key) {

    if (!t || !key || isAVLempty(t)) return 0;

    bool update = TRUE;
    return __removeAVL__(t, NULL, t->root, key, &update);
}

void postOrder(AVLnode *root, procedure *f) {
    if (!root) return;
    postOrder(root->left, f);
    postOrder(root->right, f);
    f(root);
}

void destroyAVL(AVL **t) {
    if (!t || !(*t)) return;
    postOrder((*t)->root, free);
    free(*t);
    t = NULL;
    return;
}
