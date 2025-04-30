#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#ifndef MAXPATHLEN
#define MAXPATHLEN 260
#endif
#include <io.h>
#else
#include <strings.h>
#include <sys/param.h>
#include <unistd.h> // Windowsの場合は外す、UNIXなら必要
#endif
#include <sys/types.h>

int main() {
  uint8_t buf[256];
  int len;

  if ((len = read(0, buf, 128)) < 0)
    exit(1);
  while ((len = read(0, buf, sizeof(buf))) > 0)
    write(1, buf, len);
  exit(0);
}
