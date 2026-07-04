#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

#define MAXLINE 512

// Read one newline-delimited line from standard input.
// Return 1 for a line, 0 for EOF, and -1 for an error or overlong line.
int
readline(char *buf, int size)
{
  int length = 0;
  int n;
  char c;

  while(1){
    n = read(0, &c, 1);
    if(n < 0)
      return -1;
    if(n == 0){
      if(length == 0)
        return 0;
      buf[length] = '\0';
      return 1;
    }
    if(c == '\n'){
      buf[length] = '\0';
      return 1;
    }
    if(length >= size - 1)
      return -1;
    buf[length++] = c;
  }
}

// Append the words from line to the fixed arguments and execute them.
int
execute_line(int argc, char *argv[], char *line)
{
  char *args[MAXARG];
  char *p;
  int arg_count = 0;
  int i;
  int pid;

  for(i = 1; i < argc; i++){
    if(arg_count >= MAXARG - 1){
      fprintf(2, "xargs: too many arguments\n");
      return -1;
    }
    args[arg_count++] = argv[i];
  }

  p = line;
  while(*p != '\0'){
    while(*p == ' ' || *p == '\t')
      p++;
    if(*p == '\0')
      break;

    if(arg_count >= MAXARG - 1){
      fprintf(2, "xargs: too many arguments\n");
      return -1;
    }
    args[arg_count++] = p;

    while(*p != '\0' && *p != ' ' && *p != '\t')
      p++;
    if(*p != '\0')
      *p++ = '\0';
  }
  args[arg_count] = 0;

  pid = fork();
  if(pid < 0){
    fprintf(2, "xargs: fork failed\n");
    return -1;
  }
  if(pid == 0){
    exec(args[0], args);
    fprintf(2, "xargs: exec %s failed\n", args[0]);
    exit(1);
  }

  wait(0);
  return 0;
}

int
main(int argc, char *argv[])
{
  char line[MAXLINE];
  int status;

  if(argc < 2){
    fprintf(2, "Usage: xargs command [args...]\n");
    exit(1);
  }

  while((status = readline(line, sizeof(line))) == 1){
    if(execute_line(argc, argv, line) < 0)
      exit(1);
  }

  if(status < 0){
    fprintf(2, "xargs: failed to read input line\n");
    exit(1);
  }

  exit(0);
}
