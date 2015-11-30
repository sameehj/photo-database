#include "wet.h"
#include <libpq-fe.h>

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

void* addUser(const char* name) {
	PGresult *res;
	char cmd[200];
	sprintf(cmd,
			"insert into users values ((select max(id)+1 from users),\'%s\')",
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
		int id = atoi(PQgetvalue(res, 0, 0));
		printf("%d\n", id);
	}
}
void* addUserMin(const char* name) {
}
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
void* addPhoto(const char* user_id, const char* photo_id) {
	PGresult *res;
	int userId = atoi(user_id);
	int photoId = atoi(photo_id);
	char cmd[200];
	//check if user exists
	sprintf(cmd, "select id from users where id=%d");
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
}
void* tagPhoto(const char* user_id, const char* photo_id, const char* info) {

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
