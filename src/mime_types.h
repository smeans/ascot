#ifndef MIME_TYPES_INCLUDED


#endif

#ifdef COMPILE
#include <ctype.h>

#ifndef MIME_TYPES_COMPILED

#include "mime_types_data.c"

const char *getMimeType(const char *ext) {
  if (!ext) {
    return NULL;
  }
  
  int i;
  for (i = 0; i < sizeof(mime_types)/sizeof(*mime_types); i += 2) {
    const char *pel = mime_types[i+1];
    char *f = strstr(pel, ext);
    if (f) {
      if (f == pel || isspace(f[-1])) {
        f = &f[strlen(ext)];

        if (*f == '\0' || isspace(*f)) {
          return mime_types[i];
        }
      }
    }
  }

  return NULL;
}

#endif
#endif
