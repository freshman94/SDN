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

void unix_error(char *msg)  //错误处理函数
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(1);
}

static void
pr_times(clock_t real, struct tms *tmsstart, struct tms *tmsend)
{
	static long	clktck = 0;

	if (clktck == 0)	/* fetch clock ticks per second first time */
		if ((clktck = sysconf(_SC_CLK_TCK)) < 0)
			unix_error("sysconf error");
	printf("=====================Time used======================\n");
	//printf("  real:  %7.2f s\n", real / (double)clktck);
	printf("  user:  %7.2f s\n",
		(tmsend->tms_utime - tmsstart->tms_utime) / (double)clktck);
	printf("  sys:   %7.2f s\n",
		(tmsend->tms_stime - tmsstart->tms_stime) / (double)clktck);
}

int  main() {
	while (1) {
		const int N = 6;
		char passwd[] = "shujuku1";
		char* sumeipai_ipaddr[] = { "192.168.1.2","192.168.2.2","192.168.3.2","192.168.4.2" ,"192.168.5.2" ,"192.168.6.2"};

		int* status = (int *)malloc(sizeof(int));
		//写入表Nodes

		clock_t start, end;
		struct tms tmsstart, tmsend;
		long real_time = 0, user_time = 0, sys_time = 0;
		if ((start = times(&tmsstart)) == -1)
			unix_error("times error\n");

		int i;
		for (i = 0; i < N; ++i) {
			connection(sumeipai_ipaddr[i], "root", passwd, "linjiejuzhen", status);
		}

		free(status);
		if ((end = times(&tmsend)) == -1)
			unix_error("times error");
		pr_times(end - start, &tmsstart, &tmsend);
	}
	return 0;
}