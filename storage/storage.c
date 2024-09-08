#include "storage.h"

#define MAX_PATH_LEN 255

// Need to free return value after usage
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
  char *fullpath;
  if (filename)
    fullpath = get_full_path(path, filename);
  else
    fullpath = path;
  FILE *file = fopen(fullpath, "w");
  if (!file) {
    printf("File opening error!\n");
    return 1;
  }

  fwrite(string, 1, strlen(string), file);

  fclose(file);
  if (filename)
    free(fullpath);
}

static void sort_dirs(char **arr, int length) {
  if (length == 0)
    return;

  bool swapped;
  for (int i = 0; i < length - 1; i++) {
    swapped = 0;
    for (int j = 0; j < length - i - 1; j++) {
      if (strcmp(arr[j], arr[j + 1]) > 0) {
        arr[j] = (char *)((uintptr_t)arr[j] ^ (uintptr_t)arr[j + 1]);
        arr[j + 1] = (char *)((uintptr_t)arr[j] ^ (uintptr_t)arr[j + 1]);
        arr[j] = (char *)((uintptr_t)arr[j] ^ (uintptr_t)arr[j + 1]);
        swapped = 1;
      }
    }
    if (!swapped)
      break;
  }
}

bool storage_is_dir(char *path, char *name) {
  DIR *d;
  struct dirent *dir;
  char *fullpath;
  if (name)
    fullpath = get_full_path(path, name);
  else
    fullpath = path;
  d = opendir(fullpath);
  if (!d) {
    return 0;
  }
  closedir(d);
  if (name)
    free(fullpath);
  return 1;
}

int storage_create_dir(char *path, char *name) {

  if (storage_is_dir(path, name))
    return 1;

  DIR *d;
  struct dirent *dir;
  char *fullpath;
  if (name)
    fullpath = get_full_path(path, name);
  else
    fullpath = path;

  char cmd[MAX_PATH_LEN];
  snprintf(cmd, MAX_PATH_LEN, "mkdir -p %s", fullpath);
  system(cmd);

  d = opendir(fullpath);
  if (!d) {
    return 1;
  }
  closedir(d);
  if (name)
    free(fullpath);
  return 0;
}

int storage_dir_list(char *path, char **arr, int arr_len, bool skip_dirs) {
  DIR *d;
  int n = 0;
  struct dirent *dir;
  d = opendir(path);
  if (!d) {
    printf("Directory opening error.\n");
    return -1;
  }
  while ((dir = readdir(d)) != NULL) {
    if (skip_dirs && storage_is_dir(path, dir->d_name) ||
        strcmp(dir->d_name, ".") == 0) {
      continue;
    }
    if (n >= arr_len) {
      printf("File array max size reached.\n");
      break;
    }
    int len = sizeof(char) * (strlen(dir->d_name) + 1);
    if (len < 13)
      len = 13;
    arr[n] = malloc(len);
    if (!arr[n])
      printf("String allocation error\n");
    // Unsafe, so what? It's open source anyway
    strcpy(arr[n], dir->d_name);
    n++;
  }
  closedir(d);

  sort_dirs(arr, n);

  return n;
}

void storage_dir_up(char *path) {
  for (uint8_t i = 0; i < 2; i++) {
    *strrchr(path, '/') = '\0';
  }
  strcat(path, "/");
}

void storage_dir_down(char *path, char *dir) {
  printf("%s\n", path);
  printf("%s\n", dir);
  strcat(path, dir);
  if (path[strlen(path) - 1] != '/')
    strcat(path, "/");
}

char *storage_get_ext(char *filename) { return strrchr(filename, '.'); }

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

  int n = storage_dir_list(".", arr, DIR_LIST_LEN, false);
  printf("%d files:\n", n);
  if (n <= 0)
    printf("No files\n");

  for (int i = 0; i < n; i++) {
    printf("%d %s\n", i, arr[i]);
    free(arr[i]);
  }

  snprintf(buff, 32, "/stop/here/not_here/");
  storage_dir_up(buff);
  printf("%s\n", buff);
  storage_dir_down(buff, "now");
  printf("%s\n", buff);

  snprintf(buff, 32, "test.ext");
  char *ext = storage_get_ext(buff);
  printf("%s\n", ext);

  snprintf(buff, 32, "test_dir");
  storage_create_dir(buff, NULL);
  system("ls -l");
  system("rm -r test_dir");

  free(buff);
}

#endif