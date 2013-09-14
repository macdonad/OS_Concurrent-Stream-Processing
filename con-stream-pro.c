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
int debug = 0;
int pid = 0;
int mode;
int limit = 0;
int number = 0;

//PROTOTYPES

void prompt_user();
void args_check(int, char*);


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
  if(debug){printf("Debug = %d\n",debug);}

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
  if(debug){printf("Mode: %d\n",mode);}
  if(mode == 0)
    {
      printf("Enter number of primes to generate: "); 
      scanf("%d",&number);
      if(debug){printf("Number: %d\n", number);}
    }
  else if(mode == 1)
    {
      printf("Enter limit to generate primes to: ");
      scanf("%d",&limit);
      if(debug){printf("Limit: %d\n", limit);}
    }
}
