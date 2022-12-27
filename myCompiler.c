#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#define MAX_WORDS   1000
#define MAX_LINE    70 /*70 character max per line*/

void shellPrompt(){
	// We print the prompt
    //getcwd return the name of the currentDirectory
    char* currentDirectory =  calloc(1024, sizeof(char));
	char hostn[1204] = "";
	gethostname(hostn, sizeof(hostn));
	printf("%s@%s %s > ", getenv("LOGNAME"), hostn, getcwd(currentDirectory, 1024));
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
    printf("%s",command);
    if (strcmp(command, "exit")==0){
        exit(0);
    }

    else if (strcmp(command, "help")==0)
	{
		printf("%s", "\nfirstword: prints the first word of the fil.\nmostrepeat: prints the most repeated word in the file.");
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
    else if (strcmp(command, "firstword")==0){
         firstword();
        return 1;
    }
    else if (strcmp(command, "mostrepeat")==0){
        mostrepeat();
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
    //most common command
    val = execvp(args[0], args);
    return val;
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
int firstword() {
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
int mostrepeat() {
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
    printf("\nOccurrences of all distinct words in file: \n");
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
int f(){
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


    while(isRunning){
        //print the shell
        shellPrompt();
        fflush(stdout); /*######*/

        fgets(input, 50, stdin);

        pid = fork();
        if (pid<0){
            fprintf(stderr,"ERROR\n");
        }
        else if (pid==0){

            getVal = execute(input);
            if (getVal ==1 ){
            continue;
            }
            else{
            printf(" this command is not supported");
            }
        }
        else //parent
        {
            // Wait for child to finish execution unless user ends with '&' (reversing the original part of the problem)
			lastPos = strlen(input) - 1;
            wait(NULL);
        }

        return 0;
    }
}
