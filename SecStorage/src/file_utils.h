#ifndef ACCESSDAEMON_FILE_UTILS_H
#define ACCESSDAEMON_FILE_UTILS_H

bool dir_exist(char* path);

bool file_exist(char* path);

int open_file(const char* file_path,const char* modes);

char** get_file_lines(const char* path, int* size);

#endif //ACCESSDAEMON_FILE_UTILS_H
