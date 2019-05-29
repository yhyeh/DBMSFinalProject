#ifndef DB_UTIL_LIKE_H
#define DB_UTIL_LIKE_H
#include "Command.h"
#include "Table_Like.h"
#include "Util.h"

void print_like(Like_t *user, SelectArgs_t *sel_args);
void print_likes(Table_Like_t *table, int *idxList, size_t idxListLen, Command_t *cmd);
void handle_builtin_cmd_like(Table_Like_t *table, Command_t *cmd, State_t *state);
int handle_query_cmd_like(Table_Like_t *table, Command_t *cmd);
int handle_insert_cmd_like(Table_Like_t *table, Command_t *cmd);
int handle_select_cmd_like(Table_Like_t *table, Command_t *cmd);
size_t where_check_like(Table_Like_t *table, Command_t *cmd, int* idxList);
#endif
