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
	sprintf(cmd, "select id from users where id=%s",id);
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
	sprintf(cmd,
			"delete from users where id=%d;delete from photos where user_id=%d;delete from tags where user_id=%d",
			atoi(id), atoi(id),atoi(id));
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
	PGresult *res;
	char cmd[200];
	sprintf(cmd,"SELECT photo_id,user_id, COUNT(photo_id) AS dupe_cnt FROM tags GROUP BY photo_id,user_id HAVING COUNT(photo_id) > 0 ORDER BY COUNT(photo_id) DESC, user_id ASC,photo_id ASC");
	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "Error executing query: %s\n",
				PQresultErrorMessage(res));
		PQclear(res);
		return;
	}
	if(PQntuples(res) !=0){
		print_table(PHOTOS_HEADER,PHOTOS_RESULT,res,PQntuples(res),PQnfields(res));
	}
	else {
		printf(EMPTY);
	}
}
void* search(const char* word) {
	PGresult *res;
	char cmd[200];
	sprintf(cmd,"SELECT photo_id,user_id, COUNT(photo_id) AS dupe_cnt FROM tags WHERE info LIKE '%%%s%%' GROUP BY photo_id,user_id HAVING COUNT(photo_id) > 0 ORDER BY COUNT(photo_id) DESC, user_id ASC,photo_id DESC",word);
	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "Error executing query: %s\n",
				PQresultErrorMessage(res));
		PQclear(res);
		return;
	}
	if(PQntuples(res) != 0){
		print_table(PHOTOS_HEADER,PHOTOS_RESULT,res,PQntuples(res),PQnfields(res));
	}
	else {
		printf(EMPTY);
	}
}
void* commonTags(const char* k) {
	PGresult *res;
	char cmd[200];
	sprintf(cmd,"SELECT temp.info,temp.count from(SELECT info ,(COUNT(info)) FROM tags GROUP BY info) temp where count > %d ORDER BY temp.count DESC , temp.info ASC",atoi(k));
	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "Error executing query: %s\n",
				PQresultErrorMessage(res));
		PQclear(res);
		return;
	}
	if(PQntuples(res) !=0){
		print_table(COMMON_HEADER,COMMON_LINE,res,PQntuples(res),PQnfields(res));
	}
	else {
		printf(EMPTY);
	}
}
void* mostCommonTags(const char* k) {
	PGresult *res;
	char cmd[200];
	sprintf(cmd,"select info,count from (select *, (select count(*) from (select * from (select info,count(info) from tags group by info) as tt order by count desc, info asc) as  p2 where p1.count < p2.count or (p1.count = p2.count and p1.info > p2.info)) as cnt from (select * from (select info,count(info) from tags group by info) as tt1 order by count desc, info asc) as p1) as pp where pp.cnt < %s",k);
	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "Error executing query: %s\n",
				PQresultErrorMessage(res));
		PQclear(res);
		return;
	}
	if(PQntuples(res) !=0){
		print_table(COMMON_HEADER,COMMON_LINE,res,PQntuples(res),PQnfields(res));
	}
	else {
		printf(EMPTY);
	}
}
void* similarPhotos(const char* k, const char* j) {
	PGresult *res;
	char cmd[500];
	sprintf(cmd,"select user_id,n1.name,photo_id from (select user_id,photo_id,count(user_id) from (select t1.user_id,t1.photo_id,count(t1.user_id) from tags as t1,tags as t2 where t1.info = t2.info and not(t1.user_id = t2.user_id and t1.photo_id = t2.photo_id) group by t1.user_id,t2.user_id,t1.photo_id,t2.photo_id) as tagtag where tagtag.count > %s group by user_id,photo_id having count(user_id)>0) as res,users as n1 where n1.id = res.user_id and count > %s order by user_id asc,photo_id desc",j,k);
	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "Error executing query: %s\n",
				PQresultErrorMessage(res));
		PQclear(res);
		return;
	}
	if(PQntuples(res) !=0){
		print_table(SIMILAR_HEADER,SIMILAR_RESULT,res,PQntuples(res),PQnfields(res));
	}
	else {
		printf(EMPTY);
	}
}
void* autoPhotoOnTagOn() {
	PGresult *res;
	char cmd[200];
	sprintf(cmd,"CREATE OR REPLACE FUNCTION insert_missing_photo() RETURNS TRIGGER AS $$ BEGIN IF(NEW.photo_id NOT IN (SELECT photo_id FROM Tags)) THEN INSERT INTO photos VALUES(NEW.photo_id,NEW.user_id); END IF; RETURN NEW; END; $$ LANGUAGE plpgsql; CREATE TRIGGER auto_photo BEFORE INSERT ON tags FOR EACH ROW EXECUTE PROCEDURE insert_missing_photo();");
	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "Error executing query: %s\n",
				PQresultErrorMessage(res));
		PQclear(res);
		return;
	}
}
void* autoPhotoOnTagOFF() {
	PGresult *res;
	char cmd[200];
	sprintf(cmd,"DROP TRIGGER auto_photo on tags; DROP FUNCTION insert_missing_photo();");
	res = PQexec(conn, cmd);
	if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "Error executing query: %s\n",
				PQresultErrorMessage(res));
		PQclear(res);
		return;
	}
}
void print_table(const char* header,const char* result_template,PGresult *res,int row,int col){
	printf(header);
	int i=0;
	for(i=0;i<row;i++){
		if(col == 2)
			printf(result_template,PQgetvalue(res, i, 0),PQgetvalue(res, i, 1));
		else if(col == 3)
			printf(result_template,PQgetvalue(res, i, 0),PQgetvalue(res, i, 1),PQgetvalue(res, i, 2));			
		else exit(0);
	}
}
