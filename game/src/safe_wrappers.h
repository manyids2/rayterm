/*
 * Copyright (C) 2021 Kovid Goyal <kovid at kovidgoyal.net>
 *
 * Distributed under terms of the GPL3 license.
 */

#pragma once
#include <errno.h>

#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

static inline int safe_open(const char *path, int flags, mode_t mode) {
  while (1) {
    int fd = open(path, flags, mode);
    if (fd == -1 && errno == EINTR)
      continue;
    return fd;
  }
}

static inline int safe_shm_open(const char *path, int flags, mode_t mode) {
  while (1) {
    int fd = shm_open(path, flags, mode);
    if (fd == -1 && errno == EINTR)
      continue;
    return fd;
  }
}

static inline void safe_close(int fd) {
  while (close(fd) != 0 && errno == EINTR)
    ;
}

static inline int safe_dup2(int a, int b) {
  int ret;
  printf("inside safe_dup2\n");
  ret = dup2(a, b);
  printf("ret: %d\n", ret);
  while ((ret = dup2(a, b)) < 0 && errno == EINTR)
    printf("a: %d, b: %d\n", a, b);
  return ret;
}
