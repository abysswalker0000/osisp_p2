#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define THREAD_COUNT 20

int compare(const void *a, const void *b) {
  return (*(int *)a - *(int *)b);
}

int *load_data(const char *path, int *size) {
  FILE *file = fopen(path, "r");
  if (!file) {
    perror("Failed to open input file");
    exit(EXIT_FAILURE);
  }
  int n;
  if (fscanf(file, "%d", &n) != 1) {
    fprintf(stderr, "Failed to read size\n");
    exit(EXIT_FAILURE);
  }
  int *data = malloc(n * sizeof(int));
  if (!data) {
    perror("Memory allocation failed");
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < n; ++i) {
    if (fscanf(file, "%d", &data[i]) != 1) {
      fprintf(stderr, "Failed to read element %d\n", i);
      exit(EXIT_FAILURE);
    }
  }
  fclose(file);
  *size = n;
  return data;
}

void save_data(const char *path, int *data, int size) {
  FILE *file = fopen(path, "w");
  if (!file) {
    perror("Failed to open output file");
    exit(EXIT_FAILURE);
  }
  fprintf(file, "%d\n", size);
  for (int i = 0; i < size; ++i) {
    fprintf(file, "%d ", data[i]);
  }
  fclose(file);
}

typedef struct {
  int *data;
  int start;
  int end;
} SortTask;

void *sort_segment(void *arg) {
  SortTask *task = (SortTask *)arg;
  qsort(task->data + task->start, task->end - task->start, sizeof(int), compare);
  pthread_exit(NULL);
}

void combine(int *data, int start, int mid, int end, int *temp) {
  int i = start, j = mid, k = start;
  while (i < mid && j < end) {
    temp[k++] = (data[i] <= data[j]) ? data[i++] : data[j++];
  }
  while (i < mid) {
    temp[k++] = data[i++];
  }
  while (j < end) {
    temp[k++] = data[j++];
  }
  for (i = start; i < end; ++i) {
    data[i] = temp[i];
  }
}

typedef struct {
  int start;
  int end;
} Range;

int main() {
  const char *input_path = "data.txt";
  int size;
  int *input_data = load_data(input_path, &size);
  printf("Loaded %d elements from %s\n", size, input_path);

  int *multi_thread_data = malloc(size * sizeof(int));
  int *single_thread_data = malloc(size * sizeof(int));
  if (!multi_thread_data || !single_thread_data) {
    perror("Memory allocation failed");
    exit(EXIT_FAILURE);
  }
  memcpy(multi_thread_data, input_data, size * sizeof(int));
  memcpy(single_thread_data, input_data, size * sizeof(int));
  free(input_data);

  struct timeval begin, finish;
  double time_multi, time_single;

  pthread_t threads[THREAD_COUNT];
  SortTask tasks[THREAD_COUNT];
  int chunk = size / THREAD_COUNT;
  for (int i = 0; i < THREAD_COUNT; ++i) {
    tasks[i].data = multi_thread_data;
    tasks[i].start = i * chunk;
    tasks[i].end = (i == THREAD_COUNT - 1) ? size : (i + 1) * chunk;
  }
  gettimeofday(&begin, NULL);
  for (int i = 0; i < THREAD_COUNT; ++i) {
    if (pthread_create(&threads[i], NULL, sort_segment, &tasks[i]) != 0) {
      perror("Thread creation failed");
      exit(EXIT_FAILURE);
    }
  }
  for (int i = 0; i < THREAD_COUNT; ++i) {
    pthread_join(threads[i], NULL);
  }
  int *temp = malloc(size * sizeof(int));
  if (!temp) {
    perror("Auxiliary array allocation failed");
    exit(EXIT_FAILURE);
  }
  Range ranges[THREAD_COUNT];
  for (int i = 0; i < THREAD_COUNT; ++i) {
    ranges[i].start = tasks[i].start;
    ranges[i].end = tasks[i].end;
  }
  int range_count = THREAD_COUNT;
  while (range_count > 1) {
    int new_count = (range_count + 1) / 2;
    for (int i = 0; i < range_count / 2; ++i) {
      int start = ranges[2 * i].start;
      int mid = ranges[2 * i].end;
      int end = ranges[2 * i + 1].end;
      combine(multi_thread_data, start, mid, end, temp);
      ranges[i].start = start;
      ranges[i].end = end;
    }
    if (range_count % 2 == 1) {
      ranges[new_count - 1] = ranges[range_count - 1];
    }
    range_count = new_count;
  }
  gettimeofday(&finish, NULL);
  time_multi = (finish.tv_sec - begin.tv_sec) * 1000.0 +
               (finish.tv_usec - begin.tv_usec) / 1000.0;
  printf("Multithreaded sort time: %.3f ms\n", time_multi);

  gettimeofday(&begin, NULL);
  qsort(single_thread_data, size, sizeof(int), compare);
  gettimeofday(&finish, NULL);
  time_single = (finish.tv_sec - begin.tv_sec) * 1000.0 +
                (finish.tv_usec - begin.tv_usec) / 1000.0;
  printf("Single-threaded sort time: %.3f ms\n", time_single);

  save_data("sorted2.txt", multi_thread_data, size);
  save_data("sorted.txt", single_thread_data, size);
  printf("Sorted data saved to 'sorted2.txt' (multithreaded) and 'sorted.txt' (single-threaded).\n");

  free(multi_thread_data);
  free(single_thread_data);
  free(temp);
  return 0;
}