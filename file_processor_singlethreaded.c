#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

int main(int argc, char * argv[]){
    if(argc < 2){
        printf("Incorrect number of arguments.\n");
        return 1;
    }
    int * data = NULL;
    int i = 0;
    int temp;
    FILE * fstream = fopen(argv[1], "r");

    while (fscanf(fstream, "%d", &temp) == 1) {
        data = realloc(data, (i+1) * sizeof(int));
        data[i]=temp;
        i++;
    }

    clock_t start_time = clock();
    // get minimum, maximum and sum
    long long int minimum = (int)INFINITY;
    long long int maximum = (int)-INFINITY;
    long long int sum = 0;
    for(int j = 0; j < i; j++){
        sum+=data[j];
        if(data[j] > maximum){
            maximum = data[j];
        }
        if(data[j] < minimum){
            minimum = data[j];
        }
    }

    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Sum: %I64d, Minimum: %I64d, Maximum: %I64d.\nTime: %f seconds.", sum, minimum, maximum, elapsed_time);

    free(data);
    return 0;
}