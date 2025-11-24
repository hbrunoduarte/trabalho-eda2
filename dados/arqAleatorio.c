#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void fileWrite(int *v);
int contains(int *v , int max , int n);

int main (){

    int n = 0 , v[10000] , temp;

    srand(time(NULL));

    v[0] = rand();
    n++;

    while(n < 10000){
        temp = rand();
        if(contains(v , n , temp) == 0){
            v[n] = temp;
            n++;
        }
    }
    
    fileWrite(v);

    return 0;
}

int contains(int *v , int max , int n){
    int i;

    for(i = 0;i < max;i++){
        if(n == v[i])
            return 1;
    }

    return 0;
}

void fileWrite(int *v){
    FILE *file = NULL;
    int i;
    char *c = NULL;

    file = fopen("randomFileInt.csv" , "w");

    for(i = 0;i < 10000;i++){
        fprintf(file , "%d;" , v[i]);
    }

    fclose(file);

}