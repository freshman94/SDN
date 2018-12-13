#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <limits.h>
#include <errno.h>
#include <sys/select.h>
#include "myconnector/include/mysql.h"
#include "myconnector/include/errmsg.h"
#include "myconnector/include/mysqld_error.h"


#define BUFLINE 1024*10
#define CMDLINE 50
void usage(const char * prog) {
	fprintf(stderr, "Usage: %s interface <timeout>\n", prog);
	exit(1);
}


void error(char * str) {
	perror(str);
	exit(1);
}


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
		}
	}
	return (lpReturn);
}

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

void get_bandwidth(char * server_ip, char * bandwidth, int timeout) {
	int fd[2];
	pid_t pid;
	char cmd[CMDLINE] = { 0 };
	sprintf(cmd, "iperf -c %s -t 2 -f m | grep -A2 'Interval'", server_ip);


	if (pipe(fd) < 0)
		error("pipe");

	if ((pid = fork()) < 0)
		error("Failed to create child");

	else if (pid > 0) { //parent
		close(fd[1]); //get output from child
		int fpin = fd[0];
		int readn;
		char cBuf[BUFLINE];
		time_t begin = time(NULL);

		while (1) {
			fd_set rset;
			struct timeval tv;

			time_t left = begin + timeout - time(NULL);

			if (left <= 0) {
				fprintf(stderr, "Timeout exit.\n");
				break;
			}

			tv.tv_sec = left;
			tv.tv_usec = 0;

			FD_ZERO(&rset);
			FD_SET(fpin, &rset);

			int selected = select(fpin + 1, &rset, NULL, NULL, &tv);

			if (selected > 0) {
				readn = read(fpin, cBuf, BUFLINE);
				if (readn < 0) {
					perror("read");
					break;
				}
				else if (readn == 0) {
					break;
				}
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
			kill(pid, SIGKILL); //kill the subprocess
			wait(NULL);
		}
	}
	else {	//child
		close(fd[0]);

		//dup2 stdin,stdout
		if (fd[1] != STDOUT_FILENO) {
			if (dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO)
				error("dup2 to stdout error");
		}

		if (fd[1] != STDERR_FILENO) {
			if (dup2(fd[1], STDERR_FILENO) != STDERR_FILENO)
				error("dup2 to stdout error");
			close(fd[1]);
		}

		execl("/bin/sh", "sh", "-c", cmd, (char *)0);
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

void insert_links(char *IP_src, char* IP_des, char* BandWidth) {
	unsigned int timeout = 1;
	mysql_options(&conn, MYSQL_OPT_WRITE_TIMEOUT, (const char *)&timeout);

	char cmd[255] = { 0 };
	sprintf(cmd, "INSERT INTO links(IP_src,IP_des,BandWidth) VALUES('%s','%s','%s')", IP_src, IP_des, BandWidth);
	int res = mysql_query(&conn, cmd);
	if (!res) {
		printf("Insert into Table links: %lu rows\n", (unsigned long)mysql_affected_rows(&conn));
	}
	else {
		fprintf(stderr, "Insert into Table links:error %d: %s\n", mysql_errno(&conn), mysql_error(&conn));
	}
}

void update_links(char *IP_src, char* IP_des, char* BandWidth) {
	unsigned int timeout = 1;
	mysql_options(&conn, MYSQL_OPT_WRITE_TIMEOUT, (const char *)&timeout);

	char cmd[255] = { 0 };
	sprintf(cmd, "UPDATE links SET BandWidth='%s' WHERE IP_src='%s' AND IP_des='%s'", BandWidth, IP_src, IP_des);
	int res = mysql_query(&conn, cmd);
	if (!res) {
		printf("Update Table links: %lu rows\n", (unsigned long)mysql_affected_rows(&conn));
	}
	else {
		fprintf(stderr, "Update Table links: error %d: %s\n", mysql_errno(&conn), mysql_error(&conn));
	}
}

int hasPath(const char* IP_src, const char* IP_des) {
	MYSQL_RES *res_ptr;
	MYSQL_ROW sqlrow;

	char cmd[255] = { 0 };
	sprintf(cmd, "select Connect from topology where IP_src = '%s' and IP_des = '%s'", IP_src, IP_des);
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

int get_count(const char* IP_src, const char* IP_des) {
	MYSQL_RES *res_ptr;
	MYSQL_ROW sqlrow;

	char cmd[255] = { 0 };
	sprintf(cmd, "select count(*) from links where IP_src = '%s' and IP_des = '%s' group by IP_src", IP_src, IP_des);
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

/*
获取主机号
*/

int get_hostid(char* IP)
{
	char *host_id = 0;
	host_id = strrchr(IP, '.');
	return atoi(host_id + 1);
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

	const char * prog = argv[0];

	if (argc != 3)
		usage(prog);

	char client_ipaddr[20] = "";	//ip地址
	get_ipaddr(argv[1], client_ipaddr);		//获取主机IP地址

	int timeout = atoi(argv[2]);

	printf("Executing cmd for timeout control of %d seconds....\n", timeout);

	int hostid = get_hostid(client_ipaddr);

	const int N = 6;
	char routes_ipaddr[][20] = { "192.168.0.1","192.168.0.2","192.168.0.3","192.168.0.4","192.168.0.5","192.168.0.6" };
	char bandwidth[][20] = { "1","1" ,"1" ,"1" ,"1" ,"1" };
	char test[20] = "error";

	char passwd[] = "shujuku1";
	char* sumeipai_ipaddr[] = { "192.168.1.2","192.168.2.2","192.168.3.2","192.168.4.2" ,"192.168.5.2","192.168.6.2" };
	int* status = (int *)malloc(sizeof(int));
	int pos = hostid - 1;// 根据主机的主机号获取相应的树莓派地址下标
	connection(sumeipai_ipaddr[pos], "root", passwd, "linjiejuzhen", status);//从树莓派获取拓扑结构
	if (*status == 0)
		return -1;

	FILE* fp = fopen("bd_out.txt", "w");
	int rc, i,j;
	for (i = 0; i < N; ++i) {
		if (hasPath(client_ipaddr, routes_ipaddr[i])) {
			get_bandwidth(routes_ipaddr[i], bandwidth[i], timeout);
			if (rc = fprintf(fp, "%s %s %s ", client_ipaddr,routes_ipaddr[i],bandwidth[i]) < 0)
				fprintf(stderr, "written to bd_out.txt error!!!");
		}
			
	}

	
		
	fclose(fp);

	*status = 0;

	//写入表links
	for (i = 0; i < N; ++i) {
		connection(sumeipai_ipaddr[i], "root", passwd, "linjiejuzhen", status);
		if (*status)
			for ( j = 0; j < N; ++j) {
				if (hasPath(client_ipaddr, routes_ipaddr[j])) {
					if (get_count(client_ipaddr, routes_ipaddr[j]))
						update_links(client_ipaddr, routes_ipaddr[j], bandwidth[j]);
					else
						insert_links(client_ipaddr, routes_ipaddr[j], bandwidth[j]);
				}
			}
		mysql_close(&conn);
	}
	free(status);

	if ((end = times(&tmsend)) == -1)
		unix_error("times error");
	pr_times(end - start, &tmsstart, &tmsend);
	return 0;
}
