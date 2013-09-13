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
pass_list()
{
  //Use this function to pass the list to the next child.
}

//Terminate Gracefully

//Use Pipes
