#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int * readFile(char *s);
void writeFile(int *v);

int main (){

    int *v = readFile("randomFileInt.csv");
    int i , aux , r;

    srand(time(NULL));

    for(i = 0;i < 10000;i++){
        r = rand() % 10000;

        aux = v[i];
        v[i] = v[r];
        v[r] = aux;
    }

    writeFile(v);

    return 0;

}

int * readFile(char *s){

    FILE *file = NULL;
    int *v , i;

    file = fopen(s , "r");
    v = malloc(sizeof(int) * 10000);

    for(i = 0;i < 10000;i++){
        v[i] = fscanf(file , "%d" , &v[i]);
    }

    return v;

}

 void writeFile(int *v){

    int i;
    FILE *file = fopen("shuffled.csv" , "w");

    for(i = 0;i < 10000;i++){
        fprintf(file , "%d;" , v[i]);
    }

    free(v);

}