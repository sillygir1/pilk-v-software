#include <dirent.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @brief Read data from file
/// @param path current directory path/full path
/// @param filename file name (if path is not full)
/// @param buff data buffer
/// @param buff_size data buffer size
/// @return
int storage_file_read(char *path, char *filename, char *buff, int buff_size);

/// @brief Write data to file
/// @param path current directory path/full path
/// @param filename file name (if path is not full)
/// @param string data to write
/// @return 0 if successful
int storage_file_write(char *path, char *filename, char *string);

/// @brief Returns true if path points to directory
/// @param path path to directory
/// @param name name of object in question
/// @return 1 if directory
bool storage_is_dir(char *path, char *name);

/// @brief Get contents of current directory
/// @param path current directory path
/// @param arr directory list array
/// @param arr_len directory list array max length
/// @param skip_dirs skip directories in output
/// @return length of list returned
int storage_dir_list(char *path, char **arr, int arr_len, bool skip_dirs);

/// @brief Go one directory higher
/// @param path current directory path
void storage_dir_up(char *path);

/// @brief Go one directory lower
/// @param path current directory path
/// @param dir directory name
void storage_dir_down(char *path, char *dir);

/// @brief Get file extension from file name
/// @param filename name of the file
/// @return pointer to first extension symbol
char *storage_get_ext(char *filename);
