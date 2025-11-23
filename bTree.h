#ifndef _BTREE_H
#define _BTREE_H

struct info{
    int valor;
};

struct node{
    struct info *keys ;
    struct node **children;
    int keysAmount;
    struct node *dad;
    int isLeaf; //1 == True , 0 == False
};

struct root{
    int order;
    struct node *rootNode;
};

struct root * createBTree(int order);
void addKey(struct root *root , struct info data , long unsigned int *count);
void delet(struct root *root , struct info aim , long unsigned int *count);
struct node * createNode(struct root *root);

#endif