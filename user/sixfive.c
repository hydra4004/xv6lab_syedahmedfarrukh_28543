#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

// delimiters between numbers
char *delims = " -\r\t\n./,";
// check if character is a delimiter
int is_delim(char ch) {
  return (strchr(delims, ch) != 0);
}
void scan_file(char *path) {
  int file = open(path, O_RDONLY);
  if (file < 0) {
    fprintf(2, "error: cannot open %s\n", path);
    return;
  }
  char chbuf[1];
  char numstr[64];   // temporary buffer to store digits
  int pos = 0;
  while (read(file, chbuf, 1) == 1) {
    char ch = chbuf[0];
    if (is_delim(ch)) {
      if (pos > 0) { // finished reading one number
        numstr[pos] = '\0';
        int number = atoi(numstr);
        if (number % 5 == 0 || number % 6 == 0) {
          printf("%d\n", number);
        }
        pos = 0;
      }
    } else {
      if (pos < sizeof(numstr) - 1) {
        numstr[pos++] = ch;
      }
    }
  }
  // handle the last number if file doesn't end with delimiter
  if (pos > 0) {
    numstr[pos] = '\0';
    int number = atoi(numstr);
    if (number % 5 == 0 || number % 6 == 0) {
      printf("%d\n", number);
    }
  }

  close(file);
}
int
main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(2, "usage: sixfive <files...>\n");
    exit(1);
  }

  for (int j = 1; j < argc; j++) {
    scan_file(argv[j]);
  }
  exit(0);
}
