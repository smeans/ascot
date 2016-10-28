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

#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)

const char *strext(const char *fname);
bool fexists(const char *fname);
#endif

#ifdef COMPILE

#ifndef ASYS_H_COMPILED
#define ASYS_H_COMPILED

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
#endif
#endif
