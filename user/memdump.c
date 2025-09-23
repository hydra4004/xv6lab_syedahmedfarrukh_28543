#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUFSIZE 512

void memdump(char *format, char *addr);

int
main(int argc, char *argv[])
{
  char buffer[BUFSIZE];
  int bytes_read;

  if (argc == 1) {
    // Run demo cases if no format is passed
    printf("Demo 1:\n");
    int nums[2] = {61810, 2025};
    memdump("ii", (char*)nums);

    printf("Demo 2:\n");
    memdump("S", "a string");

    printf("Demo 3:\n");
    char *txt = "another";
    memdump("s", (char*)&txt);

    struct sample {
      char *pstr;
      int value1;
      short value2;
      char onechar;
      char text[8];
    } record;

    record.pstr = "hello";
    record.value1 = 1819438967;
    record.value2 = 100;
    record.onechar = 'z';
    strcpy(record.text, "xyzzy");

    printf("Demo 4:\n");
    memdump("pihcS", (char*)&record);

    printf("Demo 5:\n");
    memdump("sccccc", (char*)&record);

    exit(0);
  }

  // Use stdin if a format string is supplied
  char *format = argv[1];
  bytes_read = read(0, buffer, sizeof(buffer));
  if (bytes_read < 0) {
    fprintf(2, "memdump: input read failed\n");
    exit(1);
  }
  memdump(format, buffer);
  exit(0);
}

void
memdump(char *format, char *addr)
{
  while (*format) {
    switch (*format) {
      case 'i': { // 32-bit int
        int n32 = *(int*)addr;
        printf("%d\n", n32);
        addr += sizeof(int);
        break;
      }
      case 'p': { // 64-bit int (hex)
        uint64 n64 = *(uint64*)addr;
        printf("%lx\n", n64);
        addr += sizeof(uint64);
        break;
      }
      case 'h': { // 16-bit int
        short n16 = *(short*)addr;
        printf("%d\n", n16);
        addr += sizeof(short);
        break;
      }
      case 'c': { // single char
        char ch = *addr;
        printf("%c\n", ch);
        addr += sizeof(char);
        break;
      }
      case 's': { // pointer to C-string
        char *sp = *(char**)addr;
        if (sp)
          printf("%s\n", sp);
        else
          printf("(null)\n");
        addr += sizeof(char*);
        break;
      }
      case 'S': { // inline C-string
        char *str = (char*)addr;
        printf("%s\n", str);
        addr += strlen(str) + 1;
        break;
      }
      default:
        printf("Unknown format specifier: %c\n", *format);
        break;
    }
    format++;
  }
}
