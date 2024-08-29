#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int storage_file_read(char *path, char *filename, char *buff, int buff_size);

int storage_file_write(char *path, char *filename, char *string);

int storage_dir_list(char *path, char **arr, int arr_len);
