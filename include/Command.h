#ifndef COMMAND_H
#define COMMAND_H

enum {
    UNRECOG_CMD,
    BUILT_IN_CMD,
    QUERY_CMD,
};

enum table_types{
  UNRECOG_TB,
  USER_TB,
  LIKE_TB,
  JOIN_TB
};

enum {
    INSERT_CMD = 100,
    SELECT_CMD,
	UPDATE_CMD = 101,
	DELETE_CMD = 99,
};

typedef struct {
    char name[256];
    int len;
    unsigned char type;
} CMD_t;

extern CMD_t cmd_list[];

typedef struct SelectArgs {
    char **fields;
    size_t fields_len;
    int offset;
    int limit;
	int where_begin;
	int where_end;
} SelectArgs_t;

typedef union {
    SelectArgs_t sel_args;
} CmdArg_t;

typedef struct Command {
    unsigned char type;
	unsigned char type_table;
    char **args;
    size_t args_len;
    size_t args_cap;
    CmdArg_t cmd_args;
} Command_t;

Command_t* new_Command();
int add_Arg(Command_t *cmd, const char *arg);
int add_select_field(Command_t *cmd, const char *argument);
void cleanup_Command(Command_t *cmd);

#endif
