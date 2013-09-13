/*
  Operation Systems
  Prof. Wolffe
  
  Program 1
  Concurrent Stream Processing
  
  Author: Doug Macdonald

*/

#include <stdio.h>
#include <string.h>

// GLOBALS
int pid = 0;

//PROTOTYPES




//Args
// show the fist 'K' primes
// Show all primes from 2 to a limit
int
main(int argc, char *argv[])
{
  int i = 0;
  printf("Total Number of Args: %d\n", argc);
  while(i < argc)
    {
      printf("Args%d: %s\n", i, argv[i]);
      i++;
    }
  return 0;
}


//Parent gets list of all number in args from user

//Parent finds first prime
//Eliminates all numbers after that are a multiple of that first number
//Passes new list to child process.
void
read_list()
{
  //Use this function to pass the list to the next child.
  //if child read from pipe
}

void 
write_list()
{
  //if parent write list to pipe
}

//Set up pipes with parent and child
void 
connect_pipes()
{
  if(pid < 0)
    {
      //error
    }
  else if (pid = 0)
    {
      //set up child's connection
      read_list();
    }
  else if (pid > 0)
    {
      //set up Parent connection
      write_list();
    }
}


//Terminate Gracefully

//Use Pipes
