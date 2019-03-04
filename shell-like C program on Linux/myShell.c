#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/wait.h>


//230201054 Elif Yılmaz
//220201026 Buket Şentürk

/* Program behave like a shell
!_NOTE_!:
user should enter like (gedit &)for background process , not (gedit&)

when calling a system command, program check is it has one attribute or not. For example it can call
(ls)  or (ls -l) it does nor check for second or more argumants.(only one command pipe needed)

when calling two system commands with using pipe,program does not check for attributes.
commands like (ls | wc ) executes with pipe but commands like
(ls -al | wc -l) cause LOOP

*/



#define PIPE_RD 0
#define PIPE_WR 1

int isFileExistsAccess(const char *path)
{
    // Check for file existence
    if (access(path, F_OK) == -1)
        return 0;
    return 1;
}


int main(void)
{

  char varChild[100]=""; //copy command from console to use in child process for pipe |
  char history[10][100];
  int j=0; // index of history array

  while(1){

    char command[100];  //array to take command input from console
    char temp[10][100]={""}; //array to keep all argumants(token) of the command (for example command cd <path> tokens: cd / <path>)
    char *token;  //tokens in temp array
    int i=0;    // index of temp array
    char tempChild[10][100]={""}; //array to keep all argumants(tokenChild) of the command (for example command : ls -l tokens :ls / -l for child process to use)
    char *tokenChild; //tokens in tempChild array
    int iChild=0; //index of tempChild array

    const char s[3]="\n ";     // delimeter, to find tokens of command
    const char sFindloc[3]="\n:";     // delimeter, to find tokens of PATH variable
    const char sChild[3]="\n| ";     // delimeter,to find tokens of varChild


    printf("myshell> ");

    fgets(command,100,stdin); //takes all input line from console

    if(j<=10){ //To only keep 10 element in history array
      j = j+1;//index of history
    }

    if(j==11){ //if array size is larger than 10, first element is removed in for loop
      for (int c = 0; c < 10; c++){
          strcpy(history[c],history[c+1]); //remove first element
      }
      j=10; //keep size max 10 again.
    }

    strcpy(varChild,command); // copy command from console to use in child process,for pipe
    strcpy( history[j-1], command ); //append at end of the array j-1 is the last index
    token = strtok(command,s); // get the first token from command
    while( token != NULL )//walk through other tokens
      {    /* While there are tokens in "string" */

          strcpy(temp[i],token); //array to keep all argumants(token) of the command (for example command cd <path> tokens: cd / <path>)
          token = strtok(NULL,s);           /* Get next token: */
          i=i+1; //index of temp array
      }

    //***CD*****
    if(strcmp(temp[0],"cd")==0){

      if(chdir(temp[1]) !=0){
        chdir(getenv("HOME"));
      }
    }


    //***DIR*****
    else if(strcmp(temp[0],"dir")==0){

        char cwd[1000]; //current working directory
        if (getcwd(cwd, sizeof(cwd)) == NULL)
          perror("getcwd() error");
        else
          printf("current working directory is: %s\n", cwd);
    }



    //***HISTORY*****
    else if(strcmp(temp[0],"history")==0){

      for (int i=0;i<j;i++){
        printf("[%d] ==> %s\n",i+1,history[i] ); //print history array
      }

    }

    //***FINDLOC*****
    else if(strcmp(temp[0],"findloc")==0){
      char tokenFindlocInteg[100]=""; //integrate tokens to find executable file path.
      char *tokenFindloc;
      char pathvar[100]="";

      strcpy(pathvar,getenv("PATH"));
      tokenFindloc = strtok(pathvar,sFindloc); // get the first token
      while( tokenFindloc != NULL )//walk through other tokens
        {    /* While there are tokens in "string" */
          strcpy(tokenFindlocInteg,tokenFindloc);
          strcat(tokenFindlocInteg,"/");
          strcat(tokenFindlocInteg,temp[1]); //construct path (if command = findloc ls then temp[0]=findloc,temp[1]=ls)

          if (isFileExistsAccess(tokenFindlocInteg)) //check if file exists
          {
              printf("File exists at path '%s'\n", tokenFindlocInteg);
              break;
          }
          tokenFindloc = strtok(NULL,sFindloc);           /* Get next token: */
        }
    }

    //***BYE*****
    else if(strcmp(temp[0],"bye")==0){
      exit(0);
    }

    //***ELSE*****
    else { // system commands

      const char needle1[10] = "|"; //if string contains & at the end of it.(user should enter as gedit & not gedit&)
      char *ret1;
      ret1 = strstr(history[j-1], needle1);

      if(ret1 != NULL){ // contains |,pipe

        tokenChild = strtok(varChild,sChild); // get the first token

        while( tokenChild != NULL )//walk through other tokens
          {    // While there are tokens in "string"
              strcpy(tempChild[iChild],tokenChild);
              tokenChild = strtok(NULL,sChild);           // Get next token:
              iChild=iChild+1;
          }

        int    pipes[2];
        pid_t   p1, p2;

        pipe(pipes); //create pipe
        //Lets assumme command is ls | wc then: tempChild[1]= wc and tempChild[0]=ls
        p1 = fork();
        if ( p1 == 0 ) {
          //child1
          char* cmd[2];
          cmd[0]=tempChild[1];
          cmd[1]=NULL;
          close( STDIN_FILENO );
          dup( pipes[PIPE_RD] );
          close( pipes[PIPE_WR] );
          if(execvp(cmd[0],cmd)==-1){
            printf("Error in calling exec!!");
          }

        }
        if ( p1 != 0 ) {
          p2 = fork();
          if ( p2 == 0 ) {
            //child2
            char* cmd[2];
            cmd[0]=tempChild[0];
            cmd[1]=NULL;
            close( STDOUT_FILENO );
            dup( pipes[PIPE_WR] );
            close( pipes[PIPE_RD] );
            if(execvp(cmd[0],cmd)==-1){
              printf("Error in calling exec!!");
            }

          }
        }
        close( pipes[PIPE_RD] );
        close( pipes[PIPE_WR] );

      }

      else{ // only one command from user no |, no need for pipe

      if (fork() == 0){ //child process
        char tokenFindlocInteg[100]; //integrate tokens to find executable file path.
        char *tokenFindloc;
        char pathvar[100]="";

        strcpy(pathvar,getenv("PATH"));

        tokenFindloc = strtok(pathvar,sFindloc); // get the first token

        while( tokenFindloc != NULL )//walk through other tokens
          {    /* While there are tokens in "string" */

            strcpy(tokenFindlocInteg,tokenFindloc);
            strcat(tokenFindlocInteg,"/");
            strcat(tokenFindlocInteg,command);//construct path

            if (isFileExistsAccess(tokenFindlocInteg)) //check if file exists
            {

              if(strcmp(temp[1],"")!=0){ //if there is an attribute like -l (command : ls -l)
              char* argv[3];
              argv[0] = tokenFindlocInteg;
              argv[1]=temp[1];
              argv[2] = NULL;
              if(execvp(argv[0],argv)==-1){
          			printf("Error in calling exec!!");
              }
              else{
                wait(NULL);
              }
                break;

              }

              else{ //there is no attribute, not like ls -l , (command ls)
                char* argv[2];
                argv[0] = tokenFindlocInteg;
                argv[1] = NULL;
                if(execvp(argv[0],argv)==-1){
            			printf("Error in calling exec!!");
                }
                else{
                  wait(NULL);
                }
                  break;
              }
            }
            tokenFindloc = strtok(NULL,sFindloc); // Get next token
         }
      }

        else{
          const char needle[10] = "&\n"; //if string contains & at the end of it(foreground).user should enter as gedit & not gedit&
          char *ret;
          ret = strstr(history[j-1], needle);
          if(ret == NULL){ //foreground command
            wait(NULL);
          }

        }
      }
    }

  }

}
