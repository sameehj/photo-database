#include "wet.h"
#include <libpq-fe.h>
#include <stdlib.h>
void print_table(const char* header,const char* result_template,PGresult *res,int row,int col);
PGconn *conn;
int main(int arc, char **argv) {
	char connect_param[200];
	sprintf(connect_param,
			"host=csl2.cs.technion.ac.il dbname=%s user=%s password=%s",
			USERNAME, USERNAME, PASSWORD);
	conn = PQconnectdb(connect_param);
	parseInput();
	return 0;
}

//TODO : if there is an Error Executing the second command, should return the database into prior execution state - maybe we can assume it will work?
void* addUser(const char* name) {
	PGresult *res;
	char cmd[200];
	//TODO : fix the case where there is no users. what max(id) returns? - Done!
	sprintf(cmd,
			"insert into users values (coalesce((select max(id)+1 from users),0),\'%s\')",
			name);
	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "Error executing query: %s\n",
				PQresultErrorMessage(res));
		PQclear(res);
		return;
	} else {
		sprintf(cmd,
				"select id from users where id=(select max(id) from users)");
		res = PQexec(conn, cmd);
		if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
			fprintf(stderr, "Error executing query: %s\n",
					PQresultErrorMessage(res));
			PQclear(res);
			
			return;
		}
		printf(ADD_USER, PQgetvalue(res, 0, 0));
	}
}
//TODO : if there is an Error Executing the second command, should return the database into prior execution state
void* addUserMin(const char* name) {
	PGresult *res;
	char cmd[200];
	sprintf(cmd,
			"insert into users values (coalesce((select min(id)+1 from users where users.id not in(select u1.id from users as u1,users as u2 where u1.id = u2.id - 1)),0),\'%s\')",
			name);
	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "Error executing query: %s\n",
				PQresultErrorMessage(res));
		PQclear(res);
		return;
	} else {
		sprintf(cmd,
				"select * from users where name=\'%s\' order by id asc",name);
		res = PQexec(conn, cmd);
		if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
			fprintf(stderr, "Error executing query: %s\n",
					PQresultErrorMessage(res));
			PQclear(res);
			return;
		}
		print_table(USER_HEADER,USER_RESULT,res,PQntuples(res),PQnfields(res));
	}
}
//TODO : if the user doesn't Exist ==> print ILL_PARAMS
void* removeUser(const char* id) {
	PGresult *res;
	char cmd[200];
	sprintf(cmd,
			"delete from users where id=%d;delete from photos where user_id=%d",
			atoi(id), atoi(id));
	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "Error executing query: %s\n",
				PQresultErrorMessage(res));
		PQclear(res);
		return;
	}
}
//TODO : if there is an Error Executing the second command, should return the database into prior execution state
void* addPhoto(const char* user_id, const char* photo_id) {
	PGresult *res;
	int userId = atoi(user_id);
	int photoId = atoi(photo_id);
	char cmd[200];
	//check if user exists
	sprintf(cmd, "select id from users where id=%d",userId);
	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "Error executing query: %s\n",
				PQresultErrorMessage(res));
		PQclear (res);
		return;
	}
	if(PQntuples(res)==0){
		printf("%s",ILL_PARAMS);
		return;
	}
	//check if photo exists
	sprintf(cmd, "select user_id,id from photos where user_id=%d and id=%d",
			userId, photoId);
	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "Error executing query: %s\n",
				PQresultErrorMessage(res));
		PQclear (res);
		return;
	}
	if(PQntuples(res)!=0){
		printf("%s",EXISTING_RECORD);
		return;
	}
	// the user exists anf the phoyo doesn't exist - add it.
	sprintf(cmd, "insert into photos (user_id,id) values (%d,%d)",
			userId, photoId);
	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "Error executing query: %s\n",
				PQresultErrorMessage(res));
		PQclear (res);
		return;
	}						
}
void* tagPhoto(const char* user_id, const char* photo_id, const char* info) {
	PGresult *res;
	int userId = atoi(user_id);
	int photoId = atoi(photo_id);
	char cmd[200];
	//check if user exists
	sprintf(cmd, "select id from users where id=%d",userId);
	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "Error executing query: %s\n",
				PQresultErrorMessage(res));
		PQclear (res);
		return;
	}
	if(PQntuples(res)==0){
		printf("%s",ILL_PARAMS);
		return;
	}
	//check if info exists
	sprintf(cmd, "select user_id,photo_id,info from tags where user_id=%d and photo_id=%d and info='%s'",
			userId, photoId,info);
	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "Error executing query: %s\n",
				PQresultErrorMessage(res));
		PQclear (res);
		return;
	}
	if(PQntuples(res)!=0){
		printf("%s",EXISTING_RECORD);
		return;
	}
	// the user exists anf the phoyo doesn't exist - add it.
	sprintf(cmd, "insert into tags (photo_id,user_id,info) values (%d,%d,'%s')",
			photoId, userId,info);
	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "Error executing query: %s\n",
				PQresultErrorMessage(res));
		PQclear (res);
		return;
	}
}
void* photosTags() {

}
void* search(const char* word) {

}
void* commonTags(const char* k) {

}
void* mostCommonTags(const char* k) {

}
void* similarPhotos(const char* k, const char* j) {

}
void* autoPhotoOnTagOn() {

}
void* autoPhotoOnTagOFF() {

}
void print_table(const char* header,const char* result_template,PGresult *res,int row,int col){
	printf(header);
	int i=0;
	for(i=0;i<row;i++){
		if(col == 2)
			printf(result_template,PQgetvalue(res, i, 0),PQgetvalue(res, i, 1));
		else if(col == 1)
			printf(result_template,PQgetvalue(res, i, 0),PQgetvalue(res, i, 1),PQgetvalue(res, i, 2));			
		else exit(0);
	}
}
