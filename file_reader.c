#include <stdio.h>
#include <stdlib.h>

typedef struct {
  char* text;
  size_t size;
} File;

File read_entire_file(const char* path) {
  FILE* file_descriptor = fopen(path, "r");
  File file = {"", 0};

  if(file_descriptor == NULL) {
    printf("File %s not found.\n", path);
    return file;
  }

  fseek(file_descriptor, 0, SEEK_END);
  size_t size = ftell(file_descriptor);
  fseek(file_descriptor, 0, SEEK_SET);

  char* string = calloc(sizeof(char), size + 1);
  fread(string, size, 1, file_descriptor);
  fclose(file_descriptor);

  string[size] = '\0';
  
  file.text = string;
  file.size = size;
  return file;
}