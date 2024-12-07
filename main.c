#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


struct Array {
    char **strings;
    int capacity;
    int items; 
};

struct Row {
    char *county;
    char *state;
    float *nums;
};

int main(int argc, char *argv[]){

    if (argc != 3){
        perror("Use the correct number of Arguments");
    }

    FILE *csv_file = fopen(argv[1], "r");
    if (csv_file == NULL) {
        perror("Error opening CSV file");
    }

    FILE *ops_file = fopen(argv[2], "r");
    if (ops_file == NULL) {
        perror("Error opening OPS file");
    }

    char *line = NULL;
    unsigned int len;

    if (getline(&line, (size_t *)&len, csv_file) == -1) {
        perror("Error getting line");
    }

    struct Array *header = malloc(sizeof(struct Array));
    header->strings = malloc(sizeof(char *) * 10);
    header->items = 0;
    header-> capacity = 10;

    char *args;
    while((args = strsep(&line, ",")) != NULL){

        if(header->items >= header->capacity){
            header->capacity += 10;
            header->strings = realloc(header->strings, sizeof(char *) * header->capacity);
        }

        header->strings[header->items] = strdup(args);
        header->items += 1;
    }

    struct Row *counties[5000];
    int i = 0;

    while (getline(&line, (size_t *)&len, csv_file) != -1) {
        counties[i] = malloc(sizeof(struct Row));
        args = strsep(&line, ",");
        counties[i]->county = strdup(args);
        args = strsep(&line, ",");
        counties[i]->state = strdup(args);
        float *nums_array = malloc(sizeof(float) * header->items);
        int j = 0;

        while((args = strsep(&line, ",")) != NULL){
            args[strlen(args) - 1] = '\0';
            args++;
            nums_array[j] = atof(args);
            j++;
        }

        counties[i]->nums = nums_array;
        i++;
    }

    printf("%s   %f\n", counties[4]->county, counties[4]->nums[4]);
}