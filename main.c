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
    int valid;
};

void freeArray(struct Array *arr) {

    for (int i = 0; i < arr->items; i++) {
        free(arr->strings[i]);
    }
    
    free(arr->strings);
    
    arr->strings = NULL;
    arr->capacity = 0;
    arr->items = 0;
    
    free(arr);
}

void freeRow(struct Row *row) {

    free(row->county);
    free(row->state);
    free(row->nums);
    
    row->county = NULL;
    row->state = NULL;
    row->nums = NULL;
    row->valid = 0;

    free(row);
}

int main(int argc, char *argv[]){

    if (argc != 3){
        perror("Use the correct number of Arguments");
    }

    FILE *csv_file = fopen(argv[1], "r");
    if (csv_file == NULL) {
        perror("Error opening CSV file");
        return 1;
    }

    FILE *ops_file = fopen(argv[2], "r");
    if (ops_file == NULL) {
        perror("Error opening OPS file");
        return 1;
    }


    struct Array *header = malloc(sizeof(struct Array));
    header->strings = malloc(sizeof(char *) * 10);
    header->items = 0;
    header-> capacity = 10;

    char *line = NULL;
    size_t len = 0;
    if (getline(&line, (size_t *)&len, csv_file) == -1) {
        perror("Error getting line");
        free(line);
        return 1;
    }

    char *args;
    while((args = strsep(&line, ",")) != NULL){

        args[strlen(args) - 1] = '\0';
        args++;

        if(header->items >= header->capacity){
            header->capacity += 10;
            header->strings = realloc(header->strings, sizeof(char *) * header->capacity);
        }

        header->strings[header->items] = strdup(args);
        header->items += 1;
    }

    free(line);

    struct Row *counties[5000];
    int i = 0;
    int j = 0;
    int rows = 0;

    while (getline(&line, (size_t *)&len, csv_file) != -1) {
        rows++;
        counties[i] = malloc(sizeof(struct Row));
        counties[i]->valid = 1;
        args = strsep(&line, ",");
        counties[i]->county = strdup(args);
        args = strsep(&line, ",");
        counties[i]->state = strdup(args);
        float *nums_array = malloc(sizeof(float) * header->items);

        j = 0;
        while((args = strsep(&line, ",")) != NULL){
            args[strlen(args) - 1] = '\0';
            args++;
            nums_array[j] = atof(args);
            j++;
        }

        counties[i]->nums = nums_array;
        i++;
    }

    // get population index (cause we use that alot)
    int pop_index = -1;
    for(i = 0; i < header->items; i++){
        if (strcmp(header->strings[i], "Population.2014 Population") == 0){
            pop_index = i - 2;
            break;
        }
    }

    int sum;
    int sum2;
    float temp;
    float percent;
    int index; 

    while (getline(&line, (size_t *)&len, ops_file) != -1) {

        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }

        // display
        if (strcmp(line, "display") == 0) {
            for(i = 0; i < rows; i++) {
                if(counties[i]->valid) {
                    printf("%s %s: ", counties[i]->county, counties[i]->state);
                    for (j = 0; j < header->items - 2; j++) {
                        printf("%s: %.1f ", header->strings[j + 2], counties[i]->nums[j]);
                    }
                    printf("\n");
                }
            }
            continue;

        // population-total
        } else if (strcmp(line, "population-total") == 0) {
            
            if (pop_index == -1) {
                printf("Cant find population total");
            } else {
                sum = 0;
                for(i = 0; i < rows; i++) {
                    if(counties[i]->valid) {
                        sum += counties[i]->nums[pop_index];
                    }
                }
                printf("2014 Population: %i\n", sum);
                continue;
            }
        }

        args = strsep(&line, ":");

        if (args == NULL) {
            printf("Error geting Line");
            continue;
        }

        // population:<field>
        // percent:<field>
        if (strcmp(args, "population") == 0 || strcmp(args, "percent") == 0) {
            index = -1;
            for(i = 0; i < header->items; i++){
                if (strcmp(header->strings[i], line) == 0){
                    index = i - 2;
                    break;
                }   
            }

            if (index == -1) {
                printf("Cant find population total");
                continue;
            }

            sum = 0;
            for(i = 0; i < rows; i++) {
                if(counties[i]->valid) {
                    sum += counties[i]->nums[index];
                }
            }

            if (strcmp(args, "population") == 0) {
                printf("%s Population: %i\n", line, sum);
            } else {
                sum2 = 0;
                for(i = 0; i < rows; i++) {
                    if(counties[i]->valid) {
                        sum2 += counties[i]->nums[pop_index];
                    }
                }
                percent = (float)sum / sum2;
                printf("%s Population Percentage: %f\n", line, percent);
            }
            continue;
        
        // filter-state:<state abbreviation>
        } else if (strcmp(args, "filter-state") == 0) {
            for(i = 0; i < rows; i++) {
                if(counties[i]->valid && strcmp(counties[i]->state, line)) {
                    counties[i]->valid = 0;
                }
            }
            continue;

        // filter:<field>:<ge/le>:<number>
        } else if (strcmp(args, "filter") == 0) {
            char *field = strsep(&line, ":");
            char *gele = strsep(&line, ":");
            char *number = strsep(&line, ":");

            if (field == NULL || gele == NULL || number == NULL) {
                printf("Error getting line");
                continue;
            }

            temp = atof(number);

            index = -1;
            for(i = 0; i < header->items; i++){
                if (strcmp(header->strings[i], field) == 0){
                    index = i - 2;
                    break;
                }   
            }

            if (index == -1) {
                printf("Cant find population total");
                continue;
            }
            
            if (strcmp(gele, "ge") == 0) {
                for(i = 0; i < rows; i++) {
                    if(counties[i]->nums[index] < temp ) {
                        counties[i]->valid = 0;
                    }
                }

            } else if (strcmp(gele, "le") == 0) {
                for(i = 0; i < rows; i++) {
                    if(counties[i]->nums[index] > temp ) {
                        counties[i]->valid = 0;
                    }
                }

            } else {
                printf("Couldn't get ge/le");
                continue;
            }
            
            continue;
        }
    }

    freeArray(header);
    
    for(i = 0; i < rows; i++) {
        freeRow(counties[i]);
    }

    fclose(csv_file);
    fclose(ops_file);

}