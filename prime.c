/*
  Operation Systems
  Prof. Wolffe

  Program 1:
  Concurrent Stream Processing

  Author: Doug MacDonald

  Commands:
    -debug : Shows extra print outs, including reads and writes to pipe
    -closes : Shows print outs of closes
    -help : Shows Help Menu
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

//PROTOTYPES

void prompt_user();
void start_limit();
void print_header();
void print_info(int, int, char*);
void generate_to_limit(int, int, int);
void child_Stuff(int, int, int);
void handle_signals(int);
void child_read(int, int, int);
void print_status(int, char*);

//GLOBALS

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

  //These two variables are used to show more/less comments
  //turned on via args
  debug = 0;
  ShowCloses = 0;

  //Read in args
  //Check for debug, closes and help
  //Ignore everything else
  //-Help Shows other possible args then exits.
  while(i < argc)
    {
      if(strcmp(argv[i], "-debug") == 0)
	{
	  debug = 1;
	  printf("*Showing Reads and Writes Activated*\n");
	}
      if(strcmp(argv[i], "-closes") == 0)
	{
	  ShowCloses = 1;
	  printf("*Showing Closes Activated*\n");
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

  //Create Signal Handlers for Ctrl - C
  //Create Signal Handler for SigUsr1 - Used to end when in Number Mode
  signal(SIGINT, handle_signals);
  signal(SIGUSR1, handle_signals);

  //Call To Prompt User for Mode
  prompt_user();

  return 0;
}

//Signal Handler
//SIGINT : Closes Process
//SIGUSR1: Closes Parent Write
//This will tell every child to close as it goes through the pipes
//Parent then closes.
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
	if(ShowCloses){print_status(parent_pid, "Closing");}
	exit(0);
      }
    }
}

//Prompt User for Which Mode to use
//Also prompt for Limit or number to generate
//If they enter an incorrect mode it will prompt again.
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
      //If User selects number not requiring Children handle it here and close
      if(number == 1)
	{
	  print_header();
	  print_info(pid, 2, "My Prime");
	  if(ShowCloses){print_status(pid, "Closing");}
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
      //If User selects a limit not requiring child handle it here and close
      if(limit == 2)
	{
	  print_header();
	  print_info(pid, 2, "My Prime");
	  if(ShowCloses){print_status(pid, "Closing");}
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
      //Prompt User if an invalid mode is selected
      prompt_user();
    }
}

//Start generating primes by creating the first pipe and child process
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

//If child finds a new prime it will run this function
//the child will create a pipe, and fork then start the function where
//it will continually read and write till told to stop
// The new child process will begin reading till it finds the next prime
void continue_limit(int currentNumber)
{
  pid = getpid();
  int num = currentNumber;

  if(num <= limit || limit == 0)
    {
      //Create Pipe
      if(pipe(tempfd) < 0)
	{
	  //Failed
	  perror("Pipe Burst\n");
	  exit(1);
	}
      if((pid = fork()) < 0)
	{
          //Failed
	  perror("Fork Failed\n");
	  exit(1);
	}
      //Child
      else if(!pid)
	{
	  pid = getpid();
	  fd[READ] = tempfd[READ];
	  fd[WRITE] = tempfd[WRITE];
	  //Read till next prime or end
	  child_Stuff(fd[READ], fd[WRITE], pid);
	}
      //Parent
      else
        {
	  //Creates its signal handler for Ctrl-c
	  signal(SIGINT, handle_signals);	  
	  pid = getpid();
	  fd[WRITE] = tempfd[WRITE];
	  if(write(fd[WRITE], &num, sizeof(num))< 0)
	    {
	      perror("Child Write Failed\n");
	      exit(1);	      
	    }	  
	  if(debug){print_info(pid, num, "Write");}
	  //Read and write till end
	  child_read(fd[READ], fd[WRITE], pid);
	}
    }
}

//If a child does not have a child it will run this.
//It will read from its pipe until it finds the next prime
//Then call the function to create a new pipe and fork
//Or until it receives the value that its parent closed their end of the pipe
//The child will then close.
void child_Stuff(int pread, int pwrite, int ppid)
{
  fd[READ] = pread;
  fd[WRITE] = pwrite;
  pid = ppid;
  int buf = 10;
  ssize_t numRead;
  
  if(close(fd[WRITE]) < 0)
    {
      //Failed
      perror("Child Write Failed To Close.");
      exit(1);
    }
   numRead = read(fd[READ], &buf, sizeof(buf));
   if(numRead < 0)
     {
       //Failed
       perror("Child Read Error\n");
       exit(1);
     }
   if(buf != '\0')
     {
       //Found new prime
       number--;
       print_info(pid, buf, "My Prime");
       my_prime = buf;
       if(number <= 0)
	 {
	   //Number mode reached end
	   //Also prevents more than 1000 processes from being created in limit mode
	   SEND_SIG(parent_pid, SIGUSR1);
	   close(fd[READ]);
	   printf("Number Reached: Closing\n");
	   if(ShowCloses){print_status(pid, "Closing");}
	   exit(EXIT_SUCCESS);
	 }
     }
   //Begin Reading continually till find next prime
  while(1)
    {     	   
      numRead = read(fd[READ], &buf, sizeof(buf));
      if(numRead < 0)
	{
	  //Failed
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
	  //Print Number Read In
	  if(debug){print_info(pid, buf, "Read");}
	}
      if(buf % my_prime != 0)
	{	  
	  //Found next prime
	  continue_limit(buf);	  
	}
    }
  pid = getpid();
  print_info(pid, 0, "Limit Reached");
  if(ShowCloses){print_status(pid, "Closing");}
  exit(EXIT_SUCCESS);
}

//Child processes read from pipe and write to their child
// until their parent closes their pipe, then the child closes his
// and waits for his child to close before exiting
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
	  //Failed
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
	  //Write next number not a multiple of my number
	  if(write(fd[WRITE], &buf, numRead) != numRead)
	    {
	      //failed
	      perror("Child Write Failed\n");
	      exit(1);	      
	    }
	  else
	    {
	      //print number written to pipe
	      if(debug){print_info(pid, buf, "Write");}
	    }
	}
    }
  //Close Read
  if(close(fd[READ] < 0))
    {
      //failed
      perror("Child Read Failed To Close\n");
      exit(1);
    }
  //Close Write
  if(close(fd[WRITE]) < 0)
    {
      perror("Child Write Failed To CLose\n");
      exit(1);
    }

  //Wait for Child to end then close
  wait(NULL);
  pid = getpid();
  if(ShowCloses){print_status(pid, "Closing");}
  exit(EXIT_SUCCESS);
}

//Parent Process continues generating numbers and writing to its child
// until it reaches the limit in limit mode, or until it receives a 
//Signal in number mode. (Handled in Signal Handler)
void generate_to_limit(int pread, int pwrite, int ppid)
{
  fd[READ] = pread;
  fd[WRITE] = pwrite;
  pid = ppid;
  if(close(fd[READ]) < 0)
    {
      //Failed
      perror("Parent Read Failed To Close\n");
      exit(1);
    }
  my_prime = 2;
  print_info(pid, my_prime, "My Prime");
  if(number <= 0)
    {
      //If only last number signal end
      SEND_SIG(parent_pid, SIGUSR1);
    }
  int count = 3;
  //Generate to next prime. Starting from 3
  while(count <= limit || limit == 0)
    {
      if(count % my_prime != 0)
	{
	  //Found next number not multiple of my number
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
  //Close Write to Pipe
  if(close(fd[WRITE]) < 0)
    {
      perror("Parent Write Failed To CLose\n");
      exit(1);
    }
  //Wait for Child to end then exit
  wait(NULL);
  pid = getpid();
  if(ShowCloses){print_status(pid, "Closing");}
  exit(EXIT_SUCCESS);
}

//Prints Header
void print_header()
{
  printf("\n PID  |  Prime #  |  Status/Info  \n");
  printf("----------------------------------\n");
}

//Function to print Statuses
void print_status(int pid, char* status)
{
  printf("%d\t \t\t%s\n", pid, status);
}

//Function to print Primes, Reads and Writes.
void print_info(int pid, int prime, char* status)
{
  printf("%d\t%d\t\t%s\n",pid, prime, status);
}
