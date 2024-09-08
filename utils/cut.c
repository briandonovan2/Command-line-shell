#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

// You may assume that lines are no longer than 1024 bytes
#define LINELEN 1024

static void usage(void);

int main(int argc, char *argv[])
{
 char *filename = NULL;
 char *delimiter = " ";
 int field = 1;

 int opt;
 while ((opt = getopt(argc, argv, "d:f:")) != -1)
 {
    switch (opt)
    {
      case 'd':
        delimiter = optarg;
        break;
      case 'f':
        char *endptr;
        field = strtol(optarg, &endptr, 10);
        if (endptr == optarg || field < 1) {
          fprintf(stderr, "No digits were found\n");
          usage();
          return EXIT_FAILURE;
    }
      break;
      default:
        usage();
        return EXIT_FAILURE;
    }
 }

 if (optind < argc)
    filename = argv[optind];

 FILE *file;
 if (filename == NULL)
    file = stdin;
 else
 {
    file = fopen(filename, "r");
    if (file == NULL)
    {
      perror("fopen");
      return EXIT_FAILURE;
    }
 }

 char line[LINELEN];
 while (fgets(line, LINELEN, file))
 {
    char *pos;
    if ((pos=strchr(line, '\n')) != NULL)
        *pos = '\0';
    char *token = strtok(line, delimiter);
    int count = 1;
    while (token != NULL)
    {
      if (count == field)
      {
        printf("%s\n", token);
        break;
      }
      token = strtok(NULL, delimiter);
      count++;
    }
     //If field is out of range for this line, just continue to next line
    if (token == NULL)
    {
      printf("\n");
      continue;
    }
  }

 if (filename != NULL)
    fclose(file);

 return EXIT_SUCCESS;
}

static void
usage(void)
{
  printf("cut, splits each line based on a delimiter\n");
  printf("usage: cut [FLAG] FILE\n");
  printf("FLAG can be:\n");
  printf("  -d C     split each line based on the character C (default ' ')\n");
  printf("  -f N     print the Nth field (1 is first, default 1)\n");
  printf("If no FILE specified, read from STDIN\n");
}
