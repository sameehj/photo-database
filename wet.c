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
	sprintf(cmd, "insert into users values ((select max(id)+1 from users),\'%s\')",name);
	res = PQexec(conn,cmd);
	if(!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
		fprintf(stderr, "Error executing query: %s\n",PQresultErrorMessage(res));
		PQclear(res);
		return;
	}
}
void* addUserMin(const char* name) {

}
void* removeUser(const char* id) {

}
void* addPhoto(const char* user_id, const char* photo_id) {

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
