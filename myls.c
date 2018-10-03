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

extern void print_l(struct dirent *dir_entry); 
extern void flag_test(struct dirent *dir_entry, int flag_a, int flag_l);
extern void print_args(char *dir_arg, char *file, int flag_a, int flag_l, int flag_file); 

//main function    
int main(int argc, char *argv[]){
    //initialize flags
    int flag_l = 0; 
    int flag_a = 0; 
    int flag_file = 0; //flag if a file is passed on cmd-line
    int data_arg = 0; //flagged if dir/file args are passed on cmd-line 

    //get options 
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

    //check command line args and call print_args with appropriate parameters 
    if(argc > 1){
        for(int i = 1; i <= (argc-1); i++){ 
            if(argv[i][0] != '-'){
                struct stat argbuf;
                char *arg = argv[i]; 
                if((stat(arg, &argbuf)) == -1){
                    data_arg = 1; 
                    printf("myls: cannot access '%s': No such file or directory\n", argv[i]);
                }else{
                    data_arg = 1;
                    if(S_ISREG(argbuf.st_mode)){
                        flag_file = 1;
                        print_args(".", arg, flag_a, flag_l, flag_file); 
                    }
                    if(S_ISDIR(argbuf.st_mode)){
                        printf("%s:\n", arg); 
                        print_args(arg, "NULL", flag_a, flag_l, flag_file); 
                    }
                    flag_file = 0; //resetting file_flag each iteration through 'for' loop
                }      
            }
        }
    }
    if(data_arg == 0){
        print_args(".", "NULL", flag_a, flag_l, flag_file); //if myls is called with no dir/file args
    }
    if(flag_l == 0){
        printf("\n");
    }
}

//function to print file/directory data with ls option -l
void print_l(struct dirent *dir_entry){
    struct stat statbuf; 
    if(stat(dir_entry->d_name, &statbuf) == -1){
        perror("stat"); 
        exit(EXIT_FAILURE); 
    }
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
    printf("%s\n", dir_entry->d_name); 
}

//function to check flags and print file/directory info accordingly
void flag_test(struct dirent *dir_entry, int flag_a, int flag_l){
    if(flag_a == 0){
        if((dir_entry->d_name[0] == '.')){ 
            return; 
        } 
    }
    if(flag_l == 0){
        printf("%s ", dir_entry->d_name);
    }else{
        print_l(dir_entry);
    }
}

//function to handle cmd-line args (files and directories)
void print_args(char *dir_arg, char *file, int flag_a, int flag_l, int flag_file){
    //open directory
    DIR *dir = opendir(dir_arg);
    if(dir == NULL){
        perror("opendir"); 
        exit(EXIT_FAILURE);
    } 

    //read and print directory/file data 
    struct dirent *dir_entry; 
    rewinddir(dir); 
    errno = 0;
    while((dir_entry = readdir(dir))!= NULL){ 
        if(flag_file == 1){
            if(strcmp(dir_entry->d_name, file) == 0){ //comparing current file to cmd-line arg
                flag_test(dir_entry, flag_a, flag_l);
            }
        }else{
            flag_test(dir_entry, flag_a, flag_l); 
        }
    }
    if(errno == 1){
        perror("readdir");
        exit(EXIT_FAILURE); 
    }

    //close directory
    closedir(dir);
}
