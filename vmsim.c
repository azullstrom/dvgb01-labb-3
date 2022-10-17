#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 100
#define MAX_LENGTH 10

int pageFaults = 0, pageHits = 0, pageReplacement = 0, frameFull = 0;

typedef struct MemoryAddress {
    char address[MAX_LENGTH];
    char pageNumberHex[MAX_LENGTH];
    int pageNumberDec;
    char offsetHex[MAX_LENGTH];
    int offsetDec;
} MemoryAddress;

typedef struct Frames {
    char address[MAX_LENGTH];
    int hasElement;
    int arrivalTime;
} Frames;

void fifoAlgorithm(MemoryAddress memoryAddress[], Frames frames[], int size, int numberOfFrames);

int lines = 0;

int main(int argc, char* argv[]) {
    MemoryAddress memoryAddress[MAX] = {0};
    char fifo[] = "fifo", optimal[] = "optimal", lru[] = "lru";
    int fifoFalse, optimalFalse, lruFalse;
    char *ret;
    FILE *fp;

    fp = fopen(argv[1], "r");
    // Om filen inte existerar
    if(fp == NULL) {
        printf("Error: File not found\n");
        exit(EXIT_FAILURE);
    }

    // Lägger till varje rad i addresses på egna rader
    while(!feof(fp) && !ferror(fp)) {
        if(fgets(memoryAddress[lines].address, MAX_LENGTH, fp) != NULL) {
            lines++;
        }
    }
    fclose(fp);

    // Delar upp strängarna i page number och offset
    for(int i = 0; i < lines; i++) {
        for(int j = 2; j < 4; j++) {
            memoryAddress[i].pageNumberHex[j-2] = memoryAddress[i].address[j];
        }
        for(int j = 4; j < 6; j++) {
            memoryAddress[i].offsetHex[j-4] = memoryAddress[i].address[j];
        }
    }

    for(int i = 0; i < lines; i++) {
        memoryAddress[i].pageNumberDec = (int)strtol(memoryAddress[i].pageNumberHex, NULL, 16);
        memoryAddress[i].offsetDec = (int)strtol(memoryAddress[i].offsetHex, NULL, 16);
    }

    // Om strängen innehåller .dat eller inte
    ret = strstr(argv[1], ".dat");
    if(!ret) {
        printf("Error: First parameter has to be the file.dat\nand the second parameter has to be the algorithm name\nand the third has to be number of frames.\n");
        exit(1);
    } else {
        printf("\nProcess information file: %s\n", argv[1]);
    }
    
    // Om argumenten inte är 3 eller 4 (./sched .txt arg)
    if(argc < 4) {
        printf("Error: Less parameters than allowed\n");
        exit(2);
    } else if(argc > 4) {
        printf("Error: More parameters than allowed\n");
        exit(3);
    }

    // Bestämmer vilken algoritm som ska köras
    fifoFalse = strcmp(argv[2], fifo);
    optimalFalse = strcmp(argv[2], optimal);
    lruFalse = strcmp(argv[2], lru);

    char *p;
    int numberOfFrames = (int)strtol(argv[3], &p, 10);
    Frames frames[numberOfFrames];

    for(int i = 0; i < numberOfFrames; i++) {
        frames[i].hasElement = 0;
    }
    
    if(!fifoFalse) {
        fifoAlgorithm(memoryAddress, frames, lines, numberOfFrames);
    } else if(!optimalFalse) {

    } else if(!lruFalse) {
    
    } else {
        printf("Error: First parameter has to be the file.dat\nand the second parameter has to be the algorithm name\nand the third has to be number of frames.\n");
        exit(1);
    }

    return 0;
}

void fifoAlgorithm(MemoryAddress memoryAddress[], Frames frames[], int size, int numberOfFrames) {
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < numberOfFrames; j++) {
            if(frames[j].hasElement) {
                frames[j].arrivalTime++;
            }

            for(int k = 0; k < numberOfFrames; k++) {
                if(frames[k].hasElement == 0) {
                    break;
                }
                if(k == numberOfFrames - 1) {
                    frameFull = 1;
                    printf("full\n");
                }
            }

            if(frames[j].hasElement == 0) {
                pageFaults++;
                frames[j].hasElement = 1;
                strcpy(frames[j].address, memoryAddress[i].address);
                break;
            } else if(frames[j].hasElement && strcmp(memoryAddress[i].address, frames[j].address) == 0) {
                pageHits++;
                break;
            } else if(frames[j].hasElement && strcmp(memoryAddress[i].address, frames[j].address) != 0 && frameFull) {
                int removeIndex;
                for(int k = 0; k < numberOfFrames; k++) {
                    for(int l = 0; l < numberOfFrames; l++) {
                        if(frames[k].arrivalTime > frames[l].arrivalTime) {
                            removeIndex = k;
                        } 
                    }
                }
                break;
            } 
        }
    }

    for(int i = 0; i < numberOfFrames; i++) {
        printf("%s", frames[i].address);
    }
    printf("full: %d\n", frameFull);
}