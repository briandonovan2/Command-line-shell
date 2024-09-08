#include <stdio.h>
#include <string.h>

#include "builtins.h"
#include "hash.h"
#include "process.h"
#include <sys/wait.h>

// No command line can be more than 100 characters
#define MAXLENGTH 100
#define WHITESPACE " \t\n\r"

void
shell (FILE *input)
{
  hash_init (100);
  hash_insert ("?", "0");
  char buffer[MAXLENGTH];
  while (1)
    {
      // Print the cursor and get the next command entered
      printf ("$ ");
      memset (buffer, 0, sizeof (buffer));
      if (fgets (buffer, MAXLENGTH, input) == NULL)
        break;

      if (input != stdin)
        printf ("%s", buffer);

      char *command = strtok (buffer, WHITESPACE);

      if (command == NULL)
        {
          continue; // go to the next loop iteration if no command is provided
        }
      if (!strncmp (command, "echo", 5))
        {
          echo (buffer + 5);
        }
      else if (!strncmp (command, "export", 7))
        {
          export(buffer + 7);
        }
      else if (!strncmp (command, "unset", 6))
        {
          unset (buffer + 6);
        }
      else if (!strncmp (command, "pwd", 4))
        {
          pwd ();
        }
      else if (!strncmp (command, "quit", 5))
        {
          break;
        }
      else if (!strncmp (command, "cd", 3))
        {
          command = strtok (NULL, WHITESPACE);
          if (command != NULL)
            chdir (command);
        }
      else if (!strncmp (command, "which", 6))
        {
          command = strtok (NULL, WHITESPACE);
          if (command != NULL)
            which (command);
        }
      else
        {
          // Child process
          // call strtok
          // call strdup for each arg "ls", "-s", "-a", "data"
          // call execvp
          // fork()
          // call execvp(command, args[])
          __pid_t child_pid = fork ();
          if ((child_pid) < 0)
            {
              perror ("fork");
              exit (1);
            }
          if (child_pid == 0)
            {
              char *args[MAXLENGTH];
              int i = 0;
              // go through and tokenize and copy each command
              while (command != NULL)
                {
                  args[i++] = strdup (command);
                  command = strtok (NULL, WHITESPACE);
                }
              args[i] = NULL;                 // Last element should be NULL
              if (execvp (args[0], args) < 0) // Execute the command
                {
                  perror ("execvp");
                  exit (1);
                }
            }
          else // Parent Process
            {
              int status;
              waitpid (child_pid, &status, 0);
              if (WIFEXITED (status) && WEXITSTATUS (status) == 0)
                {
                  hash_insert ("?", "0");
                }
              else
                {
                  hash_insert ("?", "1");
                }
            }
        }
    }
  printf ("\n");
  hash_destroy ();
}
