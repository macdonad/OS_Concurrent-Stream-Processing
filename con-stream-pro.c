/*
  Operation Systems
  Prof. Wolffe
  
  Program 1
  Concurrent Stream Processing
  
  Author: Doug Macdonald

  Commands:
    debug : shows extra print outs
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define READ 0
#define WRITE 1
#define MAX 1024

// GLOBALS
int debug = 0;
int pid = 0;
int mode;
int limit = 0;
int number = 0;
int my_num = 2;

//PROTOTYPES

void prompt_user();
void args_check(int, char*);
void generate_number(int);
void generate_limit(int);
void print_header();
void print_prime(int, int);
void print_status(int, char*);
void found_prime(int);


//Prompt user for mode
//user debug command line arg to see statuses
int
main(int argc, char *argv[])
{  
  //Check args for "Debug"
  //If in args, activate debug mode  
  int i = 0;
  
  if(debug){printf("Total Number of Args: %d\n", argc);}
  while(i < argc)
    {
      if(debug){printf("Args%d: %s\n", i, argv[i]);}
      if(strcmp(argv[i], "debug") == 0)
	{
	  debug = 1;
	}

      i++;
    }
  if(debug){printf("Debug Activiated\n");}

  //Prompt User for Mode
  prompt_user();

  return 0;
}

//User Prompt
//Get Mode and Limit or Number to generate
void 
prompt_user()
{
  printf("Generate primes to a limit(1) or generate a number(0) of primes: ");
  scanf("%d", &mode);  
  if(mode == 0)
    {
      if(debug){printf("Mode: Number\n");}
      printf("Enter number of primes to generate: "); 
      scanf("%d",&number);
      if(debug){printf("Number: %d\n", number);}
      generate_number(number);
    }
  else if(mode == 1)
    {
      if(debug){printf("Mode: Limit\n");}
      printf("Enter limit to generate primes to: ");
      scanf("%d",&limit);
      if(debug){printf("Limit: %d\n", limit);}
      generate_limit(limit);
    }
}

//Generate specific number of primes
void
generate_number(int number)
{
  if(debug){printf("Number: %d\n", number);}
  print_header();
}

//Generate primes to a limit
void 
generate_limit(int limit)
{
  if(debug){printf("Limit: %d\n", limit);}
  print_header();
  pid = getpid();
  print_prime(pid, my_num);
  if(debug){print_status(pid, "Found Prime");}

  int num = my_num + 1;
  while(num <= limit)
    {
      if(num % my_num != 0)
	{
	  if(debug){printf("Next prime: %d\n", num);}
	  found_prime(num);
	}
      num++;
    }
}

//Print Header Info
void 
print_header()
{
  printf("PID  |  Prime #  |  Status Info\n");
  printf("-------------------------------\n");
}

//Print PID and Prime Number
void 
print_prime(int pid, int number)
{
  printf("%d\t%d\n", pid, number);
}

//Print PID and Status
//used in debugging
void
print_status(int pid, char* status)
{
  if(debug){printf("%d\t\t\t%s\n",pid, status);}
}

//Found next prime
//Spawn new process and set up pipe
void
found_prime(int prime)
{
  int fd[2];
  char str[MAX];

  if(pipe(fd) < 0)
    {
      perror("Its a Pipe Bomb!\n");
      exit(1);
    }

  if((pid = fork()) < 0)
    {
      perror("Child Bearing Issue\n");
      exit(1);
    }
  else if(pid > 0)
    {
      //Parent
      dup2(fd[WRITE], STDOUT_FILENO);
      write (STDOUT_FILENO, &prime, sizeof(prime));
      close(fd[READ]);
      close(fd[WRITE]);
    }
  else
    {
      //Child
      dup2(fd[READ], STDIN_FILENO);
      read(STDIN_FILENO, &my_num, sizeof(my_num));
      pid = getpid();
      print_prime(pid, my_num);
      if(debug){print_status(pid, "Printing Prime");}
      close(fd[READ]);
      close(fd[WRITE]);
      exit(0);
    }
}
