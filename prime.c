/*
  Operation Systems
  Prof. Wolffe

  Program 1:
  Concurrent Stream Processing

  Author: Doug MacDonald

  Commands:
    -debug : Shows extra print outs, including reads and writes to pipe
    -closes : Shows print outs of closes
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

#define READ 0
#define WRITE 1
#define MAX 1024
#define SEND_SIG(pid, sig) kill(pid, sig)

void prompt_user();
void start_limit();
void print_header();
void print_info(int, int, char*);
void generate_to_limit(int, int, int);
void child_Stuff(int, int, int);
void handle_signals(int);
void child_read(int, int, int);

int my_prime;
int BUF_SIZE = 4;
int debug;
int mode;
int number;
int limit;
int fd[2];
int tempfd[2];
int pid;
pid_t parent_pid;
int ShowCloses;

int main(int argc, char *argv[])
{
  parent_pid = getpid();
  int i = 0;
  debug = 0;
  ShowCloses = 0;

  while(i < argc)
    {
      if(strcmp(argv[i], "-debug") == 0)
	{
	  debug = 1;
	}
      if(strcmp(argv[i], "-closes") == 0)
	{
	  ShowCloses = 1;
	}
      if(strcmp(argv[i], "-help") == 0)
	{
	  printf("--------------------------------------\n");
	  printf("- Welcome To Prime Number Generating -\n");
	  printf("-                                    -\n");
	  printf("-Options:                            -\n");
	  printf("- -debug  : Shows Reads and Writes   -\n");
	  printf("- -closes : Shows Process Closes     -\n");
	  printf("- -help   : Shows this screen        -\n");
	  printf("--------------------------------------\n");
	  exit(0);
	}
      i++;
    }
  signal(SIGINT, handle_signals);
  signal(SIGUSR1, handle_signals);
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
    case SIGUSR1:
      {
	close(fd[WRITE]);
	close(fd[READ]);
	wait(NULL);
	if(ShowCloses){print_info(parent_pid, 0, "Closing");}
	exit(0);
      }
    }
}

void prompt_user()
{
  printf("Generate primes to a limit(1) or generate a number(0) of primes: ");
  scanf("%d", &mode);
  number = 0;
  limit = 0;
  pid = getpid();
  if(mode == 0)
    {
      printf("Enter number of primes to generate: ");
      scanf("%d", &number);
      //Handle Generating Number
      if(number == 1)
	{
	  print_header();
	  print_info(pid, 2, "My Prime");
	  if(ShowCloses){print_info(pid, 0, "Closing");}
	  exit(0);
	}
      else if(number == 0)
	{
	  print_header();
	  exit(0);
	}
      else
	{
	  number--;
	  print_header();
	  start_limit();
	}
    }
  else if(mode == 1)
    {
      printf("Enter Limit To Generate Primes To: ");
      scanf("%d",&limit);
      //Handle Generating Limit
      if(limit == 2)
	{
	  print_header();
	  print_info(pid, 2, "My Prime");
	  if(ShowCloses){print_info(pid, 0, "Closing");}
	  exit(0);
	}
      else if (limit == 0)
	{
	  print_header();
	  exit(0);
	}
      else
	{
	  number = 1000;
	  print_header();
	  start_limit();
	}
    }
  else
    {
      prompt_user();
    }
}

void start_limit()
{
  pid = getpid();

  if(2 <= limit || limit == 0)
    {
      //Create Pipe
      if(pipe(tempfd) < 0)
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
	  fd[READ] = tempfd[READ];
	  fd[WRITE] = tempfd[WRITE];
	  child_Stuff(fd[READ], fd[WRITE], pid);
	}
      else
        {
	  pid = getpid();
	  fd[WRITE] = tempfd[WRITE];
	  generate_to_limit(fd[READ], fd[WRITE], pid);
	}
    }
}

void continue_limit(int currentNumber)
{
  pid = getpid();
  int num = currentNumber;

  if(num <= limit || limit == 0)
    {
      //Create Pipe
      if(pipe(tempfd) < 0)
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
	  fd[READ] = tempfd[READ];
	  fd[WRITE] = tempfd[WRITE];
	  child_Stuff(fd[READ], fd[WRITE], pid);
	}
      else
        {
	  signal(SIGINT, handle_signals);	  
	  pid = getpid();
	  fd[WRITE] = tempfd[WRITE];
	  if(write(fd[WRITE], &num, sizeof(num))< 0)
	    {
	      perror("Child Write Failed\n");
	      exit(1);	      
	    }	  
	  if(debug){print_info(pid, num, "Write");}
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
       number--;
       print_info(pid, buf, "My Prime");
       my_prime = buf;
       if(number <= 0)
	 {
	   SEND_SIG(parent_pid, SIGUSR1);
	   close(fd[READ]);
	   printf("Number Reached: Closing\n");
	   if(ShowCloses){print_info(pid, 0, "Closing");}
	   exit(EXIT_SUCCESS);
	 }
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
      if(buf != '\0')
	{
	  if(debug){print_info(pid, buf, "Read");}
	}
      if(buf % my_prime != 0)
	{	  
	  continue_limit(buf);	  
	}
    }
  pid = getpid();
  print_info(pid, 0, "Limit Reached");
  if(ShowCloses){print_info(pid, 0, "Closing");}
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
	  if(debug){print_info(pid, buf, "Read");}
	}
      if(buf % my_prime != 0)
	{
	  if(write(fd[WRITE], &buf, numRead) != numRead)
	    {
	      perror("Child Write Failed\n");
	      exit(1);	      
	    }
	  else
	    {
	      if(debug){print_info(pid, buf, "Write");}
	    }
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
  if(ShowCloses){print_info(pid, 0, "Closing");}
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
  my_prime = 2;
  print_info(pid, my_prime, "My Prime");
  if(number <= 0)
    {
      SEND_SIG(parent_pid, SIGUSR1);
    }
  int count = 3;
  while(count <= limit || limit == 0)
    {
      if(count % my_prime != 0)
	{
	  if(debug){print_info(pid, count, "Write");}
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
  if(ShowCloses){print_info(pid, 0, "Closing");}
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
