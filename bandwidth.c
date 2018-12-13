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



/*
提取数据信息
*/
char * praseFields(char *pSrc, char *pField, char *pValue)
{
	char *lpReturn = 0;

	if (strstr(pSrc, pField) == pSrc)
	{
		pSrc += strlen(pField);

		while (*(++pSrc) == ' ');

		char *lpSpace = strstr(pSrc, " ");
		if (lpSpace != 0)
		{
			if (lpSpace > pSrc)
			{
				if (*(lpSpace - 1) == '\n')
				{
					lpSpace--;
				}

				memcpy(pValue, pSrc, lpSpace - pSrc);
				pValue[lpSpace - pSrc] = 0;

				lpReturn = lpSpace + 1;
			}
			else
			{
				// do nothing.
			}
		}
		else
		{
			// do nothing.
		}
	}
	else
	{
		// do nothing.
	}


	return (lpReturn);
}

/*
获取主机的IP
*/
void get_ipaddr(char* iface, char* ip_addr) {
	char cmd[50] = "";
	sprintf(cmd, "ifconfig %s > IPaddr_out.txt", iface);
	system(cmd);
	sleep(1);   // wait for ifconfig cmd done.

	FILE* pFd;
	pFd = fopen("IPaddr_out.txt", "rb");
	char cBuf[1024 * 10] = { 0 };
	fread(cBuf, 1, 1024 * 10, pFd);
	fclose(pFd);

	char *lpSrc = strstr(cBuf, "inet addr");
	if (lpSrc != 0)
	{
		if ((lpSrc = praseFields(lpSrc, "inet addr", ip_addr)) != 0)
		{
			printf("Get the address:\n");
			printf("inet address: [%s]\n", ip_addr);
		}
		else
		{
			printf("Invalid address field.\n");
			printf("inet address: [%s]\n", ip_addr);
		}
	}
	else
	{
		printf("Can not find addr field.\n");
	}
}


/*
获取带宽
*/
void get_bandwidth(char* server_ip, char* bandwidth)
{
	char cmd[255] = { 0 };
	sprintf(cmd, "iperf -c %s  -t 2 -f m  | grep -A2 'Interval' > iperf_out.txt ", server_ip);
	system(cmd);
	sleep(3);	// wait for iperf cmd done
	FILE* pFd = fopen("iperf_out.txt", "rb");
	char cBuf[1024 * 10] = { 0 };
	fread(cBuf, 1, 1024 * 10, pFd);
	fclose(pFd);
	char *lpSrc = strstr(cBuf, "Bytes");

	if (lpSrc != 0) {
		if ((lpSrc = praseFields(lpSrc, "Bytes", bandwidth)) != 0) {
			printf("Get the BandWidth [%s]:\n", server_ip);
			printf("BandWidth: [%s Mbits/sec]\n\n", bandwidth);
		}
		else
		{
			printf("Invalid field. [%s]\n", server_ip);
			printf("Check the fields:\n");
			printf("BandWidth: [%s Mbits/sec]\n", bandwidth);
		}
	}
	else
	{
		printf("Can not find [%s]!!!\n", server_ip);
	}
}

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

void insert_links( char *IP_src, char* IP_des, char* BandWidth) {
	unsigned int timeout = 1;
	mysql_options(&conn, MYSQL_OPT_WRITE_TIMEOUT, (const char *)&timeout);

	char cmd[255] = { 0 };
	sprintf(cmd, "INSERT INTO Links(IP_src,IP_des,BandWidth) VALUES('%s','%s','%s')", IP_src, IP_des, BandWidth);
	int res = mysql_query(&conn, cmd);
	if (!res) {
		printf("Insert into Table Links: %lu rows\n", (unsigned long)mysql_affected_rows(&conn));
	}
	else {
		fprintf(stderr, "Insert into Table Links:error %d: %s\n", mysql_errno(&conn), mysql_error(&conn));
	}
}

void update_links( char *IP_src, char* IP_des, char* BandWidth) {
	unsigned int timeout = 1;
	mysql_options(&conn, MYSQL_OPT_WRITE_TIMEOUT, (const char *)&timeout);

	char cmd[255] = { 0 };
	sprintf(cmd, "UPDATE Links SET BandWidth='%s' WHERE IP_src='%s' AND IP_des='%s'", BandWidth, IP_src, IP_des);
	int res = mysql_query(&conn, cmd);
	if (!res) {
		printf("Update Table Links: %lu rows\n", (unsigned long)mysql_affected_rows(&conn));
	}
	else {
		fprintf(stderr, "Update Table Links: error %d: %s\n", mysql_errno(&conn), mysql_error(&conn));
	}
}



int get_count(const char* IP_src,const char* IP_des) {
	MYSQL_RES *res_ptr;
	MYSQL_ROW sqlrow;

	char cmd[255] = { 0 };
	sprintf(cmd, "select count(*) from Links where IP_src = '%s' and IP_des = '%s' group by IP_src", IP_src,IP_des);
	int res = mysql_query(&conn, cmd);
	int count;

	if (res) {
		fprintf(stderr, "SELECT error(database: NSInfo): %s\n", mysql_error(&conn));
	}
	else {
		res_ptr = mysql_use_result(&conn);
		if (res_ptr) {
			if ((sqlrow = mysql_fetch_row(res_ptr))) {
				count = atoi(sqlrow[0]);
			}
			else
				count = 0;
			if (mysql_errno(&conn))
				fprintf(stderr, "Retrive error(database: NSInfo): %s\n", mysql_error(&conn));
			mysql_free_result(res_ptr);
		}
	}
	return count;
}

int hasPath(const char* IP_src, const char* IP_des) {
	MYSQL_RES *res_ptr;
	MYSQL_ROW sqlrow;

	char cmd[255] = { 0 };
	sprintf(cmd, "select `%s` from topology where IP_addr = '%s'", IP_src, IP_des);
	int res = mysql_query(&conn, cmd);
	int count;

	if (res) {
		fprintf(stderr, "SELECT error(database: linjiejuzhen): %s\n", mysql_error(&conn));
	}
	else {
		res_ptr = mysql_use_result(&conn);
		if (res_ptr) {
			if ((sqlrow = mysql_fetch_row(res_ptr))) {
				count = atoi(sqlrow[0]);
			}
			else
				count = 0;
			if (mysql_errno(&conn))
				fprintf(stderr, "Retrive error(database: linjiejuzhen): %s\n", mysql_error(&conn));
			mysql_free_result(res_ptr);
		}
	}
	return count;
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
	printf("  real:  %7.2f s\n", real / (double)clktck);
	printf("  user:  %7.2f s\n",
		(tmsend->tms_utime - tmsstart->tms_utime) / (double)clktck);
	printf("  sys:   %7.2f s\n",
		(tmsend->tms_stime - tmsstart->tms_stime) / (double)clktck);
}

int main(int argc, char *argv[]) {
	clock_t start, end;
	struct tms tmsstart, tmsend;
	long real_time = 0, user_time = 0, sys_time = 0;
	if ((start = times(&tmsstart)) == -1)
		unix_error("times error\n");

	char client_ipaddr[20] =  "" ;	//ip地址
	get_ipaddr(argv[1],client_ipaddr);		//获取主机IP地址
									//获取带宽
	const int N = 6;
	char routes_ipaddr[][20] = { "192.168.0.1","192.168.0.2","192.168.0.3","192.168.0.4","192.168.0.5","192.168.0.6" };
	char bandwidth[][20] = { "","" ,"" ,"" ,"" ,"" };
	char test[20] = "error";
	
	char passwd[] = "shujuku1";
	char* sumeipai_ipaddr[] = { "192.168.1.2","192.168.2.2","192.168.3.2","192.168.4.2" ,"192.168.5.2","192.168.6.2" };
	int* status = (int *)malloc(sizeof(int));
	for (int i = 0; i < N; ++i) {
		connection(sumeipai_ipaddr[i], "root", passwd, "linjiejuzhen", status);//从树莓派获取拓扑结构
		if()
	}

	
	if (*status1 == 0)
		return -1;
	
	for (int i = 0; i < N; ++i) {
		if (hasPath(client_ipaddr,routes_ipaddr[i]))
			get_bandwidth(routes_ipaddr[i], bandwidth[i]);
	}
	
	int* status = (int *)malloc(sizeof(int));

	//写入表Links
	for (int i = 0; i < N; ++i) {
		connection(sumeipai_ipaddr[i], "root", passwd, "linjiejuzhen", status);
		if (*status)
			for (int j = 0; j < N; ++j) {
				if (get_count(client_ipaddr, routes_ipaddr[j]))
					update_links(client_ipaddr, routes_ipaddr[j], bandwidth[j]);
				else
					insert_links(client_ipaddr, routes_ipaddr[j], bandwidth[j]);
			}
		mysql_close(&conn);
	}
	free(status);
	free(status1);

	if ((end = times(&tmsend)) == -1)
		unix_error("times error");
	pr_times(end - start, &tmsstart, &tmsend);

	return 0;
}