#include "hash.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Given a message as input, print it to the screen followed by a
// newline ('\n'). If the message contains the two-byte escape sequence
// "\\n", print a newline '\n' instead. No other escape sequence is
// allowed. If the sequence contains a '$', it must be an environment
// variable or the return code variable ("$?"). Environment variable
// names must be wrapped in curly braces (e.g., ${PATH}).
//
// Returns 0 for success, 1 for errors (invalid escape sequence or no
// curly braces around environment variables).
int
echo (char *message)
{
  int outputIndex = 0;
  bool prevSpace = false;

  for (int i = 0; message[i] != '\0'; i++)
    {
      if (message[i] == ' ')
        {
          if (!prevSpace)
            {
              putchar (' ');
            }
          prevSpace = true;
        }
      else if (message[i] == '\\' && message[i + 1] == 'n')
        {
          putchar ('\n');
          i++; // Skip the n character in the escape sequence
        }
      else if (message[i] == '$' && message[i + 1] != '{')
        {
          if (strncmp (message + i, "$?", 2) == 0)
            {
              printf ("%s", hash_find ("?"));
              i++; // skip the '?'
            }
          else
            {
              printf ("$");
            }
        }
      else if (message[i] == '$' && message[i + 1] == '{')
        {
          // Handle environment variable
          int j = i + 2;
          char varName[50];
          int varIndex = 0;

          while (message[j] != '}' && message[j] != '\0')
            {
              varName[varIndex++] = message[j++];
            }

          varName[varIndex] = '\0'; // Null terminate the variable name

          if (message[j] == '}')
            {
              i = j; // Skip the variable name and the closing curly brace
              const char *varValue = hash_find (varName);
              if (varValue != NULL)
                {
                  printf ("%s", varValue);
                  break;
                }
              else
                {
                  putchar ('\n');
                  break;
                }
            }
        }
      else
        {
          putchar (message[i]);
          prevSpace = false;
        }
    }
  return 0;
}

// Given a key-value pair string (e.g., "alpha=beta"), insert the mapping
// into the global hash table (hash_insert ("alpha", "beta")).
//
// Returns 0 on success, 1 for an invalid pair string (kvpair is NULL or
// there is no '=' in the string).
int export(char *kvpair)
{
  if (kvpair == NULL || strchr (kvpair, '=') == NULL)
    {
      // Invalid pair string.
      return 1;
    }
  // Parse the key and value from the kvpair string
  char *key = strtok (kvpair, "=");
  char *value = strtok (NULL, "=");

  hash_insert (key, value);

  // Set or update the environment variable
  setenv (key, value, 1);

  return 0;
}

// Prints the current working directory (see getcwd()). Returns 0.
int
pwd (void)
{
  char *cwd = NULL;
  size_t size = 128; // Initial buffer size

  while (1)
    {
      cwd = (char *)realloc (cwd, size);
      if (!cwd)
        {
          perror ("Error allocating memory for cwd");
          return 1;
        }

      if (getcwd (cwd, size))
        {
          printf ("%s\n", cwd);
          free (cwd);
          return 0;
        }

      size *= 2; // Double the buffer size and retry
    }
}

// Removes a key-value pair from the global hash table.
// Returns 0 on success, 1 if the key does not exist.
int
unset (char *key)
{
  if (hash_remove (key))
    {
      return 0;
    }
  return 1;
}

// Given a string of commands, find their location(s) in the $PATH global
// variable. If the string begins with "-a", print all locations, not just
// the first one.
//
// Returns 0 if at least one location is found, 1 if no commands were
// passed or no locations found.
int
which (char *cmdline)
{
  // char *cmdline = strtok(strdup(cmdline), " \t\n");
  // if (!cmdline) return 1; // Ensure cmdline is not NULL

  //  char *cmd = cmdline;
  bool printAll = false;

  //   if (strlen(cmdline) >= 3 && strncmp(cmdline, "-a", 2) == 0) {
  //       printAll = true;
  //       cmd = cmdline + 3;
  //   }

  // check if the command is built-in
  if (!strcmp (cmdline, "cd") || !strcmp (cmdline, "pwd")
      || !strcmp (cmdline, "echo") || !strcmp (cmdline, "quit")
      || !strcmp (cmdline, "which") || !strcmp (cmdline, "export"))
    {
      printf ("%s: dukesh built-in command\n", cmdline);
      return 0;
    }
  // Check if the file is executable: i.e ./bin/ls
  // call get env to get the path
  // then strtok to get the different comonents
  // make a copy of the path

  // Get the PATH environment variable.
  char *path = getenv ("PATH");
  if (!path)
    return 1; // Return if $PATH is not set
  char *pathCopy = strdup (
      path); // Make a copy as strtok modifies the string (NO NEED FOR THIS)
  char *dir = strtok (pathCopy, ":"); // Tokenize by :
  // check just ./ / and ../ using strncmp and then the first or the first two
  // bytes call access and then return 1
  if (strncmp (cmdline, "./", 2) == 0 || strncmp (cmdline, "/", 1) == 0
      || strncmp (cmdline, "../", 3) == 0)
    {
      if (access (cmdline, X_OK) == 0)
        {
          printf ("%s\n", cmdline);
          free (pathCopy); // Free the allocated memory before returning.
          return 1;
        }
    }
  while (dir != NULL)
    {
      char fullPath[1000];
      // call memeset and set them to 0s
      memset (fullPath, 0, sizeof (fullPath));
      snprintf (fullPath, sizeof (fullPath), "%s/%s", dir, cmdline);
      if (access (fullPath, X_OK) == 0)
        {
          printf ("%s\n", fullPath);
          if (!printAll)
            break; // If not printing all locations, break on the first one
                   // found
        }
      dir = strtok (NULL, ":"); // Get next directory in $PATH
    }

  free (pathCopy); // Free the allocated memory
  return 0;
}
