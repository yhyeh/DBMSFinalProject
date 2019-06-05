#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "Util.h"
#include "Command.h"
#include "Table.h"
#include "SelectState.h"
#include "User.h"
#include "Table_Like.h"
#include "Hash.h"

///
/// Allocate State_t and initialize some attributes
/// Return: ptr of new State_t
///
State_t* new_State() {
    State_t *state = (State_t*)malloc(sizeof(State_t));
    state->saved_stdout = -1;
    return state;
}

///
/// Print shell prompt
///
void print_prompt(State_t *state) {
    if (state->saved_stdout == -1) {
        printf("db > ");
    }
}

///
/// Print the user in the specific format
///
void print_user(User_t *user, SelectArgs_t *sel_args) {
    size_t idx;
    printf("(");
    for (idx = 0; idx < sel_args->fields_len; idx++) {
        if (!strncmp(sel_args->fields[idx], "*", 1)) {
            printf("%d, %s, %s, %d", user->id, user->name, user->email, user->age);
        } else {
            if (idx > 0) printf(", ");

            if (!strncmp(sel_args->fields[idx], "id", 2)) {
                printf("%d", user->id);
            } else if (!strncmp(sel_args->fields[idx], "name", 4)) {
                printf("%s", user->name);
            } else if (!strncmp(sel_args->fields[idx], "email", 5)) {
                printf("%s", user->email);
            } else if (!strncmp(sel_args->fields[idx], "age", 3)) {
                printf("%d", user->age);
            }
        }
    }
    printf(")\n");
}

///
/// Print the users for given offset and limit restriction
///
void print_users(Table_t *table, int *idxList, size_t idxListLen, Command_t *cmd) {
    size_t idx;
    int limit = cmd->cmd_args.sel_args.limit;
    int offset = cmd->cmd_args.sel_args.offset;

    if (offset == -1) {
        offset = 0;
    }
	int flag = 0;
	for (int i = 0; i < cmd->cmd_args.sel_args.fields_len; i++){
		if (!strncmp(cmd->cmd_args.sel_args.fields[i], "avg(id)", 7)) {
			if (offset !=0 || limit == 0)
				return;
			flag = 1;
			if (i==0)
				printf("(");
			else
				printf(", ");
			if (idxList){
				double sum = 0 ;
				for (idx = 0 ; idx < idxListLen; idx++)
				{
					User_t* user = get_User(table, idxList[idx]);
					sum += user->id;
				}
				printf("%.3f",sum/idxListLen);
			}
			else{
				double sum = 0 ;
				for (idx = 0 ; idx < table->len; idx++)
				{
					User_t* user = get_User(table, idx);
					sum += user->id;
				}
				printf("%.3f",sum/table->len);
			}
		}
		else if (!strncmp(cmd->cmd_args.sel_args.fields[i], "avg(age)", 8)){
			if (offset !=0 || limit == 0)
				return;
			flag = 1;
			if (i==0)
				printf("(");
			else
				printf(", ");
			if (idxList){
				double sum = 0 ;
				for (idx = 0 ; idx < idxListLen; idx++)
				{
					User_t* user = get_User(table, idxList[idx]);
					sum += user->age;
				}
				printf("%.3f",sum/idxListLen);
			}
			else{
				double sum = 0 ;
				for (idx = 0 ; idx < table->len; idx++)
				{
					User_t* user = get_User(table, idx);
					sum += user->age;
				}
				printf("%.3f",sum/table->len);
			}
		}
		else if (!strncmp(cmd->cmd_args.sel_args.fields[i], "sum(id)", 7)){
			if (offset !=0 || limit == 0)
				return;
			flag = 1;
			if (i==0)
				printf("(");
			else
				printf(", ");
			if (idxList){
				int sum = 0 ;
				for (idx = 0 ; idx < idxListLen; idx++)
				{
					User_t* user = get_User(table, idxList[idx]);
					sum += user->id;
				}
				printf("%d",sum);
			}
			else{
				int sum = 0 ;
				for (idx = 0 ; idx < table->len; idx++)
				{
					User_t* user = get_User(table, idx);
					sum += user->id;
				}
				printf("%d",sum);
			}
		}
		else if (!strncmp(cmd->cmd_args.sel_args.fields[i], "sum(age)", 8)){
			if (offset !=0 || limit == 0)
				return;
			flag = 1;
			if (i==0)
				printf("(");
			else
				printf(", ");
			if (idxList){
				int sum = 0 ;
				for (idx = 0 ; idx < idxListLen; idx++)
				{
					User_t* user = get_User(table, idxList[idx]);
					sum += user->age;
				}
				printf("%d",sum);
			}
			else{
				int sum = 0 ;
				for (idx = 0 ; idx < table->len; idx++)
				{
					User_t* user = get_User(table, idx);
					sum += user->age;
				}
				printf("%d",sum);
			}
		}
		else if (!strncmp(cmd->cmd_args.sel_args.fields[i], "count(", 6)){
			if (offset !=0 || limit == 0)
				return;
			flag = 1;
			if (i==0)
				printf("(");
			else
				printf(", ");
			if (idxList){
				printf("%ld",idxListLen);
			}
			else{
				printf("%ld",table->len);
			}
		}

	}

	if(flag){
		printf(")\n");
	}
	else{
		if (idxList) {
			for (idx = offset; idx < idxListLen; idx++) {
				if (limit != -1 && (idx - offset) >= limit) {
					break;
				}
				print_user(get_User(table, idxList[idx]), &(cmd->cmd_args.sel_args));
			}

		} else {
			for (idx = offset; idx < table->len; idx++) {
				if (limit != -1 && (idx - offset) >= limit) {
					break;
				}
				print_user(get_User(table, idx), &(cmd->cmd_args.sel_args));
			}
		}
	}
}

///
/// This function received an output argument
/// Return: category of the command
///
int parse_input(char *input, Command_t *cmd) {
    char *token;
    int idx;
    token = strtok(input, " ,\n");
	cmd->type_table = UNRECOG_TB;
	if(token == NULL){
		cmd->type = UNRECOG_CMD;
		return cmd->type;
	}
    for (idx = 0; strlen(cmd_list[idx].name) != 0; idx++) {
        if (!strncmp(token, cmd_list[idx].name, cmd_list[idx].len)) {
            cmd->type = cmd_list[idx].type;
        }
    }
    while (token != NULL) {
		if (!strncmp(token,"join",4))
			cmd->type_table = JOIN_TB;
		if (cmd->type_table == UNRECOG_TB){
			if (!strncmp(token,"user",4)){
				cmd->type_table = USER_TB;
			}
			else if (!strncmp(token,"like",4)){
				cmd->type_table = LIKE_TB;
			}
		}
        add_Arg(cmd, token);
        token = strtok(NULL, " ,\n");
    }
    return cmd->type;
}

///
/// Handle built-in commands
/// Return: command type
///
void handle_builtin_cmd(Table_t *table, Command_t *cmd, State_t *state) {
    if (!strncmp(cmd->args[0], ".exit", 5)) {
        archive_table(table);
        exit(0);
    } else if (!strncmp(cmd->args[0], ".output", 7)) {
        if (cmd->args_len == 2) {
            if (!strncmp(cmd->args[1], "stdout", 6)) {
                close(1);
                dup2(state->saved_stdout, 1);
                state->saved_stdout = -1;
            } else if (state->saved_stdout == -1) {
                int fd = creat(cmd->args[1], 0644);
                state->saved_stdout = dup(1);
                if (dup2(fd, 1) == -1) {
                    state->saved_stdout = -1;
                }
                __fpurge(stdout); //This is used to clear the stdout buffer
            }
        }
    } else if (!strncmp(cmd->args[0], ".load", 5)) {
        if (cmd->args_len == 2) {
            load_table(table, cmd->args[1]);
        }
    } else if (!strncmp(cmd->args[0], ".help", 5)) {
        print_help_msg();
    }
}

///
/// Handle query type commands
/// Return: command type
///
int handle_query_cmd(Table_t *table, Command_t *cmd) {
    if (!strncmp(cmd->args[0], "insert", 6)) {
        handle_insert_cmd(table, cmd);
        return INSERT_CMD;
    } else if (!strncmp(cmd->args[0], "select", 6)) {
        handle_select_cmd(table, cmd);
        return SELECT_CMD;
    }
	else if (!strncmp(cmd->args[0], "update", 6)) {
		handle_update_cmd(table,cmd);
		return UPDATE_CMD;
	}
	else if (!strncmp(cmd->args[0], "delete", 6)) {
		handle_delete_cmd(table,cmd);
		return DELETE_CMD;
	}
	else {
        return UNRECOG_CMD;
    }
}
///
/// The return value is the number of rows insert into table
/// If the delete operation success, then change the input arg
/// `cmd->type` to DELETE_CMD
///

int handle_delete_cmd(Table_t *table, Command_t *cmd) {
	int ret = 0;

	cmd->cmd_args.sel_args.where_begin = -1;
	cmd->cmd_args.sel_args.where_end = -1;
	if(strncmp(cmd->args[1], "from", 4) || strncmp(cmd->args[2], "user", 4)){
		cmd->type = UNRECOG_CMD;
        return ret;
	}
	size_t arg_idx = 3;
	if (arg_idx == cmd->args_len){
		table->len = 0;
	}
	else if(!strncmp(cmd->args[arg_idx], "where", 5)){
		cmd->cmd_args.sel_args.where_begin = arg_idx+1;
		cmd->cmd_args.sel_args.where_end = cmd->args_len - 1 ;
		int *idxList = malloc(sizeof(int)* table->len);
		size_t idxListLen = where_check(table,cmd,idxList,NULL);
		for (int idx = idxListLen -1 ; idx >=0 ; idx--)
		{
			for(int i = idxList[idx] ; i < table->len -1 ; i++)
			{
				User_t* temp_u1 = get_User(table,i);
				User_t* temp_u2 = get_User(table,i+1);
				memcpy((table->users)+i, temp_u2, sizeof(User_t));
				memcpy((table->users)+i+1, temp_u1, sizeof(User_t));
			}
			table->len--;
		}
	}
	else {
		cmd->type = UNRECOG_CMD;
		return ret;
	}
	cmd->type = DELETE_CMD;
	ret = 1;
	return ret;
}




///
/// The return value is the number of rows insert into table
/// If the update operation success, then change the input arg
/// `cmd->type` to UPDATE_CMD
///

int handle_update_cmd(Table_t *table, Command_t *cmd) {
    int ret = 0;

	cmd->cmd_args.sel_args.where_begin = -1;
	cmd->cmd_args.sel_args.where_end = -1;

	if(strncmp(cmd->args[1], "user", 4) || strncmp(cmd->args[2], "set", 3)){
		cmd->type = UNRECOG_CMD;
        return ret;
	}
	size_t arg_idx = 3;
	size_t char_idx = 0;
	int flag_col = -1;
	int num = 0;
	char *str = NULL;
	char temp[200];
	for(size_t i = arg_idx; i < cmd->args_len; i++){
		if (!strncmp(cmd->args[i], "where", 5)){
			arg_idx = i;
			cmd->cmd_args.sel_args.where_begin = i+1;
			break;
		}
		if(i == arg_idx)
			strcpy(temp,cmd->args[i]);
		else
			strcat(temp,cmd->args[i]);
	}

	if(!strncmp(temp, "id", 2)){
		char_idx = 2;
		if (temp[char_idx] != '='){
			cmd->type = UNRECOG_CMD;
			return ret;
		}
		char_idx++;
		char *ptr = temp + char_idx;
		flag_col = 1;
		num = atoi(ptr);
	}
	else if(!strncmp(temp, "name", 4)){
		char_idx = 4;
		if (temp[char_idx] != '='){
			cmd->type = UNRECOG_CMD;
			return ret;
		}
		char_idx++;
		char *ptr = temp + char_idx;
		flag_col = 2;
		str = strdup(ptr);
	}
	else if(!strncmp(temp, "email", 5)){
		char_idx = 5;
		if (temp[char_idx] != '='){
			cmd->type = UNRECOG_CMD;
			return ret;
		}
		char_idx++;
		char *ptr = temp + char_idx;
		flag_col = 3;
		str = strdup(ptr);
	}
	else if(!strncmp(temp, "age", 3)){
		char_idx = 3;
		if (temp[char_idx] != '='){
			cmd->type = UNRECOG_CMD;
			return ret;
		}
		char_idx++;
		char *ptr = temp + char_idx;
		flag_col = 4;
		num = atoi(ptr);
	}
	else {
		cmd->type = UNRECOG_CMD;
		return ret;
	}

	if (cmd->cmd_args.sel_args.where_begin != -1){
		cmd->cmd_args.sel_args.where_end = cmd->args_len - 1;
		int *idxList = malloc(sizeof(int)* table->len);
		size_t idxListLen = where_check(table,cmd,idxList,NULL);
		for (size_t idx =0 ; idx<idxListLen; idx++){
			User_t* user = get_User(table, idxList[idx]);
			if(flag_col ==1){
				if(idxListLen==1)
				{
					int flag_t = 0;
					for(size_t i = 0 ; i < table->len; i++)
					{
						User_t* user_temp = get_User(table, i);
						if(user_temp->id == num)
						{
							flag_t = 1;
							break;
						}
					}
					if(!flag_t)
						user->id = num;
				}
			}
			else if(flag_col ==2){
				strncpy(user->name,str,strlen(str)+1);
			}
			else if(flag_col ==3){
				strncpy(user->email,str,strlen(str)+1);
			}
			else if(flag_col ==4){
				user->age = num;
			}
		}
	}
	else{
		for(size_t idx = 0 ; idx < table->len; idx++)
		{
			User_t* user = get_User(table, idx);
			if(flag_col ==1){
				if (table->len == 1)
					user->id = num;
			}
			else if(flag_col ==2){
				strncpy(user->name,str,strlen(str)+1);
			}
			else if(flag_col ==3){
				strncpy(user->email,str,strlen(str)+1);
			}
			else if(flag_col ==4){
				user->age = num;
			}
		}
	}
	if(str != NULL)
		free(str);

	cmd->type = UPDATE_CMD;
	ret = 1;
    return ret;
}

///
/// The return value is the number of rows insert into table
/// If the insert operation success, then change the input arg
/// `cmd->type` to INSERT_CMD
///
int handle_insert_cmd(Table_t *table, Command_t *cmd) {
    int ret = 0;
    User_t *user = command_to_User(cmd);
    if (user) {
        ret = add_User(table, user);
        if (ret > 0) {
            cmd->type = INSERT_CMD;
        }
    }
    return ret;
}

///
/// directly handle join
///
int handle_join_cmd(Table_t *user_table, Table_Like_t *like_table, Command_t *cmd){
  // select count(*) from user join like on [id = id1] where [age > 20]
  // 0      1        2    3    4    5    6   7  8 9    10
  /*// for debugging
  if(strncmp(cmd->args[4], "join", 4)) {
    printf("Not join cmd.\n");
    return UNRECOG_CMD;
  }
  else{
    printf("Enter join cmd processing.\n");
  }
  // end of debugging
  */

  // Init
  int arg_idx = 10;
  Hash_t* hash_entry = (Hash_t*)malloc(sizeof(Hash_t)*HASH_ENTRY_LEN);
  memset(hash_entry, 0, sizeof(Hash_t)*HASH_ENTRY_LEN);
  size_t result_cnt = 0;
  // check where conditions exist or not. As well as offset and limit.
  if(!strncmp(cmd->args[arg_idx], "where", 5)){
    where_state_handler(cmd, arg_idx+1);
    where_check(user_table, cmd, NULL, hash_entry);
  } else {
    //-----------------------------------------------
    // TODO:
    // Add all user id to hash_entry
    //-----------------------------------------------
    if (!strncmp(cmd->args[arg_idx], "offset", 6)) {
      offset_state_handler(cmd, arg_idx+1);
    } else if (!strncmp(cmd->args[arg_idx], "limit", 5)) {
      limit_state_handler(cmd, arg_idx+1);
    }
  }

  // check join on id = id1 or id2
  // then count
  Like_t* Like;
  Hash_t h_entry;
  if(!strncmp(cmd->args[9], "id1", 3)){
    for(size_t i = 0; i < like_table->len; i++){
      Like = get_Like(like_table, i);
      h_entry = hash_entry[Like->id1 % HASH_ENTRY_LEN];
      for(size_t j = 0; j < h_entry.uid_buf_len; j++){
        if(Like->id1 == h_entry.uid_buf[j]) result_cnt++;
      }
    }
  }else if(!strncmp(cmd->args[9], "id2", 3)){
    for(size_t i = 0; i < like_table->len; i++){
      Like = get_Like(like_table, i);
      h_entry = hash_entry[Like->id2 % HASH_ENTRY_LEN];
      for(size_t j = 0; j < h_entry.uid_buf_len; j++){
        if(Like->id2 == h_entry.uid_buf[j]) result_cnt++;
      }
    }
  }

  //-----------------------------------------------
  // TODO:
  // adjust count if offset or limit is set
  //-----------------------------------------------

  // print count(*)
  printf("(%lu)\n", result_cnt);
  return SELECT_CMD;
}
size_t where_check(Table_t *table, Command_t *cmd, int* idxList, Hash_t* hash_entry)
{
	if (idxList == NULL)
		idxList = malloc(sizeof(int) * table->len);
	int operator_mat[10]; // relational_ops
	int op_i = 0;
	int cond_mat[10]; // user_fields
	int cond_i = 0;
	double number_mat[10];
	char char_mat[10][100];
	int num_i = 0;
	int logic_mat[10]; // logical_ops
	int logic_i = 0;
	size_t idxListLen = 0;
	int cond_begin = cmd->cmd_args.sel_args.where_begin;
	int cond_end = cmd->cmd_args.sel_args.where_end;
  int cond_len = cond_end - cond_begin + 1;
  char cur_str[256];
	for(int index_cmd = 0; index_cmd < cond_len; index_cmd++){
    strcpy(cur_str, cmd->args[cond_begin + index_cmd]);
    switch(index_cmd % 4){
      case COND:
        if (!strncmp(cur_str, "id", 2))
          cond_mat[cond_i++] = ID;
        else if (!strncmp(cur_str, "name", 4))
          cond_mat[cond_i++] = NAME;
        else if (!strncmp(cur_str, "email", 5))
          cond_mat[cond_i++] = EMAIL;
        else if (!strncmp(cur_str, "age", 3))
          cond_mat[cond_i++] = AGE;
        else
          printf("where-condition field err: %s\n", cur_str);
        break;
      case OP:
        if (!strncmp(cur_str, "=", 1))
          operator_mat[op_i++] = EQ;
        else if (!strncmp(cur_str, "!=", 2))
          operator_mat[op_i++] = NEQ;
        else if (!strncmp(cur_str, ">=", 2))
          operator_mat[op_i++] = GTE;
        else if (!strncmp(cur_str, "<=", 2))
          operator_mat[op_i++] = LTE;
        else if (!strncmp(cur_str, ">", 1))
          operator_mat[op_i++] = GT;
        else if (!strncmp(cur_str, "<", 1))
          operator_mat[op_i++] = LT;
        else
          printf("where-condition op err: %s\n", cur_str);
        break;
      case VALUE:
        if (cur_str[0] == '"')
          strcpy(char_mat[num_i++], cur_str);
        else
          number_mat[num_i++] = atof(cur_str);
        break;
      case LOGIC:
        if (!strncmp(cur_str, "or", 2))
          logic_mat[logic_i++] = OR;
        else if (!strncmp(cur_str, "and", 3))
          logic_mat[logic_i++] = AND;
        else
          printf("where-condition logic op err: %s\n", cur_str);
    }
	}
	//////////////////////end translation /////////////////////////////
	if(op_i != cond_i || op_i != num_i || op_i != logic_i + 1)
	{
		printf("error op cond num error!\n");
		cmd->type = UNRECOG_CMD;
		return table->len;
	}
	for (int idx = 0 ; idx < table->len; idx++)
	{
		User_t* User = get_User(table, idx);
		int flag1 = 0 ;
		int flag2 = 0 ;
		for(int i = 0 ; i < op_i;i++){
			double number = number_mat[i];
			if (cond_mat[i] == ID){
				if(operator_mat[i]==EQ){
					if(User->id == number)
						flag2 = 1;
					else
						flag2 = 0;
				}
				if(operator_mat[i]==NEQ){
					if(User->id != number)
						flag2 = 1;
					else
						flag2 = 0;
				}
				if(operator_mat[i]==GT){
					if(User->id > number)
						flag2 = 1;
					else
						flag2 = 0;
				}
				if(operator_mat[i]==LT){
					if(User->id < number)
						flag2 = 1;
					else
						flag2 = 0;
				}
				if(operator_mat[i]==GTE){
					if(User->id >= number)
						flag2 = 1;
					else
						flag2 = 0;
				}
				if(operator_mat[i]==LTE){
					if(User->id <= number)
						flag2 = 1;
					else
						flag2 = 0;
				}
			}
			else if (cond_mat[i] == NAME){
				if(operator_mat[i]==EQ){
					if(!strcmp(User->name,char_mat[i]))
						flag2 = 1;
					else
						flag2 = 0;
				}
				if(operator_mat[i]==NEQ){
					if(strcmp(User->name,char_mat[i]))
						flag2 = 1;
					else
						flag2 = 0;
				}
			}
			else if (cond_mat[i] == EMAIL){
				if(operator_mat[i]==EQ){
					if(!strcmp(User->email,char_mat[i]))
						flag2 = 1;
					else
						flag2 = 0;
				}
				if(operator_mat[i]==NEQ){
					if(strcmp(User->email,char_mat[i]))
						flag2 = 1;
					else
						flag2 = 0;
				}
			}
			else if (cond_mat[i] == AGE){
				if(operator_mat[i]==EQ){
					if(User->age == number)
						flag2 = 1;
					else
						flag2 = 0;
				}
				if(operator_mat[i]==NEQ){
					if(User->age != number)
						flag2 = 1;
					else
						flag2 = 0;
				}
				if(operator_mat[i]==GT){
					if(User->age > number)
						flag2 = 1;
					else
						flag2 = 0;
				}
				if(operator_mat[i]==LT){
					if(User->age < number)
						flag2 = 1;
					else
						flag2 = 0;
				}
				if(operator_mat[i]==GTE){
					if(User->age >= number)
						flag2 = 1;
					else
						flag2 = 0;
				}
				if(operator_mat[i]==LTE){
					if(User->age <= number)
						flag2 = 1;
					else
						flag2 = 0;
				}
			}
			if(i == 0)
				flag1 = flag2;
			else
			{
				if(logic_mat[i-1]==OR)  // or
					flag1 |= flag2;
				else if (logic_mat[i-1]==AND)
					flag1 &= flag2;
			}
		}
		if (flag1){
      if (hash_entry == NULL){
        idxList[idxListLen++] = idx;
      }else {
        // append User->id to corresponding hash_entry
        Hash_t* h_entry_ptr = &hash_entry[User->id % HASH_ENTRY_LEN];
        unsigned int* new_uid_buf = (unsigned int*)malloc(sizeof(unsigned int)*(h_entry_ptr->uid_buf_len+1));
        memcpy(new_uid_buf, h_entry_ptr->uid_buf, sizeof(unsigned int)*h_entry_ptr->uid_buf_len);
        new_uid_buf[h_entry_ptr->uid_buf_len] = User->id;
        free(h_entry_ptr->uid_buf);
        h_entry_ptr->uid_buf = new_uid_buf;
        h_entry_ptr->uid_buf_len++;
      }
    }
	}
	return idxListLen;

}




///
/// The return value is the number of rows select from table
/// If the select operation success, then change the input arg
/// `cmd->type` to SELECT_CMD
///
int handle_select_cmd(Table_t *table, Command_t *cmd) {
    cmd->type = SELECT_CMD;
    field_state_handler(cmd, 1);
	if (cmd->cmd_args.sel_args.where_begin == -1){
		print_users(table, NULL, 0, cmd);
	}
	else {
		int *idxList ;
		idxList = malloc(sizeof(int) * table->len);
		size_t idxListLen;
		idxListLen = where_check(table,cmd,idxList, NULL);
		print_users(table, idxList, idxListLen, cmd);
		free(idxList);
    }
    return table->len;
}

///
/// Show the help messages
///
void print_help_msg() {
    const char msg[] = "# Supported Commands\n"
    "\n"
    "## Built-in Commands\n"
    "\n"
    "  * .exit\n"
    "\tThis cmd archives the table, if the db file is specified, then exit.\n"
    "\n"
    "  * .output\n"
    "\tThis cmd change the output strategy, default is stdout.\n"
    "\n"
    "\tUsage:\n"
    "\t    .output (<file>|stdout)\n\n"
    "\tThe results will be redirected to <file> if specified, otherwise they will display to stdout.\n"
    "\n"
    "  * .load\n"
    "\tThis command loads records stored in <DB file>.\n"
    "\n"
    "\t*** Warning: This command will overwrite the records already stored in current table. ***\n"
    "\n"
    "\tUsage:\n"
    "\t    .load <DB file>\n\n"
    "\n"
    "  * .help\n"
    "\tThis cmd displays the help messages.\n"
    "\n"
    "## Query Commands\n"
    "\n"
    "  * insert\n"
    "\tThis cmd inserts one user record into table.\n"
    "\n"
    "\tUsage:\n"
    "\t    insert <id> <name> <email> <age>\n"
    "\n"
    "\t** Notice: The <name> & <email> are string without any whitespace character, and maximum length of them is 255. **\n"
    "\n"
    "  * select\n"
    "\tThis cmd will display all user records in the table.\n"
    "\n";
    printf("%s", msg);
}
