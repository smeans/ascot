#ifndef WEBSOCKET_INCLUDED
#define WEBSOCKET_INCLUDED

#define WEBSOCKET_MAGIC   "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
void websocket_processLine(char *line);
void websocket_processConnection(int connfd);

#endif

#ifdef COMPILE
#ifndef WEBSOCKET_COMPILED
#define WEBSOCKET_COMPILED

void websocket_processLine(char *line) {
  char *pk = line;
  char *pc = strchr(line, ':');
  assert(pc);

  *pc = '\0';
  char *pv = pc + 1;

  fprintf(stderr, "websocket_processLine:'%s'=>'%s'\r\n", pk, pv);
}

void websocket_processConnection(int connfd) {

}

#endif
#endif
