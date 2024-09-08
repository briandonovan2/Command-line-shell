#include <dirent.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// You may assume that lines are no longer than 1024 bytes
#define LINELEN 1024

int
main (int argc, char *argv[])
{
  bool show_hidden = false;
  bool show_size = false;
  char *directory = ".";

  int opt;
  while ((opt = getopt (argc, argv, "as")) != -1)
    {
      switch (opt)
        {
        case 's':
          show_size = true;
          break;
        case 'a':
          show_hidden = true;
          break;
        default:
          exit (EXIT_FAILURE);
        }
    }
  if (optind < argc)
    {
      directory = argv[optind];
    }
  else
    {
      directory = ".";
    }
  DIR *dir = opendir (directory);

  struct dirent *entry;
  while (entry = readdir (dir))
    {
      if (!show_hidden && entry->d_name[0] == '.')
        {
          continue;
        }

      char full_path[LINELEN];
      snprintf (full_path, LINELEN, "%s/%s", directory, entry->d_name);

      if (show_size)
        {
          struct stat st;
          if (stat (full_path, &st) == 0)
            {
              printf ("%s (%ld bytes)\n", entry->d_name, (long)st.st_size);
            }
          else
            {
              printf ("%s\n", entry->d_name);
            }
        }
      else
        {
          printf ("%s\n", entry->d_name);
        }
    }
  closedir (dir);
  return 0;
}

static void
usage (void)
{
  printf ("ls, lsit directory contents\n");
  printf ("usage: ls [FLAG ...] [DIR]\n");
  printf ("FLAG is one or more of :\n");
  printf ("-a      list all files (even hidden ones)\n");
  printf ("-s      list file sizes:\n");
  printf ("If no DIR specified, list current directory contents\n");
}