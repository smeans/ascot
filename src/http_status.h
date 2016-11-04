#ifndef HTTP_STATUS_INCLUDED
#define HTTP_STATUS_INCLUDED

const char *getStatusReason(int status);

#endif

#ifdef COMPILE
#ifndef HTTP_STATUS_COMPILED
#define HTTP_STATUS_COMPILED

int http_status_codes[] = {100, 101, 200, 201, 202, 203, 204, 205, 206, 300, 301, 302, 303, 304, 305, 307, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 500, 501, 502, 503, 504, 505};
const char *http_status_strings[] = {"Continue", "Switching Protocols", "OK", "Created", "Accepted", "Non-Authoritative Information", "No Content", "Reset Content", "Partial Content", "Multiple Choices", "Moved Permanently", "Found", "See Other", "Not Modified", "Use Proxy", "Temporary Redirect", "Bad Request", "Unauthorized", "Payment Required", "Forbidden", "Not Found", "Method Not Allowed", "Not Acceptable", "Proxy Authentication Required", "Request Time-out", "Conflict", "Gone", "Length Required", "Precondition Failed", "Request Entity Too Large", "Request-URI Too Large", "Unsupported Media Type", "Requested range not satisfiable", "Expectation Failed", "Internal Server Error", "Not Implemented", "Bad Gateway", "Service Unavailable", "Gateway Time-out", "HTTP Version not supported"};

const char *getStatusReason(int status) {
  for (int i = 0; i < sizeof(http_status_codes)/sizeof(*http_status_codes); i++) {
    if (http_status_codes[i] == status) {
      return http_status_strings[i];
    }
  }

  int gs = status/100*100;

  return status != gs ? getStatusReason(gs) : "Unknown";
}

#endif
#endif
