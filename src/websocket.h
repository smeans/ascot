#ifndef WEBSOCKET_INCLUDED
#define WEBSOCKET_INCLUDED

#include "asys.h"

#define WEBSOCKET_MAGIC   "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

/*
Frame format:
​​
      0                   1                   2                   3
      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     +-+-+-+-+-------+-+-------------+-------------------------------+
     |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
     |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
     |N|V|V|V|       |S|             |   (if payload len==126/127)   |
     | |1|2|3|       |K|             |                               |
     +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
     |     Extended payload length continued, if payload len == 127  |
     + - - - - - - - - - - - - - - - +-------------------------------+
     |                               |Masking-key, if MASK set to 1  |
     +-------------------------------+-------------------------------+
     | Masking-key (continued)       |          Payload Data         |
     +-------------------------------- - - - - - - - - - - - - - - - +
     :                     Payload Data continued ...                :
     + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
     |                     Payload Data continued ...                |
     +---------------------------------------------------------------+
*/

// 81 81 B9 FC D3 7E D8

typedef enum _WEBSOCKET_OPCODES {continuation_frame=0, text_frame=1, binary_frame=2,
    connection_close_frame=8, ping_frame=9, pong_frame=10} WEBSOCKET_OPCODES;
const char *WEBSOCKET_OPCODE_NAMES[] = {"continuation_frame", "text_frame", "binary_frame",
  "unk-3", "unk-4", "unk-5", "unk-6", "unk-7", "connection_close_frame", "ping_frame", "pong_frame"};

#pragma pack(push, 1)
typedef struct _WEBSOCKET_FRAME_HEADER {
  uint opcode:4;
  uint RSV3:1;
  uint RSV2:1;
  uint RSV1:1;
  uint FIN:1;
  uint payload_len:7;
  uint mask:1;
} WEBSOCKET_FRAME_HEADER, *PWEBSOCKET_FRAME_HEADER;
#pragma pack(pop)

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

void websocket_dump_frame(PWEBSOCKET_FRAME_HEADER pwf) {
  fprintf(stderr, "FIN: 0x%x\r\n", pwf->FIN);
  fprintf(stderr, "RSV1: 0x%x\r\n", pwf->RSV1);
  fprintf(stderr, "RSV2: 0x%x\r\n", pwf->RSV2);
  fprintf(stderr, "RSV3: 0x%x\r\n", pwf->RSV3);
  fprintf(stderr, "opcode: 0x%x (%s)\r\n", pwf->opcode, WEBSOCKET_OPCODE_NAMES[pwf->opcode]);
  fprintf(stderr, "mask: 0x%x\r\n", pwf->mask);
  fprintf(stderr, "payload_len: 0x%x\r\n", pwf->payload_len);
}

void websocket_processConnection(int connfd) {
  websocket_writeHeader(connfd);

  WEBSOCKET_FRAME_HEADER wf;

  byte ab[1024];
  uint cb;


  while ((cb = read(connfd, &wf, sizeof(wf))) == sizeof(wf)) {
    websocket_dump_frame(&wf);

    u64 pl = wf.payload_len;

    switch (pl) {
      case 126: {
        read(connfd, &pl, 2);
      } break;

      case 127: {
        read(connfd, &pl, 8);
      } break;
    }

    u32 mask;
    pbyte pbm = (pbyte)&mask;

    if (wf.mask) {
      read(connfd, &mask, sizeof(mask));
    }
    fprintf(stderr, "mask: %x sizeof: %ld\n", mask, sizeof(pl));

    int i = 0;
    while (i < pl && (cb = read(connfd, ab, min(sizeof(ab), pl)))) {
      int j;
      for (j = 0; j < cb; j++) {
        ab[j] ^= pbm[(i+j)%4];
      }

      i += cb;

      util_dump_hex(stderr, ab, cb);
    }
  }

  fprintf(stderr, "websocket connection closed\n");
}

#endif
#endif
