#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include "Util_Like.h"
#include "Command.h"
#include "Table_Like.h"
#include "SelectState.h"
#include "Util.h"

///
/// Print the like in the specific format
///
void print_like(Like_t *like, SelectArgs_t *sel_args) {
    size_t idx;
    printf("(");
    for (idx = 0; idx < sel_args->fields_len; idx++) {
        if (!strncmp(sel_args->fields[idx], "*", 1)) {
            printf("%d, %d", like->id1, like->id2);
        }
		else {
            if (idx > 0) printf(", ");
            if (!strncmp(sel_args->fields[idx], "id1", 3)) {
                printf("%d", like->id1);
            } else if (!strncmp(sel_args->fields[idx], "id2", 3)) {
                printf("%d", like->id2);
            }
        }
    }
    printf(")\n");
}

///
/// Print the likes for given offset and limit restriction
///
void print_likes(Table_Like_t *table, int *idxList, size_t idxListLen, Command_t *cmd) {
    size_t idx;
    int limit = cmd->cmd_args.sel_args.limit;
    int offset = cmd->cmd_args.sel_args.offset;

    if (offset == -1) {
        offset = 0;
    }
	int flag = 0;
	for (int i = 0; i < cmd->cmd_args.sel_args.fields_len; i++){
		if (!strncmp(cmd->cmd_args.sel_args.fields[i], "avg(id1)", 8)) {
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
					Like_t* like = get_Like(table, idxList[idx]);
					sum += like->id1;
				}
				printf("%.3f",sum/idxListLen);
			}
			else{
				double sum = 0 ;
				for (idx = 0 ; idx < table->len; idx++)
				{
					Like_t* like = get_Like(table, idx);
					sum += like->id1;
				}
				printf("%.3f",sum/table->len);
			}
		}
		else if (!strncmp(cmd->cmd_args.sel_args.fields[i], "avg(id2)", 8)){
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
					Like_t* like = get_Like(table, idxList[idx]);
					sum += like->id2;
				}
				printf("%.3f",sum/idxListLen);
			}
			else{
				double sum = 0 ;
				for (idx = 0 ; idx < table->len; idx++)
				{
					Like_t* like = get_Like(table, idx);
					sum += like->id2;
				}
				printf("%.3f",sum/table->len);
			}
		}
		else if (!strncmp(cmd->cmd_args.sel_args.fields[i], "sum(id1)", 8)){
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
					Like_t* like = get_Like(table, idxList[idx]);
					sum += like->id1;
				}
				printf("%d",sum);
			}
			else{
				int sum = 0 ;
				for (idx = 0 ; idx < table->len; idx++)
				{
					Like_t* like = get_Like(table, idx);
					sum += like->id1;
				}
				printf("%d",sum);
			}
		}
		else if (!strncmp(cmd->cmd_args.sel_args.fields[i], "sum(id2)", 8)){
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
					Like_t* like = get_Like(table, idxList[idx]);
					sum += like->id2;
				}
				printf("%d",sum);
			}
			else{
				int sum = 0 ;
				for (idx = 0 ; idx < table->len; idx++)
				{
					Like_t* like = get_Like(table, idx);
					sum += like->id2;
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
				print_like(get_Like(table, idxList[idx]), &(cmd->cmd_args.sel_args));
			}

		} else {
			for (idx = offset; idx < table->len; idx++) {
				if (limit != -1 && (idx - offset) >= limit) {
					break;
				}
				print_like(get_Like(table, idx), &(cmd->cmd_args.sel_args));
			}
		}
	}
}




///
/// Handle query type commands
/// Return: command type
///
int handle_query_cmd_like(Table_Like_t *table, Command_t *cmd) {
    if (!strncmp(cmd->args[0], "insert", 6)) {
        handle_insert_cmd_like(table, cmd);
        return INSERT_CMD;
    } else if (!strncmp(cmd->args[0], "select", 6)) {
        handle_select_cmd_like(table, cmd);
        return SELECT_CMD;
    }
	else {
        return UNRECOG_CMD;
    }
}


///
/// The return value is the number of rows insert into table
/// If the insert operation success, then change the input arg
/// `cmd->type` to INSERT_CMD
///
int handle_insert_cmd_like(Table_Like_t *table, Command_t *cmd) {
    int ret = 0;
    Like_t *like = command_to_Like(cmd);
    if (like) {
        ret = add_Like(table, like);
        if (ret > 0) {
            cmd->type = INSERT_CMD;
        }
    }
    return ret;
}




	///////       	UNFINISHED  		///////
	///////			UNFINISHED  		///////
	///////			UNFINISHED  		///////
	///////			UNFINISHED  		///////
	///////			UNFINISHED  		///////
	///////			UNFINISHED  		///////
	///////			UNFINISHED  		///////
size_t where_check_like(Table_Like_t *table, Command_t *cmd, int* idxList)
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
        if (!strncmp(cur_str, "id1", 3))
          cond_mat[cond_i++] = ID1;
        else if (!strncmp(cur_str, "id2", 3))
          cond_mat[cond_i++] = ID2;
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
    Like_t* Like = get_Like(table, idx);
    int flag1 = 0 ;
    int flag2 = 0 ;
    for(int i = 0 ; i < op_i;i++){
      double number = number_mat[i];
      if (cond_mat[i] == ID1){
        if(operator_mat[i]==EQ){
          if(Like->id1 == number)
            flag2 = 1;
          else
            flag2 = 0;
        }
        if(operator_mat[i]==NEQ){
          if(Like->id1 != number)
            flag2 = 1;
          else
            flag2 = 0;
        }
        if(operator_mat[i]==GT){
          if(Like->id1 > number)
            flag2 = 1;
          else
            flag2 = 0;
        }
        if(operator_mat[i]==LT){
          if(Like->id1 < number)
            flag2 = 1;
          else
            flag2 = 0;
        }
        if(operator_mat[i]==GTE){
          if(Like->id1 >= number)
            flag2 = 1;
          else
            flag2 = 0;
        }
        if(operator_mat[i]==LTE){
          if(Like->id1 <= number)
            flag2 = 1;
          else
            flag2 = 0;
        }
      }
      else if (cond_mat[i] == ID2){
        if(operator_mat[i]==EQ){
          if(Like->id2 == number)
            flag2 = 1;
          else
            flag2 = 0;
        }
        if(operator_mat[i]==NEQ){
          if(Like->id2 != number)
            flag2 = 1;
          else
            flag2 = 0;
        }
        if(operator_mat[i]==GT){
          if(Like->id2 > number)
            flag2 = 1;
          else
            flag2 = 0;
        }
        if(operator_mat[i]==LT){
          if(Like->id2 < number)
            flag2 = 1;
          else
            flag2 = 0;
        }
        if(operator_mat[i]==GTE){
          if(Like->id2 >= number)
            flag2 = 1;
          else
            flag2 = 0;
        }
        if(operator_mat[i]==LTE){
          if(Like->id2 <= number)
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
    if (flag1)
      idxList[idxListLen++] = idx;
  }
  return idxListLen;
}




///
/// The return value is the number of rows select from table
/// If the select operation success, then change the input arg
/// `cmd->type` to SELECT_CMD
///
int handle_select_cmd_like(Table_Like_t *table, Command_t *cmd) {
    cmd->type = SELECT_CMD;
    field_state_handler(cmd, 1);
	if (cmd->cmd_args.sel_args.where_begin == -1){
		print_likes(table, NULL, 0, cmd);
	}
	else {
		int *idxList ;
		idxList = malloc(sizeof(int) * table->len);
		size_t idxListLen;
		idxListLen = where_check_like(table,cmd,idxList);
		printf("idxListLen: %ld\n",idxListLen);
		print_likes(table, idxList, idxListLen, cmd);
		free(idxList);
    }
    return table->len;
}
