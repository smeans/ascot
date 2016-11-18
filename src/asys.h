#ifndef ASYS_H_INCLUDED
#define ASYS_H_INCLUDED

typedef void *pvoid;
typedef unsigned char byte;
typedef unsigned char *pbyte;
typedef unsigned int uint;
typedef uint *puint;

typedef unsigned short int u16;
typedef u16 *pu16;
typedef unsigned long int u32;
typedef u32 *pu32;
typedef unsigned long long int u64;
typedef u64 *pu64;

typedef _Bool bool;
#define true    1
#define false   0

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)

const char *strext(const char *fname);
bool fexists(const char *fname);

int base64Encode(const pbyte in, u32 in_len, pbyte out, u32 out_len);
const char *skipspaces(const char *ps);
int stricmp(const char *a, const char *b);
#endif

#ifdef COMPILE
#ifndef ASYS_H_COMPILED
#define ASYS_H_COMPILED

#include <ctype.h>

const char *strext(const char *fname) {
  const char *pe = &fname[strlen(fname)-1];

  while (pe >= fname && *pe != '.') {
    pe--;
  }

  return pe >= fname ? (pe+1) : NULL;
}

bool fexists(const char *fname) {
  struct stat st;
  return stat(fname, &st) == 0;
}

#define _b64(cb) ((int)(4.0*((cb)/3.0)))
#define base64len(cb) (_b64(cb) + ((_b64(cb) % 4) ? 4-(_b64(cb) % 4) : 0))
const char *BASE64_CODES = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

int base64Encode(const pbyte in, u32 in_len, pbyte out, u32 out_len) {
    if (out_len < base64len(in_len)) {
      return -1;
    }
    int b;
    int i;
    pbyte po = out;

    for (i = 0; i < in_len && po < &out[out_len]; i += 3) {
        b = (in[i] & 0xfc) >> 2;
        *po++ = BASE64_CODES[b];
        b = (in[i] & 0x03) << 4;
        if (i + 1 < in_len)      {
            b |= (in[i + 1] & 0xf0) >> 4;
            *po++ = BASE64_CODES[b];
            b = (in[i + 1] & 0x0f) << 2;
            if (i + 2 < in_len)  {
                b |= (in[i + 2] & 0xc0) >> 6;
                *po++ = BASE64_CODES[b];
                b = in[i + 2] & 0x3f;
                *po++ = BASE64_CODES[b];
            } else {
                *po++ = BASE64_CODES[b];
                *po++ = '=';
            }
        } else {
            *po++ = BASE64_CODES[b];
            *po++ = '=';
            *po++ = '=';
        }
    }

    return po - out;
}

const char *skipspaces(const char *ps) {
  while (isspace(*ps)) {
    ps++;
  }

  return ps;
}

int stricmp(const char *a, const char *b) {
  int ca, cb;
  do {
     ca = (unsigned char) *a++;
     cb = (unsigned char) *b++;
     ca = tolower(toupper(ca));
     cb = tolower(toupper(cb));
   } while (ca == cb && ca != '\0');
   return ca - cb;
}
#endif
#endif
