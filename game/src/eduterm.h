#include <ctype.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

struct PTY {
  int master, slave;
};

struct X11 {
  int fd;

  char *buf;
  int buf_w, buf_h;
  int buf_x, buf_y;
};

int loop_once(struct PTY *pty, struct X11 *x11, int i, int maxfd,
              fd_set *readable, char *buf, bool just_wrapped);

int eduterm_setup(struct PTY *pty, struct X11 *x11);
