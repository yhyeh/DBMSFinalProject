#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Command.h"
#include "SelectState.h"

void field_state_handler(Command_t *cmd, size_t arg_idx) {
    cmd->cmd_args.sel_args.fields = NULL;
    cmd->cmd_args.sel_args.fields_len = 0;
    cmd->cmd_args.sel_args.limit = -1;
    cmd->cmd_args.sel_args.offset = -1;
	cmd->cmd_args.sel_args.where_begin = -1;
	cmd->cmd_args.sel_args.where_end = -1;
    while(arg_idx < cmd->args_len) {
        if (!strncmp(cmd->args[arg_idx], "*", 1)) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "id", 2)) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "name", 4)) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "email", 5)) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "age", 3)) {
            add_select_field(cmd, cmd->args[arg_idx]);
        } else if (!strncmp(cmd->args[arg_idx], "from", 4)) {
            table_state_handler(cmd, arg_idx+1);
            return;
        }
		else if (!strncmp(cmd->args[arg_idx],"avg(",4))
		{
			char *ptr = cmd->args[arg_idx]+4;
			if (!strncmp(ptr, "id)", 3)) {
				add_select_field(cmd, cmd->args[arg_idx]);
			} else if (!strncmp(ptr, "age)", 4)) {
				add_select_field(cmd, cmd->args[arg_idx]);
			} else if (!strncmp(ptr, "id1)", 4)) {
				add_select_field(cmd, cmd->args[arg_idx]);
			} else if (!strncmp(ptr, "id2)", 4)) {
				add_select_field(cmd, cmd->args[arg_idx]);
			}
		}
		else if (!strncmp(cmd->args[arg_idx],"sum(",4))
		{
			char *ptr = cmd->args[arg_idx]+4;
			if (!strncmp(ptr, "id)", 3)) {
				add_select_field(cmd, cmd->args[arg_idx]);
			} else if (!strncmp(ptr, "age)", 4)) {
				add_select_field(cmd, cmd->args[arg_idx]);
			} else if (!strncmp(ptr, "id1)", 4)) {
				add_select_field(cmd, cmd->args[arg_idx]);
			} else if (!strncmp(ptr, "id2)", 4)) {
				add_select_field(cmd, cmd->args[arg_idx]);
			}
		}
		else if (!strncmp(cmd->args[arg_idx],"count(",6))
		{
			char *ptr = cmd->args[arg_idx]+6;
			if (!strncmp(ptr, "*)", 2)) {
				add_select_field(cmd, cmd->args[arg_idx]);
			} else if (!strncmp(ptr, "id)", 3)) {
				add_select_field(cmd, cmd->args[arg_idx]);
			} else if (!strncmp(ptr, "name)", 5)) {
				add_select_field(cmd, cmd->args[arg_idx]);
			} else if (!strncmp(ptr, "email)", 6)) {
				add_select_field(cmd, cmd->args[arg_idx]);
			} else if (!strncmp(ptr, "age)", 4)) {
				add_select_field(cmd, cmd->args[arg_idx]);
			} else if (!strncmp(ptr, "id1)", 4)) {
				add_select_field(cmd, cmd->args[arg_idx]);
			} else if (!strncmp(ptr, "id2)", 4)) {
				add_select_field(cmd, cmd->args[arg_idx]);
			}
		}
		else {
            cmd->type = UNRECOG_CMD;
            return;
        }
        arg_idx += 1;
    }
    cmd->type = UNRECOG_CMD;
    return;
}

void table_state_handler(Command_t *cmd, size_t arg_idx) {
    if (arg_idx < cmd->args_len
            && (!strncmp(cmd->args[arg_idx], "user", 4) ||
				!strncmp(cmd->args[arg_idx], "like", 4))) {

        arg_idx++;
        if (arg_idx == cmd->args_len) {
            return;
        } 
		else if (!strncmp(cmd->args[arg_idx], "where", 5)){
			where_state_handler(cmd, arg_idx+1);
			return;
		}
		else if (!strncmp(cmd->args[arg_idx], "offset", 6)) {
            offset_state_handler(cmd, arg_idx+1);
            return;
        } else if (!strncmp(cmd->args[arg_idx], "limit", 5)) {
            limit_state_handler(cmd, arg_idx+1);
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
    return;
}

void where_state_handler(Command_t *cmd, size_t arg_idx) {
	cmd->cmd_args.sel_args.where_begin = arg_idx;
	while(arg_idx < cmd->args_len) {
		if (!strncmp(cmd->args[arg_idx], "offset", 6)) {
            offset_state_handler(cmd, arg_idx+1);
            return;
        } else if (!strncmp(cmd->args[arg_idx], "limit", 5)) {
            limit_state_handler(cmd, arg_idx+1);
            return;
        }
		cmd->cmd_args.sel_args.where_end = arg_idx;
		arg_idx++;
    }
    cmd->type = UNRECOG_CMD;
    return;
}

void offset_state_handler(Command_t *cmd, size_t arg_idx) {
    if (arg_idx < cmd->args_len) {
        cmd->cmd_args.sel_args.offset = atoi(cmd->args[arg_idx]);

        arg_idx++;

        if (arg_idx == cmd->args_len) {
            return;
        } else if (arg_idx < cmd->args_len
                && !strncmp(cmd->args[arg_idx], "limit", 5)) {

            limit_state_handler(cmd, arg_idx+1);
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
    return;
}

void limit_state_handler(Command_t *cmd, size_t arg_idx) {
    if (arg_idx < cmd->args_len) {
        cmd->cmd_args.sel_args.limit = atoi(cmd->args[arg_idx]);

        arg_idx++;

        if (arg_idx == cmd->args_len) {
            return;
        }
    }
    cmd->type = UNRECOG_CMD;
    return;
}
