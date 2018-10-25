//calder birdsey
//cs315
//assignment 01 

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h> 
#include <errno.h>
#include <string.h> 
#include <sys/stat.h>
#include <pwd.h>
#include <time.h>
#include <libgen.h>
#include <sys/types.h>
#include <grp.h>

void print_long(char *dir_arg, struct dirent *dir_entry); 
void flag_handler(char *dir_arg, struct dirent *dir_entry, int flag_all, int flag_long);
void print_args(char *dir_arg, char *file, int flag_all, int flag_long, int flag_file); 

//main function    
int main(int argc, char *argv[]) {
    //initialize flags
    int flag_long = 0; 
    int flag_all = 0; 
    int flag_file = 0; 

    //get options 
    int opt; 
    while((opt = getopt(argc, argv, "al")) != -1) {
        switch (opt) {
        case 'l':   
            flag_long = 1; 
            break;
        case 'a':
            flag_all = 1; 
            break;
        default:
            fprintf(stderr, "myls: supports -l and -a options\n"); 
            exit(EXIT_FAILURE); 
        }
    }

    //check command line args and call print_args with appropriate parameters 
    if(optind == argc) {
        print_args(".", "NULL", flag_all, flag_long, flag_file);
        if(flag_long == 0) {
            printf("\n");
        }
    }else{
        while(optind < argc) {
            struct stat argbuf;
            char *arg = argv[optind]; 
            if((stat(arg, &argbuf)) == -1) {
                printf("myls: cannot access '%s': No such file or directory\n", argv[optind]);
            }else{
                if(S_ISREG(argbuf.st_mode)) {
                    flag_file = 1;
                    print_args(".", arg, flag_all, flag_long, flag_file); 
                }
                if(S_ISDIR(argbuf.st_mode)) {
                    printf("%s:\n", arg); 
                    print_args(arg, "NULL", flag_all, flag_long, flag_file); 
                }
                flag_file = 0;
                if(optind < argc-1) {
                    printf("\n"); 
                }
                if(flag_long == 0) {
                    printf("\n");
                }
            }
            optind ++; 
        }    
    }
}

//function to print file/directory data with ls option -l
void print_long(char *dir_arg, struct dirent *dir_entry) {
    struct stat statbuf; 
    char fp[PATH_MAX];
    sprintf(fp, "%s/%s", dir_arg, dir_entry->d_name);
    if(stat(fp, &statbuf) == -1) {
        perror("stat");
        return;   
    }

    //permission data/nlink 
    printf((S_ISDIR(statbuf.st_mode)) ? "d" : "-"); 
    printf((statbuf.st_mode & S_IRUSR) ? "r" : "-");
    printf((statbuf.st_mode & S_IWUSR) ? "w" : "-");
    printf((statbuf.st_mode & S_IXUSR) ? "x" : "-");
    printf((statbuf.st_mode & S_IRGRP) ? "r" : "-");
    printf((statbuf.st_mode & S_IWGRP) ? "w" : "-");
    printf((statbuf.st_mode & S_IXGRP) ? "x" : "-");
    printf((statbuf.st_mode & S_IROTH) ? "r" : "-");
    printf((statbuf.st_mode & S_IWOTH) ? "w" : "-");
    printf((statbuf.st_mode & S_IXOTH) ? "x " : "- ");
    printf("%li ", statbuf.st_nlink);

    //group and user data 
    struct passwd *pw; 
    struct group *gid; 
    pw = getpwuid(statbuf.st_uid);  
    if(pw == NULL) {
        perror("getpwuid"); 
        printf("%d ", statbuf.st_uid); 
    }else {
        printf("%s ", pw->pw_name); 
    }
    gid = getgrgid(statbuf.st_gid);
    if(gid == NULL) {
        perror("getpwuid"); 
        printf("%d ", statbuf.st_gid); 
    }else  {
        printf("%s ", gid->gr_name); 
    }

    //file size
    printf("%5ld ", statbuf.st_size);

    //timestamp
    struct tm *tmp;
    char outstr[200];
    time_t t = statbuf.st_mtime;
    tmp = localtime(&t);   
    if(tmp == NULL) {
        perror("localtime"); 
        exit(EXIT_FAILURE);
    } 
    strftime(outstr, sizeof(outstr), "%b %d %R", tmp); 
    printf("%s ", outstr);

    //file name 
    printf("%s\n", dir_entry->d_name); 
}

//function to check flags and print file/directory info accordingly
void flag_handler(char *dir_arg, struct dirent *dir_entry, int flag_all, int flag_long) {
    if(flag_all == 0){
        if((dir_entry->d_name[0] == '.')) { 
            return; 
        } 
    }
    if(flag_long == 0) {
        printf("%s ", dir_entry->d_name);
    }else { 
        print_long(dir_arg, dir_entry);
    }
}

//function to handle cmd-line args
void print_args(char *dir_arg, char *file, int flag_all, int flag_long, int flag_file) {
    //open directory
    DIR *dir = opendir(dir_arg);
    if(dir == NULL) {
        perror("opendir"); 
        exit(EXIT_FAILURE);
    } 

    //read and print directory/file data 
    struct dirent *dir_entry;  
    errno = 0; 
    while((dir_entry = readdir(dir))!= NULL) { 
        if(flag_file == 1) {
            if(strcmp(dir_entry->d_name, file) == 0) {
                flag_handler(dir_arg, dir_entry, flag_all, flag_long);
            }
        }else {
            flag_handler(dir_arg, dir_entry, flag_all, flag_long); 
        }
    }
    if((dir_entry == NULL) && (errno != 0)) {
        perror("readdir");
        exit(EXIT_FAILURE); 
    }
    
    //close directory
    closedir(dir);
}