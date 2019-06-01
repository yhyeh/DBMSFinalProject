#ifndef LIKE_H
#define LIKE_H
#include "Command.h"

enum like_fields{
  ID1,
  ID2
};

typedef struct Like {
    unsigned int id1;
    unsigned int id2;
} Like_t;

Like_t* new_Like();
Like_t* command_to_Like(Command_t *cmd);

#endif
