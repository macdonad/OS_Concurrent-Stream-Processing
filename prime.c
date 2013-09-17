/*
  Operation Systems
  Prof. Wolffe

  Program 1:
  Concurrent Stream Processing

  Author: Doug MacDonald

  Commands:
    debug : Shows extra print outs
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1
#define MAX 1024

void prompt_user();
void start_limit();
void print_header();
void print_info(int, int, char*);
void generate_to_limit(int, int, int);
void child_Stuff(int, int, int);
void handle_signals(int);
void child_read(int, int, int);

int BUF_SIZE = 4;
int debug;
int mode;
int number;
int limit;
int fd[2];
int pid;

int main(int argc, char *argv[])
{
  int i = 0;
  debug = 0;

  while(i < argc)
    {
      if(strcmp(argv[i], "debug") == 0)
	{
	  debug = 1;
	}
      i++;
    }
  signal(SIGINT, handle_signals);
  prompt_user();
  return 0;
}

void handle_signals(int signal)
{
  switch(signal)
    {
    case SIGINT:
      {
	printf("\nReceiving Signal: Closing\n");
	close(fd[READ]);
	close(fd[WRITE]);
	exit(0);
	break;
      }
    }
}

void prompt_user()
{
  printf("Generate primes to a limit(1) or generate a number(0) of primes: ");
  scanf("%d", &mode);
  number = 0;
  limit = 0;
  if(mode == 0)
    {
      printf("Enter number of primes to generate: ");
      scanf("%d", &number);
      //Handle Generating Number
    }
  else if(mode == 1)
    {
      printf("Enter limit to generate primes to: ");
      scanf("%d",&limit);
      //Handle Generating Limit
      print_header();
      start_limit();
    }
}

void start_limit()
{
  pid = getpid();

  if(3 <= limit)
    {
      //Create Pipe
      if(pipe(fd) < 0)
	{
	  perror("Pipe Burst\n");
	  exit(1);
	}
      //Failed
      if((pid = fork()) < 0)
	{
	  perror("Fork Failed\n");
	  exit(1);
	}
      //Child
      else if(!pid)
	{
	  signal(SIGINT, handle_signals);
	  pid = getpid();
	  child_Stuff(fd[READ], fd[WRITE], pid);
	}
      else
        {
	  pid = getpid();
	  generate_to_limit(fd[READ], fd[WRITE], pid);
	}
    }
}

void continue_limit(int currentNumber)
{
  pid = getpid();
  int num = currentNumber;

  if(num <= limit)
    {
      //Create Pipe
      if(pipe(fd) < 0)
	{
	  perror("Pipe Burst\n");
	  exit(1);
	}
      //Failed
      if((pid = fork()) < 0)
	{
	  perror("Fork Failed\n");
	  exit(1);
	}
      //Child
      else if(!pid)
	{
	  pid = getpid();
	  child_Stuff(fd[READ], fd[WRITE], pid);
	}
      else
        {
	  signal(SIGINT, handle_signals);	  
	  pid = getpid();
	  child_read(fd[READ], fd[WRITE], pid);
	}
    }
}

void child_Stuff(int pread, int pwrite, int ppid)
{
  fd[READ] = pread;
  fd[WRITE] = pwrite;
  pid = ppid;
  int buf = 10;
  ssize_t numRead;
  int my_prime = 0;
  if(close(fd[WRITE]) < 0)
    {
      perror("Child Write Failed To Close.");
      exit(1);
    }
   numRead = read(fd[READ], &buf, sizeof(buf));
   if(numRead < 0)
     {
       perror("Child Read Error\n");
       exit(1);
     }
   if(buf != '\0')
     {
       print_info(pid, buf, "My Prime");
       my_prime = buf;
     }
  while(1)
    {     	   
      numRead = read(fd[READ], &buf, sizeof(buf));
      if(numRead < 0)
	{
	  perror("Child Read Error\n");
	  exit(1);
	}
      if(numRead == 0)
	{
	  //Reached Limit Stop Reading      
	  break;
	  //send signal or whatevs
	}
      if(buf != '\0')
	{
	  print_info(pid, buf, "Read");
	}
      if(buf % my_prime != 0)
	{
	  continue_limit(buf);	  
	}
    }
  pid = getpid();
  printf("%d Child Closing\n", pid);
  exit(EXIT_SUCCESS);
}

void child_read(int pread, int pwrite, int ppid)
{ 
  fd[READ] = pread;
  fd[WRITE] = pwrite;
  pid = ppid;
  int buf = 10;
  ssize_t numRead;

  while(1)
    {
      numRead = read(fd[READ], &buf, sizeof(buf));
      if(numRead < 0)
	{
	  perror("Child Read Error\n");
	  exit(1);
	}
      if(numRead == 0)
	{
	  //Reached Limit Stop Reading
	  break;
	}
      if(buf != '\0')
	{
	  print_info(pid, buf, "Read");
	}
      if(write(fd[WRITE], &buf, numRead) != numRead)
	{
	  perror("Child Write Failed\n");
	  exit(1);	      
        }	      
    }
  if(close(fd[READ] < 0))
    {
      perror("Child Read Failed To Close\n");
      exit(1);
    }
  if(close(fd[WRITE]) < 0)
    {
      perror("Child Write Failed To CLose\n");
      exit(1);
    }

  wait(NULL);
  pid = getpid();
  printf("%d Child Closing\n", pid);
  exit(EXIT_SUCCESS);
}

void generate_to_limit(int pread, int pwrite, int ppid)
{
  fd[READ] = pread;
  fd[WRITE] = pwrite;
  pid = ppid;
  if(close(fd[READ]) < 0)
    {
      perror("Parent Read Failed To Close\n");
      exit(1);
    }
  int my_prime = 2;
  print_info(pid, my_prime, "My Prime");
  int count = 3;
  while(count <= limit)
    {
      if(count % my_prime != 0)
	{
	  print_info(pid, count, "Write");
	  BUF_SIZE =  sizeof(count);
	  if(write(fd[WRITE], &count, BUF_SIZE) != BUF_SIZE)
	    {
	      perror("Parent Failed On Write\n");
	      exit(1);
	    }
	}
      count++;
    }

  if(close(fd[WRITE]) < 0)
    {
      perror("Parent Write Failed To CLose\n");
      exit(1);
    }

  wait(NULL);
  pid = getpid();
  printf("%d Parent Closing\n", pid);
  exit(EXIT_SUCCESS);
}

void print_header()
{
  printf(" PID  |  Prime #  |  Status/Info  \n");
  printf("----------------------------------\n");
}

void print_info(int pid, int prime, char* status)
{
  printf("%d\t%d\t\t%s\n",pid, prime, status);
}
