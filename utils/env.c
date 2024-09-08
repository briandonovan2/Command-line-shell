#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[], char *envp[])
{
  //Check if there are no command-line arguments
  if (argc == 1)
  {
    for (int i = 0; envp[i] != NULL; i++)
    {
      printf("%s\n", envp[i]);
    }
    return 0;
  }

  //If there are arguments, set the values and execute the program
  char *new_envp[100]; //New env array
  int new_envp_index = 0;

  for (int i = 1; i < argc - 1; i++)
  {
    //Split name=value argument
    char *env_var = argv[i];
    char *equals_sign = strchr(env_var, '=');

    char *env_name = env_var;
    char *env_value = equals_sign + 1;

    //Set environment variable in new environment array
    new_envp[new_envp_index] = env_var;
    new_envp_index++;
  }

  //Null-terminate the new environment array
  new_envp[new_envp_index] = NULL;

  //Execute the specified program with the modified environment
  execve(argv[argc - 1], &argv[argc - 1], new_envp);
  return 1;
}

static void usage(void)
{
  printf("env, set environment variables and execute program\n");
  printf("usage: env [name=value ...] PROG ARGS");
}