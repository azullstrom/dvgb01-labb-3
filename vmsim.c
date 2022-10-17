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
    int pageNumberDec;
    int hasElement;
    int arrivalTime;
    int leastRecent;
} Frames;

void fifoAlgorithm(MemoryAddress memoryAddress[], Frames frames[], int size, int numberOfFrames);
void lruAlgorithm(MemoryAddress memoryAddress[], Frames frames[], int size, int numberOfFrames);
void optimalAlgorithm(MemoryAddress memoryAddress[], Frames frames[], int size, int numberOfFrames);

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
        frames[i].arrivalTime = 0;
        frames[i].pageNumberDec = 0;
        frames[i].leastRecent = 0;
    }
    
    if(!fifoFalse) {
        printf("Algorithm: FIFO\nFrames: %d\nMemory accesses: %d\n", numberOfFrames, lines);
        fifoAlgorithm(memoryAddress, frames, lines, numberOfFrames);
    } else if(!optimalFalse) {
        printf("Algorithm: Optimal\nFrames: %d\nMemory accesses: %d\n", numberOfFrames, lines);
        optimalAlgorithm(memoryAddress, frames, lines, numberOfFrames);
    } else if(!lruFalse) {
        printf("Algorithm: LRU\nFrames: %d\nMemory accesses: %d\n", numberOfFrames, lines);
        lruAlgorithm(memoryAddress, frames, lines, numberOfFrames);
    } else {
        printf("Error: First parameter has to be the file.dat\nand the second parameter has to be the algorithm name\nand the third has to be number of frames.\n");
        exit(1);
    }

    return 0;
}

void checkFull(Frames frames[], int numberOfFrames) {
    for(int i = 0; i < numberOfFrames; i++) {
        if(frames[i].hasElement == 0) {
            break;
        }
        if(i == numberOfFrames - 1) {
            frameFull = 1;
            //printf("full\n");
        }
    }
}

int getFirstArrival(Frames frames[], int numberOfFrames) {
    int removeIndex = 0;
    int max = frames[0].arrivalTime;

    for (int i = 0; i < numberOfFrames; i++){
        if(frames[i].arrivalTime > max) {
            max = frames[i].arrivalTime;
            removeIndex = i;
        }
    }

    return removeIndex;
}

int getLeastRecent(Frames frames[], int numberOfFrames) {
    int removeIndex = 0;
    int max = frames[0].leastRecent;

    for (int i = 0; i < numberOfFrames; i++){
        if(frames[i].leastRecent > max) {
            max = frames[i].leastRecent;
            removeIndex = i;
        }
    }

    return removeIndex;
}

int getOptimal(int referenceIndex, Frames frames[], MemoryAddress memoryAddress[], int numberOfFrames, int size) {
    int count[numberOfFrames];

    for(int i = 0; i < numberOfFrames; i++) {
        // Check if exists at all
        for(int j = referenceIndex; j < size; j++) {
            if(frames[i].pageNumberDec != memoryAddress[j].pageNumberDec) {
                if(j == size - 1) {
                    count[i] = 1000; // Flag to remove because this one does not exist in remaining reference list
                    break;
                }
            }
        }
        // Otherwise check frame that occurrs last
        for(int j = referenceIndex; j < size; j++) {
            if(frames[i].pageNumberDec == memoryAddress[j].pageNumberDec) {
                break;
            }
            count[i]++;
        }
    }

    int removeIndex = 0;
    int max = count[0];
    for (int i = 0; i < numberOfFrames; i++){
        if(count[i] > max) {
            max = count[i];
            removeIndex = i;
        }
    }

    return removeIndex;
}

int pageHitInFrame(int pageNumberDec, Frames frames[], int numberOfFrames) {
    for(int i = 0; i < numberOfFrames; i++) {
        if(pageNumberDec == frames[i].pageNumberDec) {
            return 1;
        }
    }
    return 0;
}

void fifoAlgorithm(MemoryAddress memoryAddress[], Frames frames[], int size, int numberOfFrames) {
    for(int i = 0; i < size; i++) {
        // Incrementing arrival time for every frame that hasElement
        for(int j = 0; j < numberOfFrames; j++) {
            if(frames[j].hasElement) {
                frames[j].arrivalTime++;
            }
        }
        for(int j = 0; j < numberOfFrames; j++) {
            //printf("j: %d frame: %d memory: %d HAS ELEMENT: %d\n", j, frames[j].pageNumberDec, memoryAddress[i].pageNumberDec, frames[j].hasElement);
            checkFull(frames, numberOfFrames); 

            if(frames[j].hasElement == 0) {
                pageFaults++; 
                frames[j].hasElement = 1;
                strcpy(frames[j].address, memoryAddress[i].address);
                frames[j].pageNumberDec = memoryAddress[i].pageNumberDec;
                //printf("page fault: %s", memoryAddress[i].address);
                break;
            } else if(frames[j].hasElement && frames[j].pageNumberDec == memoryAddress[i].pageNumberDec) {
                //printf("hit: %s", memoryAddress[i].address);
                pageHits++;
                break;
            } else if(frames[j].hasElement && !pageHitInFrame(memoryAddress[i].pageNumberDec, frames, numberOfFrames) && frameFull) {
                int index = getFirstArrival(frames, numberOfFrames);
                //printf("replace %s with %s \n", frames[index].address, memoryAddress[i].address);
                strcpy(frames[index].address, memoryAddress[i].address);
                frames[index].pageNumberDec = memoryAddress[i].pageNumberDec;
                frames[index].arrivalTime = 0;
                pageFaults++;
                pageReplacement++;
                break;
            } 
        }
    } 

    printf("Page Hits: %d\nPage Faults: %d\nPage Replacements: %d\n", pageHits, pageFaults, pageReplacement);
}

void lruAlgorithm(MemoryAddress memoryAddress[], Frames frames[], int size, int numberOfFrames) {
    for(int i = 0; i < size; i++) {
        // Incrementing arrival time for every frame that hasElement
        for(int j = 0; j < numberOfFrames; j++) {
            if(frames[j].hasElement) {
                frames[j].leastRecent++;
            }
        }

        for(int j = 0; j < numberOfFrames; j++) {
            //printf("j: %d frame: %d memory: %d HAS ELEMENT: %d\n", j, frames[j].pageNumberDec, memoryAddress[i].pageNumberDec, frames[j].hasElement);
            
            checkFull(frames, numberOfFrames);

            if(frames[j].hasElement == 0) {
                pageFaults++;
                frames[j].hasElement = 1;
                strcpy(frames[j].address, memoryAddress[i].address);
                frames[j].pageNumberDec = memoryAddress[i].pageNumberDec;
                //printf("page fault: %s", memoryAddress[i].address);
                break;
            } else if(frames[j].hasElement && frames[j].pageNumberDec == memoryAddress[i].pageNumberDec) {
                //printf("hit: %s", memoryAddress[i].address);
                frames[j].leastRecent = 0;
                pageHits++;
                break;
            } else if(frames[j].hasElement && !pageHitInFrame(memoryAddress[i].pageNumberDec, frames, numberOfFrames) && frameFull) {
                int index = getLeastRecent(frames, numberOfFrames);
                //printf("replace %s with %s\n", frames[index].address, memoryAddress[i].address);
                strcpy(frames[index].address, memoryAddress[i].address);
                frames[index].pageNumberDec = memoryAddress[i].pageNumberDec;
                frames[index].leastRecent = 0;
                pageFaults++;
                pageReplacement++;
                break;
            } 
        }
    }

    printf("Page Hits: %d\nPage Faults: %d\nPage Replacements: %d\n", pageHits, pageFaults, pageReplacement);
}

void optimalAlgorithm(MemoryAddress memoryAddress[], Frames frames[], int size, int numberOfFrames) {
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < numberOfFrames; j++) {
            //printf("j: %d frame: %d memory: %d HAS ELEMENT: %d\n", j, frames[j].pageNumberDec, memoryAddress[i].pageNumberDec, frames[j].hasElement);
            
            checkFull(frames, numberOfFrames);

            if(frames[j].hasElement == 0) {
                pageFaults++;
                frames[j].hasElement = 1;
                strcpy(frames[j].address, memoryAddress[i].address);
                frames[j].pageNumberDec = memoryAddress[i].pageNumberDec;
                //printf("page fault: %s", memoryAddress[i].address);
                break;
            } else if(frames[j].hasElement && frames[j].pageNumberDec == memoryAddress[i].pageNumberDec) {
                //printf("hit: %s", memoryAddress[i].address);
                pageHits++;
                break;
            } else if(frames[j].hasElement && !pageHitInFrame(memoryAddress[i].pageNumberDec, frames, numberOfFrames) && frameFull) {
                int index = getOptimal(i, frames, memoryAddress, numberOfFrames, size);
                //printf("%d\n", index);
                if(index == -1) {
                    printf("Error\n");
                }
                //printf("replace %s with %s\n", frames[index].address, memoryAddress[i].address);
                strcpy(frames[index].address, memoryAddress[i].address);
                frames[index].pageNumberDec = memoryAddress[i].pageNumberDec;
                pageFaults++;
                pageReplacement++;
                break;
            } 
        }
    }

    printf("Page Hits: %d\nPage Faults: %d\nPage Replacements: %d\n", pageHits, pageFaults, pageReplacement);
}
