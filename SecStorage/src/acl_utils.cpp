#include "acl_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../SecStorageLib/include/common.h"
#include "file_utils.h"

bool delete_line_from_acl(char* acl_file, int line_num) {
    int size =0;

    char** lines = get_file_lines(acl_file, &size);

    if ( line_num > size )
        return false;

    FILE* fp = fopen(acl_file, "w");

    for (int i=0; i < size; ++i) {
        if (i == line_num)
            continue;

        fprintf(fp, "%s", lines[i]);
    }

    for (int i = 0; i < size; ++i){
        free(lines[i]);
    }
    free(lines);
    fclose(fp);
    return true;
}

void add_line_to_acl(char* acl_file, char* line) {
    FILE* fp = fopen(acl_file, "a");
    fprintf(fp, "%s", line);
    fclose(fp);
}

char* generate_acl_line(int uid, char* file, rights_t rights) {
    char * line = (char*) calloc(256,1);
    sprintf(line, "%d %s %d\n", uid, file, rights);
    return line;
}

bool set_permissions(int target_uid, int current_uid, char *path, rights_t right) { //char* required_modes) {
    bool result = false;

    int size = 0 ;

    char acl_file[256] = {0};
    sprintf(acl_file, "%s/%d", ACL_FILE_PATH, current_uid);
    char** lines = get_file_lines(acl_file, &size);

    int user_line_in_acl = -1;
    for (int i = 0; i < size; ++i){
        char* line_token = strtok(lines[i], " ");

        if (atoi(line_token) == target_uid &&
            strcmp(strtok(NULL, " "), path) == 0){
            user_line_in_acl =i;
            break;
        }
    }

    if (user_line_in_acl != -1) {
        delete_line_from_acl(acl_file, user_line_in_acl);
    }

    char * line = generate_acl_line(target_uid, path, right);
    add_line_to_acl(acl_file, line);

    for (int i = 0; i < size; ++i){
        free(lines[i]);
    }
    free(lines);
    free(line);
    return result;
}

bool check_permissions(int target_uid, int current_uid, char *path, rights_t right) {
    bool result = false;

    if (target_uid == current_uid)
        return true;

    int size = 0 ;

    char buf[256] = {0};
    sprintf(buf, "%s/%d", ACL_FILE_PATH, target_uid);
    char** lines = get_file_lines(buf, &size);

    for (int i = 0; i < size; ++i){
        char* line_token = strtok(lines[i], " ");

        if (atoi(line_token) == current_uid &&
            strcmp(strtok(NULL, " "), path) == 0 &&
            right == rights_t(atoi(strtok(NULL, " ")))){
            result = true;
            break;
        }
    }

    for (int i = 0; i < size; ++i){
        free(lines[i]);
    }
    free(lines);
    return result;
}
