//calder birdsey 
//cs315
//assignment 01 
//9/20/18 

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

//function to print file/directory data with ls option -1 
void print_l(struct dirent *dir_entry){
    struct stat statbuf; 
    if(stat(dir_entry->d_name, &statbuf) == -1){
        perror("stat"); 
        exit(4); 
    }else{
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
        struct passwd *pw; 
        pw = getpwuid(statbuf.st_uid);  
        printf("%s ", pw->pw_name); 
        pw = getpwuid(statbuf.st_gid);
        printf("%s ", pw->pw_name); 
        printf("%5ld ", statbuf.st_size);
        struct tm *tmp;
        char outstr[200];
        time_t t = statbuf.st_mtime;
        tmp = localtime(&t);   
        if(tmp == NULL){
            perror("localtime"); 
            exit(EXIT_FAILURE);
        } 
        strftime(outstr, sizeof(outstr), "%b %d %R", tmp); 
        printf("%s ", outstr);
        printf("%s\n", dir_entry-> d_name); 
    }
}

//function to check flags and print file/directory info accordingly
void flag_test(struct dirent *dir_entry, int flag_a, int flag_l){
    if(flag_a == 0){
        if((dir_entry->d_name[0] != '.')){ 
            if(flag_l == 0){
                printf("%s ", dir_entry->d_name);
            }else{
                print_l(dir_entry);  
            }
        }
    }else{
        if(flag_l == 0){
            printf("%s ", dir_entry->d_name);
        }else{
            print_l(dir_entry);  
        }
    }
}

//fucntion to check if myls argument is an existing file or directory in cwd 
void dir_check(int argc, char *argv[]){
    for(int i = 1; i <= (argc-1); i++){
        if(argv[i][0] != '-'){
            struct stat argbuf;
            char * arg_file = argv[i]; 
            if((stat(arg_file, &argbuf)) == -1){
                printf("myls: cannot access '%s'; No such file or directory\n",
                argv[i]);  
            }else{
                if(S_ISDIR(argbuf.st_mode)){
                    chdir(argv[i]); 
                }else{
                    printf("file\n");//unsure how to do equivalent of chdir() for a file 
                } 
            }             
        }
    }
}

//main function    
int main(int argc, char *argv[]){
    //check command line args 
    if(argc > 4){
        printf("usage: %s source dest\n", argv[0]); 
        exit(EXIT_FAILURE); 
    }
    if(argc > 1){
        dir_check(argc, argv); 
    }
    //set flags
    int flag_a, flag_l = 0;
    int opt; 
    while((opt = getopt(argc, argv, "al")) != -1) {
        switch (opt) {
        case 'l':   
            flag_l = 1; 
            break;
        case 'a':
            flag_a = 1; 
            break;
        default:
            fprintf(stderr, "usage: %s source dest\n", argv[0]); 
            exit(EXIT_FAILURE); 
        }
    }
    //open directory
    char cwd[PATH_MAX]; 
    char *cdir = getcwd(cwd, sizeof(cwd)); 
    DIR *dir = opendir(cdir);
    if(dir == NULL){
        perror("opendir"); 
        exit(EXIT_FAILURE);
    } 
    //read directory
    struct dirent *dir_entry; 
    rewinddir(dir); 
    errno = 0;
    while((dir_entry = readdir(dir))!= NULL){
        flag_test(dir_entry, flag_a, flag_l); 
    }
    printf("\n"); 
    if(errno == 1){
        perror("readdir");
        exit(EXIT_FAILURE); 
    }
    //close directory
    closedir(dir);
}
