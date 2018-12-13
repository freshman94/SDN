#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <sys/times.h>
#include <errno.h>
#include "myconnector/include/mysql.h"
#include "myconnector/include/errmsg.h"
#include "myconnector/include/mysqld_error.h"


MYSQL conn;
MYSQL_RES *res_ptr;
MYSQL_ROW row;

void connection(const char* host, const char* user, const char* password, const char* database) {
	unsigned int timeout = 1;
	mysql_init(&conn);

	mysql_options(&conn, MYSQL_OPT_CONNECT_TIMEOUT, (const char *)&timeout);
	if (!mysql_real_connect(&conn, host, user, password, database, 0, NULL, 0)) {
		fprintf(stderr, "\nConnection to %s failed!\n", host);
		if (mysql_errno(&conn)) {
			fprintf(stderr, "Connection error %d: %s\n", mysql_errno(&conn), mysql_error(&conn));
		}
	}
	else {
		printf("\nConnection to %s success!\n", host);
	}
}

void print_Nodes(int time) {
	while (1) {
		int res = mysql_query(&conn, "select * from Nodes");
		if (res) {
			fprintf(stderr, "SELECT error: %s\n", mysql_error(&conn));
		}
		else {
			res_ptr = mysql_store_result(&conn);
			if (res_ptr) {
				printf("Retrieved %lu rows\n", (unsigned long)mysql_num_rows(res_ptr));
				printf("===================================Nodes==================================\n");
				printf("  IP_addr            Mac_addr     RX_rate(B/s)   TX_rate(B/s)        BER      PLR\n");
				while ((row = mysql_fetch_row(res_ptr))) {
					printf("%-15s %-20s %-10s %-10s %-10s %-10s\n", row[0], row[1], row[2], row[3], row[4], row[5]);
				}
				if (mysql_errno(&conn)) {
					fprintf(stderr, "Retrive error: %s\n", mysql_error(&conn));
				}
				mysql_free_result(res_ptr);
			}
		}
		sleep(time);
	}
}

void print_node(int time) {
	while (1) {
		int res = mysql_query(&conn, "select * from node");
		if (res) {
			fprintf(stderr, "SELECT error: %s\n", mysql_error(&conn));
		}
		else {
			res_ptr = mysql_store_result(&conn);
			if (res_ptr) {
				printf("Retrieved %lu rows\n", (unsigned long)mysql_num_rows(res_ptr));
				printf("===================================node==================================\n");
				printf("  IP_addr            Mac_addr     RX_rate(B/s)   TX_rate(B/s)        BER      PLR\n");
				while ((row = mysql_fetch_row(res_ptr))) {
					printf("%-15s %-20s %-10s %-10s %-10s %-10s\n", row[0], row[1], row[2], row[3], row[4], row[5]);
				}
				if (mysql_errno(&conn)) {
					fprintf(stderr, "Retrive error: %s\n", mysql_error(&conn));
				}
				mysql_free_result(res_ptr);
			}
		}
		sleep(time);
	}	
}

void print_Links(int time) {
	while (1) {
		int res = mysql_query(&conn, "select * from Links");
		if (res) {
			fprintf(stderr, "SELECT error: %s\n", mysql_error(&conn));
		}
		else {
			res_ptr = mysql_store_result(&conn);
			if (res_ptr) {
				printf("Retrieved %lu rows\n", (unsigned long)mysql_num_rows(res_ptr));
				printf("======================Links=======================\n");
				printf("  IP_src              IP_des        BandWidth(Mbit/sec)\n");
				while ((row = mysql_fetch_row(res_ptr))) {
					printf("%-20s %-20s %-10s\n", row[2], row[3], row[4]);
				}
				if (mysql_errno(&conn)) {
					fprintf(stderr, "Retrive error: %s\n", mysql_error(&conn));
				}
				mysql_free_result(res_ptr);
			}
		}
		sleep(time);
	}
}

void print_links(int time) {
	while (1) {
		int res = mysql_query(&conn, "select * from links");
		if (res) {
			fprintf(stderr, "SELECT error: %s\n", mysql_error(&conn));
		}
		else {
			res_ptr = mysql_store_result(&conn);
			if (res_ptr) {
				printf("Retrieved %lu rows\n", (unsigned long)mysql_num_rows(res_ptr));
				printf("======================links=======================\n");
				printf("  IP_src              IP_des        BandWidth(Mbit/sec)\n");
				while ((row = mysql_fetch_row(res_ptr))) {
					printf("%-20s %-20s %-10s\n", row[2], row[3], row[4]);
				}
				if (mysql_errno(&conn)) {
					fprintf(stderr, "Retrive error: %s\n", mysql_error(&conn));
				}
				mysql_free_result(res_ptr);
			}
		}
		sleep(time);
	}
}

void print_topology(int time) {
	while (1) {
		int res = mysql_query(&conn, "select * from topology");
		if (res) {
			fprintf(stderr, "SELECT error: %s\n", mysql_error(&conn));
		}
		else {
			res_ptr = mysql_store_result(&conn);
			if (res_ptr) {
				printf("Retrieved %lu rows\n", (unsigned long)mysql_num_rows(res_ptr));
				printf("======================topology=======================\n");
				printf("  IP_src              IP_des        Connected\n");
				while ((row = mysql_fetch_row(res_ptr))) {
					printf("%-20s %-20s %-5s\n", row[0], row[1], row[2]);
				}
				if (mysql_errno(&conn)) {
					fprintf(stderr, "Retrive error: %s\n", mysql_error(&conn));
				}
				mysql_free_result(res_ptr);
			}
		}
		sleep(time);
	}
}

int main(int argc, char *argv[]) {
	if (argc != 4) {
		fprintf(stderr, "usage ./print <Addr> <TableName> <time>\n");
		exit(1);
	}
	connection(argv[1], "root", "shujuku1", "linjiejuzhen");
	char * TableName = argv[2];
	int time = atoi(argv[3]);
	if (!strcmp(TableName, "node"))
		print_node(time);
	else if (!strcmp(TableName, "topology"))
		print_topology(time);
	else if (!strcmp(TableName, "links"))
		print_links(time);
	else if (!strcmp(TableName, "Links"))
		print_Links(time);
	else if (!strcmp(TableName, "Nodes"))
		print_Nodes(time);
	else
		printf("Unknown table!\n");

	mysql_close(&conn);
	return 0;
}