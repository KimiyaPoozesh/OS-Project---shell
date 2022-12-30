#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/wait.h>
#include <signal.h>
#define MAX_WORDS   1000
#define MAX_LINE    70 /*70 character max per line*/
int histItems = 0;
int no_reprint_prmpt;
void shellPrompt(){
	// We print the prompt
    //getcwd return the name of the currentDirectory
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("\n%s>>", cwd);

}

int execute(char *command){
    char *args[MAX_LINE/2 + 1];
	char *words;
    int i,val;
    int len = strlen(command);
    if(command[len-1] =='\n')
        command[len-1] =0;
    // split the read input in words for the sake of args
	words = strtok(command, " ");
	i = 0;
	while(words != NULL)
	{
		if (strcmp(words, "&"))
			args[i] = words;
		i++;
		words = strtok(NULL, " ");
	}
    // we need to add null at the end of args so that we can use execvp
	args[i] = NULL;
    if (strcmp(command, "exit")==0){
        exit(0);
    }

    else if (strcmp(command, "help")==0)
	{
		printf("\na : Splits the first word of a line.\n"
        "b : prints the most repeated word in the file.\n"
        "c : Deletes all spaces and prints the file\n"
         "f : Shows the number of lines\n"
         "g : Shows ten first line in a file\n"
         "d : Shows all lines that are not comment. comments represented with #");
		fflush(stdin);
		fflush(stdout);
		return 1;
	}
    else if(command[0]=='c' && command[1]=='d'){
        val= chdir(args[1]);
        if (val == -1){
            printf("Error");
        }
        return 1;
    }
    else if (strcmp(command, "a")==0){
         a();
        return 1;
    }
    else if (strcmp(command, "b")==0){
        b();
        return 1;
    }
    else if (strcmp(command, "c")==0){
        c();
        return 1;
    }
    else if (strcmp(command, "f")==0){
        f();
        return 1;
    }
    else if (strcmp(command, "g")==0){
        g();
        return 1;
    }
    else if (strcmp(command, "d")==0){
        d();
        return 1;
    }
    else if (!strcmp(command, "history")) { printHistory(); return 1; }
    //most common command
    val = execArgs(args);
    return val;
}

int execArgs(char** parsed)
{
    // Forking a child
    pid_t pid = fork();

    if (pid == -1) {
        fprintf(stderr, "\nFailed forking child..");
        return;
    } else if (pid == 0) {
        if (execvp(parsed[0], parsed) < 0) {

            fprintf(stderr, "\nCould not execute command..");
            return 0;
        }
        exit(0);
    } else {
        // waiting for child to terminate
        wait(NULL);
        return 1;
    }
}


int c(){
     int echo, chr;
    FILE* fp;
    char word[30];
    fp = fopen ("/home/kimiya/Desktop/os/test/mostrepeat.txt", "r");

    chr = fgetc (fp);
    while (chr != EOF) {
        echo = 1;
        if (isblank (chr)|| chr=='\n' || chr =='\t') echo = 0;
        if (echo ) putchar (chr);
        chr = fgetc (fp);
    }
}
int a() {
    int echo, chr;
    FILE* fp;
    char word[30];
    fp = fopen ("/home/kimiya/Desktop/os/test/mostrepeat.txt", "r");
    echo = 1;
    chr = fgetc (fp);
    while (chr != EOF) {
        if (chr == '\n') echo = 1;
        if (isblank (chr)) echo = 0;
        if (echo ) putchar (chr);
        chr = fgetc (fp);
    }
    fclose (fp);
    return 0;
}
int b() {
    char path[100];
    int i, len, index, isUnique;
    char words[MAX_WORDS][50];
    char word[50];
    int  count[MAX_WORDS];
    FILE* fptr;

    fptr = fopen("/home/kimiya/Desktop/os/test/mostrepeat.txt", "r");
    if (fptr == NULL)
    {
        printf("Unable to open file.\n");
        printf("Please check you have read previleges.\n");
        exit(EXIT_FAILURE);
    }
    for (i=0; i<MAX_WORDS; i++)
        count[i] = 0;
    index = 0;

    while (fscanf(fptr, "%s", word) != EOF)
    {

        len = strlen(word);
        if (ispunct(word[len - 1]))
            word[len - 1] = '\0';
        isUnique = 1;
        for (i=0; i<index && isUnique; i++)
        {
            if (strcmp(words[i], word) == 0)
                isUnique = 0;
        }
        if (isUnique)
        {
            strcpy(words[index], word);
            count[index]++;

            index++;
        }
        else
        {
            count[i - 1]++;
        }
    }
    fclose(fptr);
    printf("\nMost Repeated word in file: \n");
    int res = count[0];
    int in = 0;
    for (i=1; i<index; i++) {
    	if (count[i] > res) {
    		res = count[i];
    		in = i;
		}
	}
	printf("%-15s", words[in]);


    return 0;
}

int g(){


FILE* file = fopen ("/home/kimiya/Desktop/os/test/mostrepeat.txt", "r");



char line[500];
int i =0;
while (fgets(line, sizeof(line), file)) {
if(i==10)break;
printf("%s", line);
i++;
}
fclose(file);
return 0;




}
int f()
{
FILE* file = fopen ("/home/kimiya/Desktop/os/test/mostrepeat.txt", "r");

char line[500];
int i =0;
while (fgets(line, sizeof(line), file)) {

i++;
}
printf("number of lines is %d",i);
fclose(file);
return 0;

}
int d(){
FILE* file = fopen ("/home/kimiya/Desktop/os/test/mostrepeat.txt", "r");


char line[500];

while (fgets(line, sizeof(line), file)) {
if (line[0]=='#') {
    continue;
}
else{
printf("%s", line);}

}
fclose(file);
return 0;
}

//Signal handeling for ctrl+c
void sigintHandler(int sig_num)
{

    signal(SIGINT, sigintHandler);
    no_reprint_prmpt = 1;
    fflush(stdout);
}
void updateHistory(char *temp)
{
	/** Add commands to the history file as user enters them */

	// DO NOT add to history if there is no command to add
	if (!strcmp(temp, "") || !strcmp(temp, " ") || !strcmp(temp, "\n")) return;

	// In all other cases, add to history, and update number of items in history
	FILE *f = fopen("/home/kimiya/Desktop/os/history.txt", "a+");
    histItems++;
	fprintf(f, "%s", temp);
	fclose(f);
}
void printHistory()
{
	FILE *f = fopen("/home/kimiya/Desktop/os/history.txt", "a+");
	int i = 0;
    char *line = NULL;
    size_t len = 0;
	for (i = 0; i < histItems; i++)
	{
		getline(&line, &len, f);

		printf("%d %s", i+1, line);
	}
}

int main(){

    FILE *f; // this file will save history
    bool isRunning = true;
    char input[50];
    pid_t pid;
    int lastPos; // will be required for waiting / not-waiting child process to end
    int  getVal;
    printf("%s","Welcome to Our Shell we support most of the common commands and few others use help command for more information");

    f = fopen("history.txt", "w+");
	fclose(f);

    no_reprint_prmpt = 0;

    while(isRunning){
        no_reprint_prmpt = 0;
        //print the shell
        signal(SIGINT, sigintHandler);
        if (no_reprint_prmpt == 0)
			shellPrompt();



        fflush(stdout); /*######*/

        fgets(input, 50, stdin);
        updateHistory(input);
        pid = fork();
        if (pid<0){
            fprintf(stderr,"Could not fork the child\n");
        }
        else if (pid==0){

            getVal = execute(input);
            if (getVal ==1 ){
            continue;
            }
            else{
            printf("*");
            }
        }
        else //parent
        {
            // Wait for child to finish execution
			lastPos = strlen(input) - 1;
            wait(NULL);
        }

        return 0;
    }
}
