#include "asys.h"
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <assert.h>
#include <time.h>

#define COMPILE
#include "mime_types.h"
#include "asys.h"

void dumpTextBuffer(const char *pb, int cb) {
  int i;
  for (i = 0; i < cb; i++) {
    if (pb[i] >= 32) {
      putc(pb[i], stderr);
    } else {
      fprintf(stderr, "(0x%.2x)", pb[i]);
      if (pb[i] == 0x0a) {
        putc(pb[i], stderr);
      }
    }
  }
}

void dumpHttpRequest(int connfd) {
  byte rb[4096];
  int cb;
  bool eoh = false;

  while (!eoh && (cb = read(connfd, rb, sizeof(rb)-1))) {
    dumpTextBuffer((const char *)rb, cb);
    rb[cb] = '\0';
    eoh = !!strstr((const char *)rb, "\r\n\r\n");
  }
}

void writeFileToSocket(int connfd, char *fname) {
  FILE *f = fopen(fname, "r");
  byte rb[4096];

  while (!feof(f)) {
    size_t cb = fread(rb, 1, sizeof(rb), f);
    if (cb > 0) {
      write(connfd, rb, cb);
    }
  }

  fclose(f);
}

int listenfd = 0, connfd = 0;
struct sockaddr_in serv_addr;
bool isChild;

void cleanup() {
  if (connfd) {
    close(connfd);
    connfd = 0;
  }

  if (listenfd) {
    shutdown(listenfd, SHUT_RDWR);
    listenfd = 0;
  }
}

void sigint_handler() {
  cleanup();
  exit(0);
}

typedef enum _methods {UNK, GET, POST} methods;
methods requestMethod;

#pragma pack(push, 1)
struct _request_url {
  char url_path[9]; // !!!HACK!!! exactly 9 characters to match "resources" path prefix
  char url[2048];
} rurl;
#pragma pack(pop)

void readHttpHeader() {
  byte rb[4096];
  int cb;
  bool firstBlock = true;
  bool eoh = false;

  while (!eoh && (cb = read(connfd, rb, sizeof(rb)-1))) {

    rb[cb] = '\0';
    eoh = !!strstr((const char *)rb, "\r\n\r\n");

    if (firstBlock) {
      char *ss = (char *)rb;
      char *se = strchr((const char *)ss, ' ');
      assert(se);
      *se = '\0';
      if (!strcmp(ss, "GET")) {
        requestMethod = GET;
      } else if (!strcmp(ss, "POST")) {
        requestMethod = POST;
      } else {
        requestMethod = UNK;
      }

      ss = ++se;
      se = strchr((const char *)se, ' ');
      assert(se);
      *se = '\0';
      strncpy(rurl.url, ss, sizeof(rurl.url)-1);
      rurl.url[sizeof(rurl.url)-1] = '\0';

      firstBlock = false;
    }
  }
}

void processConnection() {
  readHttpHeader();
  fprintf(stderr, "request method: %d\turl: %s\n", requestMethod, rurl.url);

  const char *pmt = getMimeType(strext(rurl.url));

  if (!strcmp("/", rurl.url)) {
    writeFileToSocket(connfd, "resources/index.html");
  } else if (fexists(rurl.url_path)) {
    writeFileToSocket(connfd, rurl.url_path);
  } else {
    writeFileToSocket(connfd, "resources/404.html");
  }

  close(connfd);
  connfd = 0;
}

int main(int argc, char **argv)
{
    atexit(cleanup);
    signal(SIGINT, sigint_handler);

    strncpy(rurl.url_path, "resources", sizeof(rurl.url_path));

    char sendBuff[1025];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, 10);

    while (true) {
      connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

      int pid = fork();

      if (pid < 0) {
        perror("error on fork");

        exit(1);
      }

      isChild = pid == 0;

      if (isChild) {
        close(listenfd);
        processConnection();
        exit(0);
      } else {
        close(connfd);
        connfd = 0;
      }
    }

    shutdown(listenfd, SHUT_RDWR);
    listenfd = 0;
}
