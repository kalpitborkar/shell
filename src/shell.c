

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);


char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
  &shell_cd,
  &shell_help,
  &shell_exit
};

int shell_num_builtins(){
    return sizeof(builtin_str)/sizeof(char *);
}

int shell_cd(char **args){
    if(args[1] == NULL){
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    }
    else{
        if(chdir(args[1]) != 0){
            perror("shell");
        }
    }
    return 1;
}

int shell_help(char **args){
    int i;
    printf("The following are built in:\n");
    for(i = 0; i < shell_num_builtins(); i++){
        printf(" %s\n", builtin_str[i]);
    }
    return 1;
}

int shell_exit(char **args){
    return 0;
}

int shell_launch(char **args){
    pid_t pid;
    int status;
    pid = fork();
    if(pid == 0){
        if(execvp(args[0], args) == -1){
            perror("shell");
        }
        exit(EXIT_FAILURE);
    }
    else if(pid < 0){
        perror("shell");
    }
    else{
        do{
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int shell_execute(char **args){
    int i;
    if(args[0] == NULL){
        return 1;
    }

    for (i = 0; i < lsh_num_builtins(); i++){
        if (strcmp(args[0], builtin_str[i]) == 0){
            return (*builtin_func[i])(args);
        }
    }
    return lsh_launch(args);
}

#define shell_RL_BUFSIZE 1024

char *shell_read_line(void){
    int bufsize = shell_RL_BUFSIZE;
    int position = 0;
    char *buffer = malloc(sizeof(*char) * bufsize);
    int c;

    if(!buffer){
        fprintf(stderr, "\n\nshell: %s\n\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    while(1){
        c = getchar();
        if(c == EOF || c == '\n'){
            buffer[position] = '\0';
            return buffer;
        }
        else{
            buffer[position] = c;
        }
        position++;
        if(position > bufsize){
            bufsize += shell_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if(!buffer){
                fprintf(stderr, "\n\nshell: %s\n\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
    }
}