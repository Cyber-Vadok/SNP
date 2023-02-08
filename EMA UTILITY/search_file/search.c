#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>

int counter = 0;

void search_file_by_name(char* directory, char* file) {
    DIR* dir = opendir(directory);
    struct dirent* entry;

    if (dir == NULL) {
        printf("Error: Could not open directory\n");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);
            search_file_by_name(path, file);
        } else {

            char path[1024];
            struct stat *buf; 

            snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);
            buf = malloc(sizeof(struct stat));
        	stat(path, buf);

            if (strcmp(entry->d_name, file) == 0) {
                counter++;
                printf("File: %d\n",counter);
                printf("Found file %s in directory: \n%s \n", file, directory);
                printf("Type: %d\n", entry->d_type);
                printf("Inode: %llu\n", entry->d_ino);
                printf("Record lenght: %d\n", entry->d_reclen);
                printf("Hard link: %d\n", buf->st_nlink);

                char buff[100];
                strftime(buff, sizeof buff, "%D %T", gmtime(&buf->st_mtimespec.tv_sec));
                printf("Last modification: %s\n",buff);
                strcpy(buff,"");
                strftime(buff, sizeof buff, "%D %T", gmtime(&buf->st_atimespec.tv_sec));
				printf("Last access: %s\n",buff);
                strcpy(buff,"");
				strftime(buff, sizeof buff, "%D %T", gmtime(&buf->st_ctimespec.tv_sec));
				printf("Last status change: %s\n\n",buff);

                return;
            }
        }
    }

    closedir(dir);
}

void search_file_by_extension(char* directory, char* extension) {
    DIR* dir = opendir(directory);
    struct dirent* entry;

    if (dir == NULL) {
        printf("Error: Could not open directory\n");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);
            search_file_by_extension(path, extension);
        } else {

            char path[1024];
            struct stat *buf; 

            snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);
            buf = malloc(sizeof(struct stat));
        	stat(path, buf);
            char *ext = strrchr(entry->d_name, '.');
            if (ext != NULL){
                if (strcmp(ext+1,extension) == 0) {
                counter++;
                printf("File: %d\n",counter);
                printf("Found file %s in directory: \n%s \n", entry->d_name, directory);
                printf("Type: %d\n", entry->d_type);
                printf("Inode: %llu\n", entry->d_ino);
                printf("Record lenght: %d\n", entry->d_reclen);
                printf("Hard link: %d\n", buf->st_nlink);

                char buff[100];
                strftime(buff, sizeof buff, "%D %T", gmtime(&buf->st_mtimespec.tv_sec));
                printf("Last modification: %s\n",buff);
                strcpy(buff,"");
                strftime(buff, sizeof buff, "%D %T", gmtime(&buf->st_atimespec.tv_sec));
				printf("Last access: %s\n",buff);
                strcpy(buff,"");
				strftime(buff, sizeof buff, "%D %T", gmtime(&buf->st_ctimespec.tv_sec));
				printf("Last status change: %s\n\n",buff);
                
                return;
                }
            }
        }
    }

    closedir(dir);
}


int main() {

    //search_file_by_name("/Users/emanuelemele/Desktop", "test.txt");
    //printf("Totale file trovati: %d\n",counter);
    //counter = 0;

    search_file_by_extension("/Users/emanuelemele/Desktop", "pdf");
    printf("Totale file trovati: %d\n",counter);

    return 0;
}
