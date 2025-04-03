#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/types.h>

#define MAX_PATH 4096


typedef enum Status {
    OK,
    INPUT_ERROR,
    MEMORY_ERROR,
    DIR_NOT_FOUND
}status;

status print_file_permissions(const char *path);

int main (int argc, char *argv[]) {
    if (argc < 2) {
        printf ("Input Error\n");
        return INPUT_ERROR;
    }

    struct dirent *pDirent;
    DIR *pDir;
    
    for(int i = 1; i < argc; i++) {
        pDir = opendir(argv[i]);
        if (!pDir) {
            printf ("Cannot open directory %s\n", argv[i]);
            continue;
        }

        printf("Directory: %s:\n", argv[i]);

        while ((pDirent = readdir(pDir)) != NULL) {
            if (strcmp(pDirent->d_name, ".") && strcmp(pDirent->d_name, "..")) {
                printf ("\tFile name: %s, Inode number: %lu, ", pDirent->d_name, pDirent->d_ino);

                char file_path[MAX_PATH];

                snprintf(file_path, MAX_PATH, "%s/%s", argv[i], pDirent->d_name);

                if(print_file_permissions(file_path) != OK) {
                    closedir(pDir);

                    return INPUT_ERROR;
                }
            }
        }
        closedir(pDir);
    }
    return 0;
}

status print_file_permissions(const char *path) {
    if(!path) return INPUT_ERROR;
    struct stat st;
    if (stat(path, &st) != 0) {
        return INPUT_ERROR;
    }

    mode_t mode = st.st_mode;
    char permissions[11] = "----------";

    if (S_ISDIR(mode))        permissions[0] = 'd';
    else if (S_ISCHR(mode))   permissions[0] = 'c';
    else if (S_ISBLK(mode))   permissions[0] = 'b';
    else if (S_ISFIFO(mode))  permissions[0] = 'p';
    else if (S_ISLNK(mode))   permissions[0] = 'l';
    else if (S_ISSOCK(mode))  permissions[0] = 's';

    permissions[1] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (mode & S_IXUSR) ? 
                     ((mode & S_ISUID) ? 's' : 'x') : 
                     ((mode & S_ISUID) ? 'S' : '-');

    permissions[4] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (mode & S_IXGRP) ? 
                     ((mode & S_ISGID) ? 's' : 'x') : 
                     ((mode & S_ISGID) ? 'S' : '-');

    permissions[7] = (mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (mode & S_IXOTH) ? 
                     ((mode & S_ISVTX) ? 't' : 'x') : 
                     ((mode & S_ISVTX) ? 'T' : '-');

    printf("%s\n", permissions);
    return OK;
}
