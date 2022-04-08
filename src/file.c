/*
 * Contains functionality to read, write, and create file paths for general
 * use.
 */

#include "../include/file.h"
#include <stdio.h>
#include <stdlib.h>

int File_concatFilePath(const char *_pFilePathBegin,
const char *_pFilePathEnd, char *_pConcatFilePath,
const int concatFilePathSize)
{
	int charsPrinted = snprintf(_pConcatFilePath, sizeof(char)*concatFilePathSize, 
	"%s%s", _pFilePathBegin, _pFilePathEnd);
  if(charsPrinted <=0){
    printf("ERROR: Unable to concatenate file (%s) and file (%s).\n",
     _pFilePathBegin, _pFilePathEnd);
    exit(EXIT_FAILURE);    
  }
  
  return 0;
}

int File_readFromFile(const char *_pFilePath, char *_readBuffer, 
const int _readBufferSize)
{
  FILE *pFile = fopen(_pFilePath, "r");
  if(pFile == NULL) {
    printf("ERROR: Unable to open file (%s) for read.\n", _pFilePath);
    exit(EXIT_FAILURE);
  }

  fgets(_readBuffer, _readBufferSize, pFile);
  fclose(pFile);

  return 0;
}

int File_writeToFile(const char *_pFilePath, const char *_pValue) 
{
  FILE *pFile = fopen(_pFilePath,"w");
  if (pFile == NULL) {
    printf("ERROR: Unable to open and write to file.\n");
    perror("Failed: ");
    exit(EXIT_FAILURE);
  }

  int charWritten = fprintf(pFile, "%s", _pValue);
	if (charWritten <= 0) {
		printf("ERROR WRITING DATA.\n");
		exit(EXIT_FAILURE);
	}

  fclose(pFile);

  return 0;
}
