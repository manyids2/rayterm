#include <pty.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFSIZE 4096
#define BUFSIZE_plus_1 BUFSIZE + 1

struct PTY {
  int master, slave, temp_stdout;
  pid_t pid;
  int success;
};

struct ReadBuffer {
  // read buffers for `select`
  fd_set rfds;
  struct timeval tv;

  char buf[BUFSIZE_plus_1];
  ssize_t size;
  size_t count;
};

void print_args(int args, char **argv) {
  printf("args: %d\n", args);
  for (int i = 0; i < args; i++) {
    printf("  %d: %s\n", i, argv[i]);
  }
}

char **remove_first_arg(int args, char **argv) {
  args -= 1;
  for (int i = 0; i < args; i++) {
    argv[i] = argv[i + 1];
  }
  argv[args] = NULL;
  return argv;
}

struct PTY setup_pty(int args, char **argv) {
  struct PTY pty;
  pty.success = -1;

  if (args < 2) {
    return pty;
  }

  // Open pty
  openpty(&pty.master, &pty.slave, NULL, NULL, NULL);
  printf("After openpty\n  master: %d\n  slave: %d\n", pty.master, pty.slave);

  // Temporarily redirect stdout to the slave, so that the command executed in
  // the subprocess will write to the slave.
  pty.temp_stdout = dup(STDOUT_FILENO);
  dup2(pty.slave, STDOUT_FILENO);
  printf("After dup2\n");

  pty.pid = fork();
  if (pty.pid == 0) {
    // We use args from cmd line, terminated by NULL as { "arg1", "arg2", NULL }
    execvp(*argv, argv);
  } else {
    close(pty.master);
    close(pty.slave);
    return pty;
  }

  pty.success = 1;
  return pty;
}

void read_once(struct PTY *pty, struct ReadBuffer *rb) {
  FD_ZERO(&rb->rfds);
  FD_SET(pty->master, &rb->rfds);
  if (select(pty->master + 1, &rb->rfds, NULL, NULL, &rb->tv)) {
    rb->size = read(pty->master, rb->buf, BUFSIZE);
    rb->buf[rb->size] = '\0';
    rb->count += rb->size;
  }
}

int main(int args, char **argv) {
  if (args < 2) {
    exit(EXIT_FAILURE);
  }

  argv = remove_first_arg(args, argv);
  args -= 1;
  print_args(args, argv);

  struct PTY pty = setup_pty(args, argv);
  printf("After pty\n");
  struct ReadBuffer rb;
  rb.tv.tv_sec = 0;
  rb.tv.tv_usec = 0;

  // Read from master as we wait for the child process to exit.
  //
  // We don't wait for it to exit and then read at once, because otherwise the
  // command being executed could potentially saturate the slave's buffer and
  // stall.
  while (1) {
    if (waitpid(pty.pid, NULL, WNOHANG) == pty.pid) {
      break;
    }
    read_once(&pty, &rb);
  }

  // Child process terminated; we flush the output and restore stdout.
  fsync(STDOUT_FILENO);
  dup2(pty.temp_stdout, STDOUT_FILENO);

  // Read until there's nothing to read, by which time we must have read
  // everything because the child is long gone.
  while (1) {
    read_once(&pty, &rb);
  }

  // Close both ends of the pty.
  close(pty.master);
  close(pty.slave);

  // Say we done
  printf("Buf:\n%s\n", rb.buf);
  printf("Done. ( %zu chars read )\n", rb.count);

  return 0;
}
