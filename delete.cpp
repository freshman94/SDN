#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include "myconnector/include/mysql.h"
#include "myconnector/include/errmsg.h"
#include "myconnector/include/mysqld_error.h"


MYSQL conn;

void connection(const char* host, const char* user, const char* password, const char* database, int* status) {
	unsigned int timeout = 1;
	mysql_init(&conn);

	mysql_options(&conn, MYSQL_OPT_CONNECT_TIMEOUT, (const char *)&timeout);
	if (!mysql_real_connect(&conn, host, user, password, database, 0, NULL, 0)) {
		*status = 0;
		fprintf(stderr, "\nConnection to %s failed!\n", host);
		if (mysql_errno(&conn)) {
			fprintf(stderr, "Connection error %d: %s\n", mysql_errno(&conn), mysql_error(&conn));
		}
	}
	else {
		*status = 1;
		printf("\nConnection to %s success!\n", host);
	}
}

void delete_NodesAll() {
	char cmd[255] = { 0 };
	sprintf(cmd, "truncate table NodesAll");
	int res = mysql_query(&conn, cmd);
	if (!res) {
		printf("Truncate %lu rows\n", (unsigned long)mysql_affected_rows(&conn));
	}
	else {
		fprintf(stderr, "Truncate error %d: %s\n", mysql_errno(&conn), mysql_error(&conn));
	}
}

int main() {
	char passwd[] = "shujuku1";
	char sumeipai_ipaddr[] = { "192.168.9.40" };

	int* status = (int *)malloc(sizeof(int));
	connection(sumeipai_ipaddr, "root", passwd, "NSInfo", status);
	if (*status)
		delete_NodesAll();
	mysql_close(&conn);

	return 0;
}


