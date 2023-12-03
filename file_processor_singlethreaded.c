#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *fstream = fopen(argv[1], "r");
    if (fstream == NULL) {
        perror("Error opening file");
        return 1;
    }

    int *data = NULL;
    int i = 0;
    int temp;

    printf("Running...\n");

    while (fscanf(fstream, "%d", &temp) == 1) {
        data = realloc(data, (i + 1) * sizeof(int));
        if (data == NULL) {
            perror("Error allocating memory");
            fclose(fstream);
            free(data);
            return 1;
        }
        data[i] = temp;
        i++;
    }

    fclose(fstream);

    clock_t start_time = clock();

    // get minimum, maximum, and sum
    long long int minimum = (int) INFINITY;
    long long int maximum = (int) -INFINITY;
    long long int sum = 0;

    for (int j = 0; j < i; j++) {
        sum += data[j];
        if (data[j] > maximum) {
            maximum = data[j];
        }
        if (data[j] < minimum) {
            minimum = data[j];
        }
    }

    clock_t end_time = clock();
    double elapsed_time = (double) (end_time - start_time) / CLOCKS_PER_SEC;

    printf("Sum: %I64i, Minimum: %I64i, Maximum: %I64i.\nTime: %f seconds.\n", sum, minimum, maximum, elapsed_time);

    free(data);
    return 0;
}
