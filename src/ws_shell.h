#ifndef WS_SHELL_INCLUDED
#define WS_SHELL_INCLUDED

void shell_processHeaderRead(int connfd, PWEBSOCKET_FRAME_HEADER_COMPLETE pfhc);
void shell_processPayloadChunkRead(int connfd, u64 cbTotalRead, pbyte pbchunk, u32 cbChunk);
void shell_killBash();

#ifdef COMPILE
#ifndef WS_SHELL_COMPILED
#define WS_SHELL_COMPILED

#include <spawn.h>
#include <unistd.h>
#include <signal.h>

#include "websocket.h"

pid_t bashPID;
int bashWriteFD;
int bashReadFD;
char *bashArgv[] = {"bash", NULL};

void shell_spawnBash() {
  int pfda[2];

  posix_spawn_file_actions_t child_fd_actions;

  posix_spawn_file_actions_init (&child_fd_actions);

  pipe(pfda);
  posix_spawn_file_actions_addclose(&child_fd_actions, pfda[1]);
  posix_spawn_file_actions_adddup2 (&child_fd_actions, pfda[0], STDIN_FILENO);
  posix_spawn_file_actions_addclose(&child_fd_actions, pfda[0]);
  bashWriteFD = pfda[1];

  pipe(pfda);
  posix_spawn_file_actions_addclose(&child_fd_actions, pfda[0]);
  posix_spawn_file_actions_adddup2 (&child_fd_actions, pfda[1], STDOUT_FILENO);
  posix_spawn_file_actions_adddup2 (&child_fd_actions, pfda[1], STDERR_FILENO);
  posix_spawn_file_actions_addclose(&child_fd_actions, pfda[1]);
  bashReadFD = pfda[0];

  fprintf(stderr, "about to spawn...");
  int ret;
  if (ret = posix_spawnp(&bashPID, "bash", &child_fd_actions, NULL, bashArgv, NULL)) {
    fprintf(stderr, "posix_spawnp returned %d\n", ret);
    exit(ret);
  }

  fprintf(stderr, "bashPID %d\n", bashPID);
}

void shell_killBash() {
  dprintf(bashWriteFD, "exit");
  if (bashPID) {
    kill(bashPID, SIGTERM);
  }
}

void shell_writeToBash(pbyte pbchunk, u32 cbChunk) {
  if (!bashPID) {
    shell_spawnBash();
  }

  write(bashWriteFD, pbchunk, cbChunk);
}

void shell_readFromBash(int connfd) {
  byte ab[1024];
  int cb = read(bashReadFD, ab, sizeof(ab));

  write(STDOUT_FILENO, ab, cb);
  fprintf(stderr, "read %d bytes from shell\n", cb);
  websocketWriteToClient(connfd, ab, cb);
}

bool shell_waitForData(int connfd) {
  struct pollfd pfd[2];
  pfd[0].fd = connfd;
  pfd[0].events = POLLIN;

  pfd[1].fd = bashReadFD;
  pfd[1].events = POLLIN;

  fprintf(stderr, "polling...");
  int ret = poll(pfd, sizeof(pfd)/sizeof(*pfd), -1);
  fprintf(stderr, "returned: %d\n", ret);

  if (pfd[1].revents & POLLIN) {
    shell_readFromBash(connfd);
  }

  return true;
}

void shell_processHeaderRead(int connfd, PWEBSOCKET_FRAME_HEADER_COMPLETE pfhc) {
  fprintf(stderr, "shell: processHeaderRead: %llu\n", pfhc->true_payload_length);
  WEBSOCKET_FRAME_HEADER_COMPLETE wfhc;
  memcpy(&wfhc, pfhc, sizeof(wfhc));
  wfhc.wfh.mask = 0;
  writeFrameHeaderComplete(connfd, &wfhc);
}

void shell_processPayloadChunkRead(int connfd, u64 cbTotalRead, pbyte pbchunk, u32 cbChunk) {
  fprintf(stderr, "shell: processPayloadChunkRead: cbTotalRead: %llu\tcbChunk: %u\n", cbTotalRead, cbChunk);
  write(connfd, pbchunk, cbChunk);
  shell_writeToBash(pbchunk, cbChunk);
}

#endif
#endif

#endif
