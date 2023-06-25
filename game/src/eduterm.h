#include <X11/Xlib.h>
#include <X11/Xutil.h>
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

bool term_set_size(struct PTY *pty, struct X11 *x11);

bool pt_pair(struct PTY *pty);

void x11_key(XKeyEvent *ev, struct PTY *pty);

bool x11_setup(struct X11 *x11);

bool spawn(struct PTY *pty);

int eduterm_main();
