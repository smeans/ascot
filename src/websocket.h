#ifndef WEBSOCKET_INCLUDED
#define WEBSOCKET_INCLUDED

#define WEBSOCKET_MAGIC   "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
void websocket_processLine(char *line);
void websocket_processConnection(int connfd);

#endif

#ifdef COMPILE
#ifndef WEBSOCKET_COMPILED
#define WEBSOCKET_COMPILED

#include <string.h>

char server_handshake[base64len(SHA1HashSize)+1];
char client_protocol[129];

void websocket_processLine(char *line) {
  char *pk = line;
  char *pc = strchr(line, ':');
  assert(pc);

  *pc = '\0';
  char *pv = (char *)skipspaces(pc + 1);

  fprintf(stderr, "websocket_processLine:'%s'=>'%s'\r\n", pk, pv);

  if (!stricmp(pk, "Sec-WebSocket-Key")) {
    SHA1Context s1c;
    SHA1Reset(&s1c);
    SHA1Input(&s1c, (const uint8_t *)pv, strlen(pv));
    SHA1Input(&s1c, (const uint8_t *)WEBSOCKET_MAGIC, strlen(WEBSOCKET_MAGIC));

    uint8_t digest[SHA1HashSize];
    SHA1Result(&s1c, digest);
    int cb = base64Encode(digest, sizeof(digest), (pbyte)server_handshake, sizeof(server_handshake));
    server_handshake[cb] = '\0';
  } else if (!stricmp(pk, "Sec-WebSocket-Protocol")) {
    strncpy(client_protocol, pv, sizeof(client_protocol)-1);
  }
}

void websocket_writeHeader(int connfd) {
  dprintf(connfd, "HTTP/1.1 101 Switching Protocols\r\n");
  dprintf(connfd, "Upgrade: websocket\r\n");
  dprintf(connfd, "Connection: Upgrade\r\n");
  dprintf(connfd, "Sec-WebSocket-Accept: %s\r\n", server_handshake);
  if (strlen(client_protocol)) {
    dprintf(connfd, "Sec-WebSocket-Protocol: %s\r\n", client_protocol);
  }

  dprintf(connfd, "\r\n");
}

void websocket_processConnection(int connfd) {
  websocket_writeHeader(connfd);
}

#endif
#endif
