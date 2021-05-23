#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "support/file.h"
#include "support/message.h"
#include "support/log.h"

typedef struct grid {
    char **g;
} grid_t;

grid_t* grid_init(File* mapfile, int rows, int cols){
    if(mapfile == NULL || rows == 0 || cols == 0){
        return NULL;
    }

    grid_t* grid  = malloc(sizeof(grid_t));
    if(grid == NULL){
        return NULL;
    }

    grid->g = malloc(rows);
    for(int i = 0;i<rows;i++){
        grid->g[i] = malloc(cols);
    }

    return grid;
}