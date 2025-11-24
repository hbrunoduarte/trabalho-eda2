#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "avl.h"
#include "rn.h"
#include "bTree.h"

#define TESTS 10

#define AVL_POS 0
#define RN_POS 1
#define B1_POS 2
#define B5_POS 3
#define B10_POS 4

#define AVL_FILE "avl.csv"
#define RN_FILE "rn.csv"
#define BTREE1_FILE "btree1.csv"
#define BTREE5_FILE "btree5.csv"
#define BTREE10_FILE "btree10.csv"

#define ANALYSED_TREES 5
#define MAX_ELEMENTS 10000
#define STEP 1000
#define SAMPLES MAX_ELEMENTS/STEP
#define RANGE_ELEMENTS 2 * MAX_ELEMENTS

struct info createInfo(int key) {
    struct info i;
    i.valor = key;
    return i;
}

int compareInt(const void *a, const void *b) {
    return *((int*)a) - *((int*)b);
}

int initVars(AVL **avl, ArvoreRN **rn, struct root **bTree1, struct root **bTree5, struct root **bTree10) {

    *avl = createAVL(compareInt);
    if (!avl) return -1;

    *rn = criarAvoreRN();
    if (!rn) {
        destroyAVL(avl);
        return -1;
    }

    *bTree1 = createBTree(1);
    if (!(*bTree1)) {
        destroyAVL(avl);
        destruirRN(rn);
        return -1;
    }

    *bTree5 = createBTree(5);
    if (!(*bTree5)) {
        destroyAVL(avl);
        destruirRN(rn);
        destroyBTree(bTree1);
        return -1;
    }

    *bTree10 = createBTree(10);
    if (!(*bTree10)) {
        destroyAVL(avl);
        destruirRN(rn);
        destroyBTree(bTree1);
        destroyBTree(bTree5);
        return -1;
    }

    return 1;
}

int* createSample() {

    int *s = malloc(sizeof(int) * MAX_ELEMENTS);
    if (!s) return NULL;

    int poolSize = RANGE_ELEMENTS; 
    int *pool = malloc(sizeof(int) * poolSize);
    if (!pool) {
        free(s);
        return NULL;
    }

    for (int i = 0; i < poolSize; i++) {
        pool[i] = i - (poolSize / 2); 
    }

    // Algoritmo Fisher-Yates Shuffle
    for (int i = 0; i < MAX_ELEMENTS; i++) {
        int j = i + rand() % (poolSize - i);
        int temp = pool[j];
        pool[j] = pool[i];
        pool[i] = temp;
        s[i] = pool[i];
    }

    free(pool);
    return s;
}

unsigned long** saveIterationsVector() {

    unsigned long **v = malloc(sizeof(unsigned long*) * ANALYSED_TREES);
    if (!v) return NULL;

    for (int i = 0; i < ANALYSED_TREES; i++) {
        v[i] = malloc(sizeof(unsigned long) * SAMPLES);
        if (!v[i]) {
            for (int j = 0; j < i; j++)
                free(v[j]);
            free(v);
            return NULL;
        }
    }

    return v;
}

unsigned long** averageVector() {
    unsigned long **v = malloc(sizeof(unsigned long*) * ANALYSED_TREES);
    if (!v) return NULL;

    for (int i = 0; i < ANALYSED_TREES; i++) {
        v[i] = calloc(SAMPLES, sizeof(unsigned long));
        if (!v[i]) {
            for (int j = 0; j < i; j++)
                free(v[j]);
            free(v);
            return NULL;
        }
    }

    return v;
}

void freeMatrixElements(void **matrix, int lines) {
    for (int i = 0; i < lines; i++)
        free(matrix[i]);
}

void saveAverages(char *fileName, unsigned long *insertAverages, unsigned long *deletionAverages) {

    FILE *file = fopen(fileName, "w+");
    if (!file) return;

    for (int i = STEP; i <= MAX_ELEMENTS; i += STEP) {
        if (i != MAX_ELEMENTS)
            fprintf(file, "i_%i,", i);
        else
            fprintf(file, "i_%i\n", i);
    }

    for (int i = 0; i < SAMPLES; i++) {
        if (i != SAMPLES-1)
            fprintf(file, "%lu,", insertAverages[i]);
        else
            fprintf(file, "%lu\n", insertAverages[i]);
    }

    for (int i = 0; i < SAMPLES; i++) {
        if (i != SAMPLES-1)
            fprintf(file, "%lu,", deletionAverages[i]);
        else
            fprintf(file, "%lu", deletionAverages[i]);
    }

    fclose(file);
}

int main() {

    srand(time(NULL));

    unsigned long **average_inserts = averageVector();
    if (!average_inserts) return -1;

    unsigned long **average_deletions = averageVector();
    if (!average_deletions) {
        freeMatrixElements((void**)average_inserts, ANALYSED_TREES);
        free(average_inserts);
        return -1;
    }

    for (int i = 0; i < ANALYSED_TREES; i++)
        for (int j = 0; j < SAMPLES; j++)
            average_inserts[i][j] = average_deletions[i][j] = 0;

    for (int test = 1; test <= TESTS; test++) {

        printf("\n[Amostragem %i]\n", test);

        int *sample = createSample();
        if (!sample) return -1;

        unsigned long **saved_Insert_Iterations = saveIterationsVector();
        if (!saved_Insert_Iterations) {
            free(sample);
            freeMatrixElements((void**)average_inserts, ANALYSED_TREES);
            free(average_inserts);
            freeMatrixElements((void**)average_deletions, ANALYSED_TREES);
            free(average_deletions);
            return -1;
        }

        unsigned long **saved_Remove_Iterations = saveIterationsVector();
        if (!saved_Remove_Iterations) {
            free(sample);
            freeMatrixElements((void**)average_inserts, ANALYSED_TREES);
            free(average_inserts);
            freeMatrixElements((void**)average_deletions, ANALYSED_TREES);
            free(average_deletions);
            freeMatrixElements((void**)saved_Insert_Iterations, ANALYSED_TREES);
            free(saved_Insert_Iterations);
            return -1;
        }

        AVL *avl;
        ArvoreRN *rn;
        struct root *bTree1;
        struct root *bTree5;
        struct root *bTree10;

        if (initVars(&avl, &rn, &bTree1, &bTree5, &bTree10) != 1) {
            free(sample);
            freeMatrixElements((void**)average_inserts, ANALYSED_TREES);
            free(average_inserts);
            freeMatrixElements((void**)average_deletions, ANALYSED_TREES);
            free(average_deletions);
            freeMatrixElements((void**)saved_Insert_Iterations, ANALYSED_TREES);
            free(saved_Insert_Iterations);
            freeMatrixElements((void**)saved_Remove_Iterations, ANALYSED_TREES);
            free(saved_Remove_Iterations);
            return -1;
        }

        printf("\n\t- Variaveis inicializadas\n");

        unsigned long insert_Iterations_AVL = 0;
        unsigned long insert_Iterations_RN = 0;
        unsigned long insert_Iterations_B1 = 0;
        unsigned long insert_Iterations_B5 = 0;
        unsigned long insert_Iterations_B10 = 0;

        printf("\t- Iniciando insercao\n");

        int samplePosition = 0;
        for (int i = 0; i < MAX_ELEMENTS; i++) {

            insert_Iterations_AVL += insertAVL(avl, sample+i);
            printf("\n\tInsercao %i - chave %i\n\tAvl feito\n", i+1, sample[i]);
            insert_Iterations_RN += inserirRN(rn, sample[i]);
            printf("\tRN feito\n");

            struct info bElement = createInfo(sample[i]);
            
            addKey(bTree1, bElement, &insert_Iterations_B1);
            printf("\tb1 feito\n");
            addKey(bTree5, bElement, &insert_Iterations_B5);
            printf("\tb5 feito\n");
            addKey(bTree10, bElement, &insert_Iterations_B10);
            printf("\tb10 feito\n");

            if ((i+1) % STEP == 0) {
                saved_Insert_Iterations[AVL_POS][samplePosition] = insert_Iterations_AVL;
                saved_Insert_Iterations[RN_POS][samplePosition] = insert_Iterations_RN;
                saved_Insert_Iterations[B1_POS][samplePosition] = insert_Iterations_B1;
                saved_Insert_Iterations[B5_POS][samplePosition] = insert_Iterations_B5;
                saved_Insert_Iterations[B10_POS][samplePosition] = insert_Iterations_B10;
                samplePosition++;
            }
        }

        printf("\t- Insercao finalizada\n");

        for (int i = 0; i < ANALYSED_TREES; i++) {
            for (int j = 0; j < SAMPLES; j++) {
                average_inserts[i][j] += saved_Insert_Iterations[i][j];
            }
        }

        unsigned long remove_Iterations_AVL = 0;
        unsigned long remove_Iterations_RN = 0;
        unsigned long remove_Iterations_B1 = 0;
        unsigned long remove_Iterations_B5 = 0;
        unsigned long remove_Iterations_B10 = 0;

        printf("\t- Iniciando remocao\n");

        samplePosition = 0;
        for (int i = 0; i < MAX_ELEMENTS && !isAVLempty(avl); i++) {

            remove_Iterations_AVL += removeAVL(avl, sample+i);
            remove_Iterations_RN += removerRN(rn, sample[i]);

            struct info bElement = createInfo(sample[i]);
            delet(bTree1, bElement, &remove_Iterations_B1);
            delet(bTree5, bElement, &remove_Iterations_B5);
            delet(bTree10, bElement, &remove_Iterations_B10);

            if ((i+1) % STEP == 0) {
                saved_Remove_Iterations[AVL_POS][samplePosition] = remove_Iterations_AVL;
                saved_Remove_Iterations[RN_POS][samplePosition] = remove_Iterations_RN;
                saved_Remove_Iterations[B1_POS][samplePosition] = remove_Iterations_B1;
                saved_Remove_Iterations[B5_POS][samplePosition] = remove_Iterations_B5;
                saved_Remove_Iterations[B10_POS][samplePosition] = remove_Iterations_B10;
                samplePosition++;
            }
        }

        printf("\t- Remocao finalizada\n");

        if (samplePosition != SAMPLES) {
            saved_Remove_Iterations[AVL_POS][samplePosition] = remove_Iterations_AVL;
            saved_Remove_Iterations[RN_POS][samplePosition] = remove_Iterations_RN;
            saved_Remove_Iterations[B1_POS][samplePosition] = remove_Iterations_B1;
            saved_Remove_Iterations[B5_POS][samplePosition] = remove_Iterations_B5;
            saved_Remove_Iterations[B10_POS][samplePosition] = remove_Iterations_B10;
        }

        for (int i = 0; i < ANALYSED_TREES; i++) {
            for (int j = 0; j < SAMPLES; j++) {
                average_deletions[i][j] += j < samplePosition ? saved_Remove_Iterations[i][j] : saved_Remove_Iterations[i][samplePosition];
            }
        }

        destroyAVL(&avl);
        destruirRN(&rn);
        destroyBTree(&bTree1);
        destroyBTree(&bTree5);
        destroyBTree(&bTree10);

        free(sample);
        freeMatrixElements((void**)saved_Insert_Iterations, ANALYSED_TREES);
        free(saved_Insert_Iterations);
        freeMatrixElements((void**)saved_Remove_Iterations, ANALYSED_TREES);
        free(saved_Remove_Iterations);

        printf("\n[Amostragem %i concluida]\n", test);

    }

    for (int i = 0; i < ANALYSED_TREES; i++) {
        for (int j = 0; j < SAMPLES; j++) {
            average_inserts[i][j] /= TESTS;
            average_deletions[i][j] /= TESTS;
        }
    }

    saveAverages(AVL_FILE, average_inserts[AVL_POS], average_deletions[AVL_POS]);
    saveAverages(RN_FILE, average_inserts[RN_POS], average_deletions[RN_POS]);
    saveAverages(BTREE1_FILE, average_inserts[B1_POS], average_deletions[B1_POS]);
    saveAverages(BTREE5_FILE, average_inserts[B5_POS], average_deletions[B5_POS]);
    saveAverages(BTREE10_FILE, average_inserts[B10_POS], average_deletions[B10_POS]);

    freeMatrixElements((void**)average_inserts, ANALYSED_TREES);
    free(average_inserts);
    freeMatrixElements((void**)average_deletions, ANALYSED_TREES);
    free(average_deletions);

    return 0;
}