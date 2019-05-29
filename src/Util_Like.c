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
	
	///////       	UNFINISHED  		///////       	
	///////			UNFINISHED  		///////  
	///////			UNFINISHED  		///////  
	///////			UNFINISHED  		///////  
	///////			UNFINISHED  		///////  
	///////			UNFINISHED  		///////  
	///////			UNFINISHED  		///////  
	
	/*
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
					Like_t* user = get_Like(table, idxList[idx]);
					sum += user->id;
				}
				printf("%.3f",sum/idxListLen);
			}
			else{
				double sum = 0 ;
				for (idx = 0 ; idx < table->len; idx++)
				{
					Like_t* user = get_Like(table, idx);
					sum += user->id;
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
					Like_t* user = get_Like(table, idxList[idx]);
					sum += user->age;
				}
				printf("%.3f",sum/idxListLen);
			}
			else{
				double sum = 0 ;
				for (idx = 0 ; idx < table->len; idx++)
				{
					Like_t* user = get_Like(table, idx);
					sum += user->age;
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
					Like_t* user = get_Like(table, idxList[idx]);
					sum += user->id;
				}
				printf("%d",sum);
			}
			else{
				int sum = 0 ;
				for (idx = 0 ; idx < table->len; idx++)
				{
					Like_t* user = get_Like(table, idx);
					sum += user->id;
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
					Like_t* user = get_Like(table, idxList[idx]);
					sum += user->age;
				}
				printf("%d",sum);
			}
			else{
				int sum = 0 ;
				for (idx = 0 ; idx < table->len; idx++)
				{
					Like_t* user = get_Like(table, idx);
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
		
	}*/
	
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
	// if (idxList == NULL)
		// idxList = malloc(sizeof(int) * table->len);
	// int operator_mat[10];
	// int op_i = 0;
	// int cond_mat[10];
	// int cond_i = 0;
	// double number_mat[10];
	// char char_mat[10][100];
	// int num_i = 0;
	// int logic_mat[10];
	// int logic_i = 0;
	// size_t idxListLen = 0;
	// int cond_begin = cmd->cmd_args.sel_args.where_begin;
	// int cond_end = cmd->cmd_args.sel_args.where_end;
	// int index_char = 0;
	// int index_cmd = 0;
	// while(index_cmd <= cond_end){
		// char temp[200];
		// index_char = 0;
		// for (index_cmd = cond_begin ; index_cmd <= cond_end; index_cmd++)
		// {
			// if (!strncmp(cmd->args[index_cmd], "or", 2)){
				// logic_mat[logic_i++] = 1;
				// cond_begin = index_cmd +1;
				// break;
			// }
			// else if (!strncmp(cmd->args[index_cmd], "and", 3)){
				// logic_mat[logic_i++] = 2;
				// cond_begin = index_cmd +1;
				// break;
			// }
			// if(index_cmd == cond_begin)
				// strcpy(temp,cmd->args[index_cmd]);
			// else
				// strcat(temp,cmd->args[index_cmd]);
		// }
		// if (!strncmp(temp, "id", 2))
		// {
			// index_char = 2;
			// cond_mat[cond_i++] = 1;
		// }
		// if (!strncmp(temp, "name", 4))
		// {
			// index_char = 4;
			// cond_mat[cond_i++] = 2;
		// }
		// if (!strncmp(temp, "email", 5))
		// {
			// index_char = 5;
			// cond_mat[cond_i++] = 3;
		// }
		// if (!strncmp(temp, "age", 3))
		// {
			// index_char = 3;
			// cond_mat[cond_i++] = 4;
		// }
		////////////////////////// logical operator ///////////////////////
		// if (temp[index_char] == '=')
		// {
			// operator_mat[op_i++] = 1;
		// }
		// if (temp[index_char] == '!')
		// {
			// if(temp[index_char+1] == '='){
				// operator_mat[op_i++] = 2;
				// index_char++;
			// }
		// }
		// if (temp[index_char] == '>')
		// {
			// if(temp[index_char+1] == '='){
				// operator_mat[op_i++] = 5;
				// index_char++;
			// }
			// else
				// operator_mat[op_i++] = 3;
		// }
		// if (temp[index_char] == '<')
		// {
			// if(temp[index_char+1] == '='){
				// operator_mat[op_i++] = 6;
				// index_char++;
			// }
			// else
				// operator_mat[op_i++] = 4;
		// }
		// index_char++;
		/////////////////////////////////////////
		// int temp_n = 0 ;
		// int dot_n = 0 ; // number after dot
		// int flag = 0;
		// if(temp[index_char]=='"')
		// {
			// char *ptr = temp + index_char;
			// strcpy(char_mat[num_i], ptr);
			// num_i++;
		// }
		// else {
			// while (temp[index_char] != 0)
			// {
				// if (flag)
					// dot_n++;
				// temp_n *= 10;
				// temp_n += temp[index_char] - '0';
				// index_char++;
				// if (temp[index_char] == '.'){
					// flag = 1;
					// index_char++;
				// }
			// }
			// double number = temp_n;
			// if (flag)
			// {
				// for (int i = 0 ; i < dot_n ; i++)
					// number /=10;
			// }
			// number_mat[num_i++] = number;
		// }		
	// }
	////////////////////end translation /////////////////////////////
	// if(op_i != cond_i || op_i != num_i || op_i != logic_i + 1)
	// {
		// printf("error op cond num error!\n");
		// cmd->type = UNRECOG_CMD;
		// return table->len;
	// }
	// for (int idx = 0 ; idx < table->len; idx++)
	// {
		// Like_t* User = get_Like(table, idx);
		// int flag1 = 0 ;
		// int flag2 = 0 ;
		// for(int i = 0 ; i < op_i;i++){
			// double number = number_mat[i];
			// if (cond_mat[i] == 1){
				// if(operator_mat[i]==1){
					// if(User->id == number)
						// flag2 = 1;
					// else
						// flag2 = 0;
				// }
				// if(operator_mat[i]==2){
					// if(User->id != number)
						// flag2 = 1;
					// else
						// flag2 = 0;
				// }
				// if(operator_mat[i]==3){
					// if(User->id > number)
						// flag2 = 1;
					// else
						// flag2 = 0;
				// }
				// if(operator_mat[i]==4){
					// if(User->id < number)
						// flag2 = 1;
					// else
						// flag2 = 0;
				// }
				// if(operator_mat[i]==5){
					// if(User->id >= number)
						// flag2 = 1;
					// else
						// flag2 = 0;
				// }
				// if(operator_mat[i]==6){
					// if(User->id <= number)
						// flag2 = 1;
					// else
						// flag2 = 0;
				// }
			// }
			// else if (cond_mat[i] ==2){
				// if(operator_mat[i]==1){
					// if(!strcmp(User->name,char_mat[i]))
						// flag2 = 1;
					// else
						// flag2 = 0;
				// }
				// if(operator_mat[i]==2){
					// if(strcmp(User->name,char_mat[i]))
						// flag2 = 1;
					// else
						// flag2 = 0;
				// }
			// }
			// else if (cond_mat[i] == 3){
				// if(operator_mat[i]==1){
					// if(!strcmp(User->email,char_mat[i]))
						// flag2 = 1;
					// else
						// flag2 = 0;
				// }
				// if(operator_mat[i]==2){
					// if(strcmp(User->email,char_mat[i]))
						// flag2 = 1;
					// else
						// flag2 = 0;
				// }
			// }
			// else if (cond_mat[i] ==4){
				// if(operator_mat[i]==1){
					// if(User->age == number)
						// flag2 = 1;
					// else
						// flag2 = 0;
				// }
				// if(operator_mat[i]==2){
					// if(User->age != number)
						// flag2 = 1;
					// else
						// flag2 = 0;
				// }
				// if(operator_mat[i]==3){
					// if(User->age > number)
						// flag2 = 1;
					// else
						// flag2 = 0;
				// }
				// if(operator_mat[i]==4){
					// if(User->age < number)
						// flag2 = 1;
					// else
						// flag2 = 0;
				// }
				// if(operator_mat[i]==5){
					// if(User->age >= number)
						// flag2 = 1;
					// else
						// flag2 = 0;
				// }
				// if(operator_mat[i]==6){
					// if(User->age <= number)
						// flag2 = 1;
					// else
						// flag2 = 0;
				// }
			// }
			// if(i == 0)
				// flag1 = flag2;
			// else
			// {
				// if(logic_mat[i-1]==1)  // or
					// flag1 |= flag2;
				// else if (logic_mat[i-1]==2)
					// flag1 &= flag2;
			// }
		// }
		// if (flag1)
			// idxList[idxListLen++] = idx;
	// }
	// return idxListLen;
	return 0;
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
		print_likes(table, idxList, idxListLen, cmd);
		free(idxList);
    }
    return table->len;
}
