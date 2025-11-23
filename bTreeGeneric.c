#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bTree.h"

int compare(struct info info1 , struct info info2);
void recursiveAddNode(struct root *root , struct node *node , struct node *new , struct info data , long unsigned int *count);
struct node * split(struct root *root , struct node *node , long unsigned int *count);
int overflow(struct node *node , struct root *root);
void add(struct node *node , struct node *right , struct info *new , long unsigned int *count);
struct node * locateNode(struct root *root , struct info aim , long unsigned int *count);
void freeOnNode(struct node *node);
int binarySearch(struct node *node , struct info aim , long unsigned int *count);
int searchNode(struct root *root , struct info aim , long unsigned int *count);
void underflow(struct node *dad , int son , struct root *root , long unsigned int *count);
void recursiveDeletNode(struct node *node , struct info aim , long unsigned int *count , struct root *root);

int compare(struct info info1 , struct info info2){//if(info1 > info2)->return > 0 || if(info1 < info2)->return < 0 
    return info1.valor - info2.valor;
}

struct node * createNode(struct root *root){
    struct node *new = NULL;
    int i;

    new = malloc(sizeof(struct node));

    new->keys = malloc(sizeof(struct info) * (2 * root->order + 1));
    new->children = malloc(sizeof(struct node *) * (2 * (root->order) + 2));

    for(i = 0;i < 2* root->order + 2;i++)
        new->children[i] = NULL;
    
    new->keysAmount = 0;

    new->isLeaf = 1;

    return new;
}

int searchNode(struct root *root , struct info aim , long unsigned int *count){

    int n , i;
    struct node *node = NULL;
    node = root->rootNode;

    while(node != NULL){
        (*count)++;

        n = binarySearch(node , aim , count);

        if(n < node->keysAmount && compare(node->keys[n] , aim) == 0){
            return 1;
        }else{
            node = node->children[n];
        }

    }

    return 0;

}

int binarySearch(struct node *node , struct info aim , long unsigned int *count){

    int min = 0 , max = node->keysAmount - 1, mid = 0 , c;

    while(min <= max){

        (*count)++;

        mid = (max + min) / 2;
        
        c = compare(node->keys[mid] , aim);

        if(c == 0)
            return mid;
        
        if(c > 0)
            max = mid - 1;
        else
            min = mid + 1;
    }

    return min;

}

void freeOnNode(struct node *node){
    free(node->keys);
    free(node->children);
    free(node);
}

struct root * createBTree(int order){
    struct root *new = NULL;
    new = malloc(sizeof(struct root));
    new->order = order;
    new->rootNode = createNode(new);
    return new;
}

struct node * locateNode(struct root *root , struct info aim , long unsigned int *count){

    struct node *node = NULL;
    node = root->rootNode;
    int n;

    while(node != NULL){
        (*count)++;
        n = binarySearch(node , aim , count);

        if(node->isLeaf == 1)
            return node;
        else
            node = node->children[n];
    }

    return NULL;

}

void add(struct node *node , struct node *right , struct info *new , long unsigned int *count){
    int n = binarySearch(node , *new , count) , i;

    for(i = node->keysAmount - 1;i >= n;i--){
        (*count)++;
        memcpy(&node->keys[i + 1] , &node->keys[i] , sizeof(struct info));
        memcpy(&node->children[i + 2] , &node->children[i + 1] , sizeof(struct node *));
    }

    memcpy(&node->keys[n] , new , sizeof(struct info));
    memcpy(&node->children[n + 1] , &right , sizeof(struct node *));

    node->keysAmount++;

}

int overflow(struct node *node , struct root *root){
    if(node->keysAmount > root->order * 2)
        return 1;
    return 0;
}


struct node * split(struct root *root , struct node *node , long unsigned int *count){

    struct node *new = createNode(root);
    int mid = node->keysAmount / 2 , i;
    new->dad = node->dad;
    new->isLeaf = node->isLeaf;

    for(i = mid + 1;i < node->keysAmount;i++){
        (*count)++;
        memcpy(&new->keys[new->keysAmount] , &node->keys[i] , sizeof(struct info));
        memcpy(&new->children[new->keysAmount] , &node->children[i] , sizeof(struct node *));

        if(new->children[new->keysAmount] != NULL)
            new->children[new->keysAmount]->dad = new;

        new->keysAmount++;

    }

    new->children[new->keysAmount] = node->children[node->keysAmount];

    if(new->children[new->keysAmount] != NULL)
        new->children[new->keysAmount]->dad = new;

    node->keysAmount = mid;

    return new;

}

void addKey(struct root *root , struct info data , long unsigned int *count){
    struct node *node = NULL;
    node = locateNode(root , data , count);

    recursiveAddNode(root , node , NULL , data , count);

}

void recursiveAddNode(struct root *root , struct node *node , struct node *new , struct info data , long unsigned int *count){

    add(node , new , &data , count);

    if(overflow(node , root) == 1){
        struct info promoted;

        memcpy(&promoted , &node->keys[root->order] , sizeof(struct info));

        struct node *new = split(root , node , count);

        if(node->dad == NULL){
            struct node *rootNode = createNode(root);
            rootNode->children[0] = node;
            add(rootNode , new , &promoted , count);
            rootNode->isLeaf = 0;

            node->dad = rootNode;
            new->dad = rootNode;
            root->rootNode = rootNode;
        }else
            recursiveAddNode(root , node->dad , new , promoted , count);
    }

}

void recursiveDeletNode(struct node *node , struct info aim , long unsigned int *count , struct root *root){

    int n = binarySearch(node , aim , count) , i;

    if(n < node->keysAmount && compare(node->keys[n] , aim) == 0){
        if(node->isLeaf == 1){
            for(i = n;i < node->keysAmount - 1;i++){
                (*count)++;
                memcpy(&node->keys[i] , &node->keys[i + 1] , sizeof(struct info));
            }
            node->keysAmount--;
            return;        
        }else{
            struct node *aux = node->children[n + 1];
            while(aux->isLeaf != 1){
                (*count)++;
                aux = aux->children[0];
            }
            memcpy(&node->keys[n] , &aux->keys[0] , sizeof(struct info));
            recursiveDeletNode(node->children[n + 1] , aux->keys[0] , count , root);

            if(node->children[n + 1]->keysAmount < root->order){
                underflow(node , n + 1 , root , count);
            }
        }
    }else{
        if(node->isLeaf == 1)
            return;
        else{
            recursiveDeletNode(node->children[n] , aim , count , root);   

            if(node->children[n]->keysAmount < root->order){
                underflow(node , n , root , count);
            }
            return;
        }
    }

}

void delet(struct root *root , struct info aim , long unsigned int *count){

    if(root == NULL || root->rootNode == NULL)
        return;

    recursiveDeletNode(root->rootNode , aim , count , root);

    if(root->rootNode->keysAmount == 0){

        struct node *temp = root->rootNode;

        if(root->rootNode->isLeaf == 1)
            root->rootNode = NULL;
        else{
            root->rootNode = root->rootNode->children[0];
            root->rootNode->dad = NULL;
        }
        freeOnNode(temp);
    }
}

void underflow(struct node *dad , int son , struct root *root , long unsigned int *count){

    int i , temp;
    struct info aux;

    if(son > 0){
        if(dad->children[son - 1]->keysAmount > root->order){
            
            memcpy(&aux , &dad->keys[son - 1] , sizeof(struct info));

            memcpy(&dad->keys[son - 1] , &dad->children[son - 1]->keys[dad->children[son - 1]->keysAmount - 1] , sizeof(struct info));
        
            temp = dad->children[son]->keysAmount;

            memcpy(&dad->children[son]->children[dad->children[son]->keysAmount + 1] , &dad->children[son]->children[dad->children[son]->keysAmount] , sizeof(struct node *));
            for(i = temp;i >= 0;i--){
                (*count)++;
                memcpy(&dad->children[son]->keys[i + 1] , &dad->children[son]->keys[i] , sizeof(struct info));
                memcpy(&dad->children[son]->children[i + 1] , &dad->children[son]->children[i] , sizeof(struct node *));
            }

            dad->children[son]->children[0] = NULL;

            memcpy(&dad->children[son]->keys[0] , &aux , sizeof(struct info));

            if(dad->children[son - 1]->isLeaf == 0){
                memcpy(&dad->children[son]->children[0] , &dad->children[son - 1]->children[dad->children[son - 1]->keysAmount] , sizeof(struct node *));
                dad->children[son - 1]->children[dad->children[son - 1]->keysAmount] = NULL;
                if(dad->children[son]->children[0] != NULL){
                    dad->children[son]->children[0]->dad = dad->children[son];
                }
            }

            dad->children[son - 1]->keysAmount--;
            dad->children[son]->keysAmount++;
        
            return;
        }
    }

    if(son < dad->keysAmount){
        if(dad->children[son + 1]->keysAmount > root->order){

            memcpy(&aux , &dad->keys[son] , sizeof(struct info));
            memcpy(&dad->keys[son] , &dad->children[son + 1]->keys[0] , sizeof(struct info));

            memcpy(&dad->children[son]->keys[dad->children[son]->keysAmount] , &aux , sizeof(struct info));

            if(dad->children[son]->isLeaf == 0){
                memcpy(&dad->children[son]->children[dad->children[son]->keysAmount + 1] , &dad->children[son + 1]->children[0] , sizeof(struct node *));
              
                if(dad->children[son]->children[dad->children[son]->keysAmount + 1] != NULL)
                    dad->children[son]->children[dad->children[son]->keysAmount + 1]->dad = dad->children[son];
            }

            temp = dad->children[son + 1]->keysAmount;
            for(i = 0;i < temp - 1;i++){
                (*count)++;
                memcpy(&dad->children[son + 1]->keys[i] , &dad->children[son + 1]->keys[i + 1] , sizeof(struct info));

                if(dad->children[son + 1]->isLeaf == 0){
                    memcpy(&dad->children[son + 1]->children[i] , &dad->children[son + 1]->children[i + 1] , sizeof(struct node *));
                }
            }

            memcpy(&dad->children[son + 1]->children[temp - 1] , &dad->children[son + 1]->children[temp] , sizeof(struct node *));

            dad->children[son]->keysAmount++;
            dad->children[son + 1]->keysAmount--;

            return;
            
        }
    }

    
    struct node *left = NULL , *right = NULL;
    int midKey;

    if(son > 0){
        left = dad->children[son - 1];
        right = dad->children[son];
        midKey = son - 1;
    }else{
        left = dad->children[son];
        right = dad->children[son + 1];
        midKey = son;
    }

    memcpy(&left->keys[left->keysAmount] , &dad->keys[midKey] , sizeof(struct info));

    for(i = 0;i < right->keysAmount;i++){
        (*count)++;
        memcpy(&left->keys[left->keysAmount + i + 1] , &right->keys[i] , sizeof(struct info));
            
        if(right->isLeaf == 0){
            left->children[left->keysAmount + i + 1] = right->children[i];
            left->children[left->keysAmount + i + 1]->dad = left;
        }
    }

    if(right->isLeaf == 0){
        left->children[left->keysAmount + i + 1] = right->children[i];
        left->children[left->keysAmount + i + 1]->dad = left;
    }

    for(i = midKey;i < dad->keysAmount - 1;i++){
        (*count)++;

        memcpy(&dad->keys[i] , &dad->keys[i + 1] , sizeof(struct info));
        dad->children[i + 1] = dad->children[i + 2];
    }

    dad->keysAmount--;
    left->keysAmount = left->keysAmount + right->keysAmount + 1;

    free(right->children);
    free(right->keys);
    free(right);

    return;

}