#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

// You may assume that lines are no longer than 1024 bytes
#define LINELEN 1024

int main(int argc, char *argv[])
{
  int num_lines = 5;
  FILE *file = stdin;

  int opt;
  while ((opt = getopt(argc, argv, "n:")) != -1)
  {
    switch (opt)
    {
    case 'n':
      num_lines = atoi(optarg);
      break;
    default:
      exit(EXIT_FAILURE);
    }
  }

  file = fopen(argv[optind], "r");

  char line[LINELEN];
  int line_count = 0;
  while (line_count < num_lines && fgets(line, sizeof(line), file) != NULL)
  {
    printf("%s", line);
    line_count++;
  }

  return 0;
}

static void
usage(void)
{
  printf("head, prints the first few lines of a file\n");
  printf("usage: head [FLAG] FILE\n");
  printf("FLAG can be:\n");
  printf("  -n N     show the first N lines (default 5)\n");
  printf("If no FILE specified, read from STDIN\n");
}
