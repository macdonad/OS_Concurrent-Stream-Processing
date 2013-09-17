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

#define READ 0
#define WRITE 1
#define MAX 1024
//#define BUF_SIZE 4

void prompt_user();
void start_limit();
void print_header();
void print_info(int, int, char*);

int BUF_SIZE = 4;
int debug;
int mode;
int number;
int limit;


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
  prompt_user();
  return 0;
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
      print_info(0, limit, "Limit");
      start_limit();
    }
}

void start_limit()
{
  int pid = getpid();
  int fd[2];
  int buf;
  ssize_t numRead;

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
	  if(close(fd[WRITE]) < 0)
	    {
	      perror("Child Write Failed To Close.");
	      exit(1);
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
		}
	      if(write(STDOUT_FILENO, &buf, numRead) != numRead)
		{
		  perror("Child Write Failed\n");
		  exit(1);
		}	      
	      printf("Num Read = %d\n", buf);
	    }
	  write(STDOUT_FILENO, "\n", 1);
	  if(close(fd[READ] < 0))
	    {
	      perror("Child Read Failed To Close\n");
	      exit(1);
	    }
	  pid = getpid();
	  printf("%d Child Closing\n", pid);
	  exit(EXIT_SUCCESS);
	}
      else
        {
	  if(close(fd[READ]) < 0)
	    {
	      perror("Parent Read Failed To Close\n");
	      exit(1);
	    }
	  int count = 5;
	  BUF_SIZE = sizeof(count);
	  if(write(fd[WRITE], &count, BUF_SIZE) != BUF_SIZE)
	    {
	      perror("Parent Failed On Write\n");
	      exit(1);
	    }
	  count = 4;
	  BUF_SIZE = sizeof(count);
	  if(write(fd[WRITE], &count, BUF_SIZE) != BUF_SIZE)
	    {
	      perror("Parent Failed On Write\n");
	      exit(1);
	    }
	  count = 3;
	  BUF_SIZE = sizeof(count);
	  if(write(fd[WRITE], &count, BUF_SIZE) != BUF_SIZE)
	    {
	      perror("Parent Failed On Write\n");
	      exit(1);
	    }
	  count = 2;
	  BUF_SIZE = sizeof(count);
	  if(write(fd[WRITE], &count, BUF_SIZE) != BUF_SIZE)
	    {
	      perror("Parent Failed On Write\n");
	      exit(1);
	    }
	  count = 1;
	  BUF_SIZE = sizeof(count);
	  if(write(fd[WRITE], &count, BUF_SIZE) != BUF_SIZE)
	    {
	      perror("Parent Failed On Write\n");
	      exit(1);
	    }
	  count = 0;
	  BUF_SIZE = sizeof(count);
	  if(write(fd[WRITE], &count, BUF_SIZE) != BUF_SIZE)
	    {
	      perror("Parent Failed On Write\n");
	      exit(1);
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
    }
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
