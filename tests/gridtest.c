/**
 * @file gridtest.c
 * @author TEAM PINE
 * @brief: unit tests for the grid module
 * @version 0.1
 * @date 2021-05-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/* Standard Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "grid.h"       /* grid module */



/* LOCAL CONSTANTS */
static const int SUCCESS = 0;
static const int INVALID_FILENAME = 1;
static const int INVALID_FILE = 2;
static const int INVALID_GRID = 3;
static const int INVALID_MAP = 4;
static const int INVALID_ROW = 5;

int 
main(int argc, const char* argv[])
{
  /* code */
  if (argc != 2) {
    fprintf(stderr, "Error, please provide a file name.\n");
    exit(INVALID_FILENAME);
  }

  FILE* fp;
  if ( (fp = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Error opening %s.\nPlease check directories and make sure you have the correct one.\n");
    exit(INVALID_FILE);
  }


  /* load the grid */
  grid_t* testGrid;
  if ( (testGrid = grid_init(fp)) == NULL) {
    fprintf(stderr, "Error creating grid.\n");
    fclose(fp);
    exit(INVALID_GRID);
  }

  if (grid_getRows(testGrid) != file_numLines(fp)) {
    fprintf(stderr, "Error: number of rows in grid and does not match that in original file.\n");
  }
  char* line;
  if ( (line = file_readLine(fp)) != NULL) {
    if (grid_getCols(testGrid) != strlen(line)) {
      fprintf(stderr, "Error: number of columns in grid and does not match that in original file.\n");
    }
    free(line);
  }

  /* close FILE */
  fclose(fp);

  /* print string representation of grid */
  char* stringifiedGrid;
  if ( (stringifiedGrid = grid_toString(testGrid)) != NULL) {
    printf("LOADED GRID:\n");
    print("%s\n", stringifiedGrid);
    free(stringifiedGrid);
  }
  else {
    exit(INVALID_MAP);
  }

  /* print individual rows in grid representation of map */
  char** rawGrid;
  if ( (rawGrid = grid_getGrid(testGrid)) != NULL) {
    printf("\n Printing raw representation:\n");
    int numRows = grid_getRows(testGrid);
    int numCols = grid_getColumns(testGrid);

    if (numRows == 0 || numCols == 0) {
      fprintf(stderr, "Detected ZERO rows/columns. Either the map file is erratic, or an error occurred in parsing the map.\n");
      free(rawGrid);
      grid_delete(testGrid);
      exit(INVALID_MAP);
    }

    for (int row = 0; row < numRows; row++) {
      if (rawGrid[row] != NULL) {
        printf("column %3d: %s\n", row, rawGrid[row]);
      }
      else {
        fprintf(stderr, "Error: NULL assignment detected inside grid at row %3d", row);
        grid_delete(testGrid);
        exit(INVALID_ROW);
      }
    }
  }

  /* if all tests are passed, return 0 */ 
  return SUCCESS;
}
