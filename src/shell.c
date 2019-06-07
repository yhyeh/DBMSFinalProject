#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "InputBuffer.h"
#include "Util.h"
#include "Util_Like.h"
#include "Table.h"
#include "Table_Like.h"

int main(int argc, char **argv) {
    InputBuffer_t *input_buffer = new_InputBuffer();
    Command_t *cmd = new_Command();
    State_t *state = new_State();
    Table_t *table_user = NULL;
	Table_Like_t *table_like = NULL;
    int cmd_type;
    if (argc != 2) {
        table_user = new_Table(NULL);
    } else {
        table_user = new_Table(argv[1]);
    }
	table_like = new_Table_Like(NULL);
    if (table_user == NULL) {
        return 1;
    }
	if (table_like == NULL){
		return 1;
	}
    for (;;) {
        print_prompt(state);
        read_input(input_buffer);
        cmd_type = parse_input(input_buffer->buffer, cmd);
		if (cmd_type == BUILT_IN_CMD) {
				handle_builtin_cmd(table_user, cmd, state);
		}
		else if (cmd_type == UNRECOG_CMD) {
				printf("Unrecognized command '%s'.\n", input_buffer->buffer);
		}
		else{
			if(cmd->type_table == USER_TB){
				handle_query_cmd(table_user, cmd);
			}
			else if(cmd->type_table == LIKE_TB){
				handle_query_cmd_like(table_like, cmd);
			}
			else if(cmd->type_table == JOIN_TB){
				handle_join_cmd(table_user, table_like, cmd);
			}
		}
        cleanup_Command(cmd);
        clean_InputBuffer(input_buffer);
    }
    return 0;
}
