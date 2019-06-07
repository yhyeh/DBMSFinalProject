#ifndef HASH_H
#define HASH_H

#define HASH_ENTRY_LEN 125000

typedef struct Hash_Entry {
  unsigned int* uid_buf;  // user id array
  size_t uid_buf_len;     // user id array len
} Hash_t;

#endif
