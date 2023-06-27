#include <pty.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "safe_wrappers.h"

int main_alt(void) {
  // create stdin pipe
  int fd[2];
  pipe(fd);

  int stdin_read_fd, stdin_write_fd;
  stdin_read_fd = fd[0];
  stdin_write_fd = fd[1];
  printf("stdin_read_fd: %d ; stdin_write_fd: %d\n", stdin_read_fd,
         stdin_write_fd);

  // openpty
  int master, slave;
  if (openpty(&master, &slave, NULL, NULL, NULL) == -1) {
    exit(EXIT_FAILURE);
  }
  printf("master: %d ; slave: %d\n", master, slave);

  // get the name of slave
  char name[2048] = {0};
  if (ttyname_r(slave, name, sizeof(name) - 1) != 0) {
    exit(EXIT_FAILURE);
  }
  printf("slave: %s\n", name);

  pid_t pid = fork();
  printf("pid: %d (after fork)\n", pid);

  switch (pid) {
  case 0: {
    // sid - Else TIOCSCTTY fails
    if (setsid() == -1) {
      printf("setsid() in child process failed");
      exit(EXIT_FAILURE);
    }

    int tfd = safe_open(name, O_RDWR | O_CLOEXEC, 0);
    printf("tfd: %d\n", tfd);
    if (tfd == -1) {
      printf("failed for tfd\n");
      exit(EXIT_FAILURE);
    }

    // On BSD open() does not establish the controlling terminal
    if (ioctl(tfd, TIOCSCTTY, 0) == -1) {
      printf("failed for TIOCSCTTY\n");
      exit(EXIT_FAILURE);
    }

    printf("before safe_close(tfd)\n");
    safe_close(tfd);
    printf("after safe_close(tfd)\n");

    if (safe_dup2(slave, STDOUT_FILENO) == -1) {
      printf("dup2() failed for fd number 1\n");
      exit(EXIT_FAILURE);
    }
    printf("after first dup2\n");

    if (safe_dup2(slave, STDERR_FILENO) == -1) {
      printf("dup2() failed for fd number 2\n");
      exit(EXIT_FAILURE);
    }
    printf("after second dup2\n");

    printf("stdin_read_fd: %d\n", stdin_read_fd);
    if (stdin_read_fd > -1) {
      if (safe_dup2(stdin_read_fd, STDIN_FILENO) == -1) {
        printf("dup2() failed for fd number 0\n");
        exit(EXIT_FAILURE);
      }
      safe_close(stdin_read_fd);
      safe_close(stdin_write_fd);
    } else {
      if (safe_dup2(slave, STDIN_FILENO) == -1) {
        printf("dup2() failed for fd number 0\n");
        exit(EXIT_FAILURE);
      }
    }
    safe_close(slave);
    safe_close(master);

    // Say we done
    printf("Done.\n");
  }

  case -1: {
    exit(EXIT_FAILURE);
    break;
  }

  default:
    break;
  }
  return 0;
}
