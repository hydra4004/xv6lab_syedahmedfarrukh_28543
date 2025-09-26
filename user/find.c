#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

#define BUF_SIZE 512


int regex_here(char*, char*);
int regex_star(int, char*, char*);

int regex_match(char *pattern, char *text)
{
  if(pattern[0] == '^')
    return regex_here(pattern + 1, text);

  do {
    if(regex_here(pattern, text))
      return 1;
  } while(*text++ != '\0');
  return 0;
}

int
regex_here(char *pattern, char *text)
{
  if(pattern[0] == '\0')
    return 1;

  if(pattern[1] == '*')
    return regex_star(pattern[0], pattern + 2, text);

  if(pattern[0] == '$' && pattern[1] == '\0')
    return *text == '\0';

  if(*text != '\0' && (pattern[0] == '.' || pattern[0] == *text))
    return regex_here(pattern + 1, text + 1);

  return 0;
}

int
regex_star(int c, char *pattern, char *text)
{
  do {
    if(regex_here(pattern, text))
      return 1;
  } while(*text != '\0' && (*text++ == c || c == '.'));
  return 0;
}




char* get_filename(char *path)
{
  char *p = path + strlen(path);
  while(p >= path && *p != '/')
    p--;
  return p + 1;
}

void
do_find(char *path, char *pattern, int with_exec, char *exec_cmd[], int exec_argc)
{
  char buf[BUF_SIZE], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if(st.type == T_FILE){
    if(regex_match(pattern, get_filename(path))){
      if(with_exec){
        char *args[MAXARG];
        for(int i = 0; i < exec_argc; i++)
          args[i] = exec_cmd[i];
        args[exec_argc] = path;
        args[exec_argc+1] = 0;

        if(fork() == 0){
          exec(exec_cmd[0], args);
          fprintf(2, "exec %s failed\n", exec_cmd[0]);
          exit(1);
        }
        wait(0);
      } else {
        printf("%s\n", path);
      }
    }
  }

  else if(st.type == T_DIR){
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      close(fd);
      return;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;

      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;

      do_find(buf, pattern, with_exec, exec_cmd, exec_argc);
    }
  }

  close(fd);
}

int main(int argc, char *argv[])
{
  if(argc < 3){
    fprintf(2, "Usage: find <path> <pattern> [-exec command ...]\n");
    exit(1);
  }

  int run_exec = 0;
  char *command[MAXARG];
  int command_argc = 0;

  // Parse for "-exec"
  for(int i = 3; i < argc; i++){
    if(strcmp(argv[i], "-exec") == 0){
      run_exec = 1;
      for(int j = i+1; j < argc; j++)
        command[command_argc++] = argv[j];
      break;
    }
  }

  do_find(argv[1], argv[2], run_exec, command, command_argc);
  exit(0);
}
