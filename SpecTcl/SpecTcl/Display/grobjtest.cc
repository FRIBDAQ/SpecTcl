/*
** Test for graphical object I/O
**
*/

#include <stdio.h>
#include <stdlib.h>

#include "dispgrob.h"

grobj_database database;

int main(int argc, char **argv)
{
  FILE *fp;

  /* argv[1] -> input file,
  ** argv[2] -> output file.
  */

  if(argc != 3) {
    fprintf(stderr, "Supply input and output file please\n");
    exit(-1);
  }
  fp = fopen(argv[1], "r");
  if(fp == NULL) {
    perror("Failure opening input file\n");
    exit(-1);
  }

  /* Read the database */

  if(database.read(fp) == EOF) {
    perror("Failed to read database\n");
    exit(-1);
  }
  fprintf(stderr, "Success reading db\n");
  fclose(fp);

  /* Write the database: */

  fp = fopen(argv[2], "w");
  if(fp == NULL) {
    perror("Failure opening output file\n");
    exit(-1);
  }
  if(database.write(fp) == EOF) {
    perror("Failed to write database\n");
    exit(-1);
  }
  fprintf(stderr, "Successfully wrote database\n");
  fclose(fp);

  exit(-1);
}
  
