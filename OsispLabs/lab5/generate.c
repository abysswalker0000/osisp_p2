#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void create_data_file(const char *path, int count) {
  FILE *file = fopen(path, "w");
  if (!file) {
    perror("Failed to open file for writing");
    exit(EXIT_FAILURE);
  }
  fprintf(file, "%d\n", count);
  for (int i = 0; i < count; ++i) {
    fprintf(file, "%d ", rand() % 1000000);
  }
  fclose(file);
  printf("Generated file '%s' with %d numbers.\n", path, count);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <number of elements>\n", argv[0]);
    return 1;
  }
  const char *path = "data.txt";
  int count = atoi(argv[1]);
  srand(time(NULL));
  create_data_file(path, count);
  return 0;
}