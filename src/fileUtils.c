/*
 * Contains functionality to read, write, and create file paths for general
 * use.
 */

#include <stdio.h>
#include <stdlib.h>

// Max length for buffers
static const int MSG_MAX_LEN = 10000;

char *File_concatFilePath(const char *pFilePathBegin, const char *pFilePathEnd)
{
  char *concatFilePath = malloc(sizeof(char)*MSG_MAX_LEN);

	snprintf(concatFilePath, sizeof(char)*MSG_MAX_LEN, 
	"%s%s", pFilePathBegin, pFilePathEnd);
    
  return concatFilePath;
}

char *File_readFromFile(const char *pFilePath)
{
  FILE *pFile = fopen(pFilePath, "r");
  if(pFile == NULL) {
    printf("ERROR: Unable to open file (%s) for read.\n", pFilePath);
    exit(EXIT_FAILURE);
  }

  char *buff = malloc(sizeof(char)*MSG_MAX_LEN);
  fgets(buff, MSG_MAX_LEN,pFile);

  fclose(pFile);

  return buff;
}

void File_writeToFile(const char *pFilePath, const char *pValue) 
{
  FILE *pFile = fopen(pFilePath,"w");
  if (pFile == NULL) {
    printf("ERROR: Unable to open and write to file.\n");
    perror("Failed: ");
    exit(EXIT_FAILURE);
  }

  int charWritten = fprintf(pFile, "%s", pValue);
	if (charWritten <= 0) {
		printf("ERROR WRITING DATA.\n");
		exit(EXIT_FAILURE);
	}

  fclose(pFile);
}
