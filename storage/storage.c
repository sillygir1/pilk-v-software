#include "storage.h"

#define MAX_PATH_LEN 255

static char *get_full_path(char *path, char *filename) {
  char *fullpath = malloc(sizeof(char) * MAX_PATH_LEN);
  snprintf(fullpath, MAX_PATH_LEN, "%s/%s", path, filename);
  return fullpath;
}

int storage_file_read(char *path, char *filename, char *buff, int buff_size) {
  char *fullpath;
  if (filename)
    fullpath = get_full_path(path, filename);
  else
    fullpath = path;
  FILE *file = fopen(fullpath, "r");
  if (!file) {
    printf("File opening error!\n");
    return 1;
  }

  fseek(file, 0L, SEEK_END);
  int len = ftell(file);
  fseek(file, 0L, SEEK_SET);
  if (len > buff_size)
    len = buff_size;

  fread(buff, 1, len, file);

  fclose(file);

  if (filename)
    free(fullpath);

  return 0;
}

int storage_file_write(char *path, char *filename, char *string) {
  char cmd[MAX_PATH_LEN + 10];
  memset(cmd, 0, MAX_PATH_LEN + 10);
  snprintf(cmd, MAX_PATH_LEN + 10, "mkdir -p %s", path);

  char *fullpath = get_full_path(path, filename);

  FILE *file = fopen(fullpath, "w");
  if (!file) {
    printf("File opening error!\n");
    return 1;
  }

  fwrite(string, 1, strlen(string), file);

  fclose(file);
  free(fullpath);
}

int storage_dir_list(char *path, char **arr, int arr_len) {
  DIR *d;
  int n = 0;
  struct dirent *dir;
  d = opendir(path);
  if (!d) {
    printf("Directory opening error.\n");
    return -1;
  }
  while ((dir = readdir(d)) != NULL) {
    if (strcmp(dir->d_name, "..") == 0 || strcmp(dir->d_name, ".") == 0)
      continue;
    if (n >= arr_len) {
      printf("File array max size reached.\n");
      break;
    }
    arr[n] = malloc(sizeof(char) * strlen(dir->d_name));
    if (!arr[n])
      printf("String allocation error\n");
    // Unsafe, so what? It's open source anyway
    strcpy(arr[n], dir->d_name);
    n++;
  }
  closedir(d);

  return n;
}

#ifndef PLUGIN
#define DIR_LIST_LEN 64

int main() {
  char *buff = malloc(sizeof(char) * 32);

  system("echo \"test test test\ntest\" > test.txt");
  memset(buff, 0, 32);
  storage_file_read("test.txt", NULL, buff, 32);
  printf("%s\n", buff);

  system("rm test.txt");

  storage_file_write(".", "test.txt", buff);
  system("cat test.txt");

  printf("\n");
  char *arr[DIR_LIST_LEN];

  int n = storage_dir_list(".", arr, DIR_LIST_LEN);
  printf("%d files:\n", n);
  if (n <= 0)
    printf("No files\n");

  for (int i = 0; i < n; i++) {
    printf("%d %s\n", i, arr[i]);
    free(arr[i]);
  }
  free(buff);
}

#endif