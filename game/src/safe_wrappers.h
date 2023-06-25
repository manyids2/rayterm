/*
 * Copyright (C) 2021 Kovid Goyal <kovid at kovidgoyal.net>
 *
 * Distributed under terms of the GPL3 license.
 */

#pragma once
#include <asm-generic/errno-base.h>
#include <errno.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

static inline int safe_open(const char *path, int flags, mode_t mode) {
  while (true) {
    int fd = open(path, flags, mode);
    if (fd == -1 && errno == EINTR)
      continue;
    return fd;
  }
}

static inline int safe_shm_open(const char *path, int flags, mode_t mode) {
  while (true) {
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
  while ((ret = dup2(a, b)) < 0 && errno == EINTR)
    ;
  return ret;
}
