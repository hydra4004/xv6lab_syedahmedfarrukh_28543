#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUFSIZE 512

void dumpmem(char *format, char *addr);

int
main(int argc, char *argv[])
{
  char buffer[BUFSIZE];
  int bytes_read;

  if (argc == 1) {
    printf("Demo 1:\n");
    int numbers[2] = {61810, 2025};
    dumpmem("ii", (char*)numbers);

    printf("Demo 2:\n");
    dumpmem("S", "a string");

    printf("Demo 3:\n");
    char *word = "another";
    dumpmem("s", (char*)&word);

    struct sample {
      char *pointer;
      int first;
      short second;
      char single;
      char array[8];
    } obj;

    obj.pointer = "hello";
    obj.first = 1819438967;
    obj.second = 100;
    obj.single = 'z';
    strcpy(obj.array, "xyzzy");

    printf("Demo 4:\n");
    dumpmem("pihcS", (char*)&obj);

    printf("Demo 5:\n");
    dumpmem("sccccc", (char*)&obj);

    exit(0);
  }

  char *format = argv[1];
  bytes_read = read(0, buffer, sizeof(buffer));
  if (bytes_read < 0) {
    fprintf(2, "dumpmem: failed to read input\n");
    exit(1);
  }
  dumpmem(format, buffer);
  exit(0);
}

void
dumpmem(char *format, char *addr)
{
  while (*format) {
    switch (*format) {
      case 'i': { // 32-bit int
        int val32 = *(int*)addr;
        printf("%d\n", val32);
        addr += sizeof(int);
        break;
      }
      case 'p': { 
        uint64 val64 = *(uint64*)addr;
        printf("%lx\n", val64);
        addr += sizeof(uint64);
        break;
      }
      case 'h': { 
        short val16 = *(short*)addr;
        printf("%d\n", val16);
        addr += sizeof(short);
        break;
      }
      case 'c': { 
        char ch = *addr;
        printf("%c\n", ch);
        addr += sizeof(char);
        break;
      }
      case 's': { 
        char *sptr = *(char**)addr;
        if (sptr)
          printf("%s\n", sptr);
        else
          printf("(null)\n");
        addr += sizeof(char*);
        break;
      }
      case 'S': { 
        char *str = (char*)addr;
        printf("%s\n", str);
        addr += strlen(str) + 1;
        break;
      }
      default:
        printf("Unknown specifier: %c\n", *format);
        break;
    }
    format++;
  }
}
