#include <pty.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {

  int master;
  int slave;
  openpty(&master, &slave, NULL, NULL, NULL);

  // Temporarily redirect stdout to the slave, so that the command executed in
  // the subprocess will write to the slave.
  int _stdout = dup(STDOUT_FILENO);
  dup2(slave, STDOUT_FILENO);

  pid_t pid = fork();
  if (pid == 0) {
    // We use
    //
    //     head -c $output_size /dev/zero
    //
    // as the command for our demo.
    const char *argv[] = {"ls", NULL};
    execvp(argv[0], argv);
  }

  fd_set rfds;
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;

  char buf[4097];
  ssize_t size;
  size_t count = 0;

  // Read from master as we wait for the child process to exit.
  //
  // We don't wait for it to exit and then read at once, because otherwise the
  // command being executed could potentially saturate the slave's buffer and
  // stall.
  while (1) {
    if (waitpid(pid, NULL, WNOHANG) == pid) {
      break;
    }
    FD_ZERO(&rfds);
    FD_SET(master, &rfds);
    if (select(master + 1, &rfds, NULL, NULL, &tv)) {
      size = read(master, buf, 4096);
      buf[size] = '\0';
      count += size;
    }
  }

  // Child process terminated; we flush the output and restore stdout.
  fsync(STDOUT_FILENO);
  dup2(_stdout, STDOUT_FILENO);

  // Read until there's nothing to read, by which time we must have read
  // everything because the child is long gone.
  while (1) {
    FD_ZERO(&rfds);
    FD_SET(master, &rfds);
    if (!select(master + 1, &rfds, NULL, NULL, &tv)) {
      // No more to read.
      break;
    }
    size = read(master, buf, 4096);
    buf[size] = '\0';
    count += size;
  }

  // Close both ends of the pty.
  close(master);
  close(slave);

  // Say we done
  printf("Buf:\n%s\n", buf);
  printf("Done. ( %zu chars read )\n", count);
}
