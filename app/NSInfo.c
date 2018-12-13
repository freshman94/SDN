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
提取数据
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
获取误码率，丢包率
*/
int myAtoi(char *str) {
	int base = 0, i = 0, sign = 1;
	while (str[i] == ' ') ++i;     //1.忽略字符串开头的空格
	if (str[i] == '-' || str[i] == '+')  //2.字符串开头的符号位
		sign = 1 - 2 * (str[i++] == '-');
	while (str[i] >= '0' && str[i] <= '9') {  //3.有效的输入（'0'~'9'）
		if (base> INT_MAX / 10 || (base == INT_MAX / 10 && str[i]>'7')) {   //4.溢出
			if (sign == 1) return INT_MAX;
			else    return INT_MIN;
		}
		base = str[i++] - '0' + base * 10;
	}
	return base * sign;
}

void ifconfig_messages(char* iface,double* mes)
{
	char cmd[50] = "";
	sprintf(cmd, "ifconfig %s > ifconfig_out.txt", iface);
	system(cmd);
	sleep(1);   // wait for ifconfig cmd done.

	FILE* pFd;
	pFd = fopen("ifconfig_out.txt", "rb");
	char cBuf[1024 * 10] = { 0 };
	fread(cBuf, 1, 1024 * 10, pFd);
	fclose(pFd);

	// debug:
	// printf("out.txt\n%s\n\n", cBuf);

	char cPackets[10] = { "error" };
	char cErrors[10] = { "error" };
	char cDropped[10] = { "error" };

	// example:
	// RX packets:1737 errors:0 dropped:0 overruns:0 frame:0

	char *lpSrc = strstr(cBuf, "RX packets");
	if (lpSrc != 0)
	{
		if ((lpSrc = praseFields(lpSrc, "RX packets", cPackets)) != 0
			&& (lpSrc = praseFields(lpSrc, "errors", cErrors)) != 0
			&& (lpSrc = praseFields(lpSrc, "dropped", cDropped)) != 0)
		{
			printf("Get the RX packets fields:\n");
			printf("RX packets: [%s]\n", cPackets);
			printf("    errors: [%s]\n", cErrors);
			printf("   dropped: [%s]\n", cDropped);
			long Packets = myAtoi(cPackets);
			long Errors = myAtoi(cErrors);
			long Dropped = myAtoi(cDropped);
			if (Packets == 0) {
				*mes = 0.0;
				*(mes + 1) = 0.0;
			}
			else {
				*mes = (double)Errors / (double)Packets;
				*(mes + 1) = (double)Dropped / (double)Packets;
			}
			printf("Bit Error Rate:[%f]\n", mes[0]);
			printf("Packet Loss Rate:[%f]\n", mes[1]);

		}
		else
		{
			printf("Invalid RX packets field.\n");

			printf("Check the RX packets fields:\n");
			printf("RX packets: [%s]\n", cPackets);
			printf("    errors: [%s]\n", cErrors);
			printf("   dropped: [%s]\n", cDropped);
		}
	}
	else
	{
		printf("Can not find RX packets field.\n");
	}
}



/*
获取主机的IP地址和Mac地址
*/
void get_addr(char* iface, char addr[][50]) {
	char cmd[50] = "";
	sprintf(cmd, "ifconfig %s > ipaddr_out.txt", iface);
	system(cmd);
	sleep(1);   // wait for ifconfig cmd done.

	FILE* pFd;
	pFd = fopen("ipaddr_out.txt", "rb");
	char cBuf[1024 * 10] = { 0 };
	fread(cBuf, 1, 1024 * 10, pFd);
	fclose(pFd);

	char *lpSrc1 = strstr(cBuf, "HWaddr");
	char *lpSrc2 = strstr(cBuf, "inet addr");
	if ((lpSrc1 != 0) && (lpSrc2 !=0))
	{
		if ((lpSrc1 = praseFields(lpSrc1, "HWaddr", addr[0])) != 0
			&& (lpSrc2 = praseFields(lpSrc2, "inet addr", addr[1])) != 0)
		{
			printf("Get the address:\n");
			printf("Mac address: [%s]\n", addr[0]);
			printf("inet address: [%s]\n", addr[1]);
		}
		else
		{
			printf("Invalid address field.\n");

			printf("Mac address: [%s]\n", addr[0]);
			printf("inet address: [%s]\n", addr[1]);
		}
	}
	else
	{
		printf("Can not find addr field.\n");
	}

}


/*
获取传输速率
*/
void get_rate(char* iface, char rate[][20]) {
	char cmd[25] = { 0 };
	sprintf(cmd, "sh rate.sh %s > rate_out.txt", iface);
	system(cmd);
	sleep(1);

	FILE* pFd;
	pFd = fopen("rate_out.txt", "rb");
	char cBuf[1024 * 10] = { 0 };
	fread(cBuf, 1, 1024 * 10, pFd);
	fclose(pFd);

	char *lpSrc = strstr(cBuf, "RX");
	if (lpSrc != 0)
	{
		if ((lpSrc = praseFields(lpSrc, "RX", rate[0])) != 0
			&& (lpSrc = praseFields(lpSrc, "TX", rate[1])) != 0)
		{
			printf("RX rate: %s\n", rate[0]);
			printf("TX_rate: %s\n", rate[1]);

		}
		else
		{
			printf("Invalid rate field.\n");

			printf("RX rate: %s\n", rate[0]);
			printf("TX_rate: %s\n", rate[1]);
		}
	}
	else
	{
		printf("Can not find rate field.\n");
	}
}

/*
获取带宽
*/
void get_bandwidth(char* server_ip, char* bandwidth)
{
	char cmd[255] = { 0 };
	sprintf(cmd, "iperf -c %s  -t 5  | grep -A2 'Interval' > iperf_out.txt ", server_ip);
	system(cmd);
	sleep(10);	// wait for iperf cmd done
	FILE* pFd = fopen("iperf_out.txt", "rb");
	char cBuf[1024 * 10] = { 0 };
	fread(cBuf, 1, 1024 * 10, pFd);
	fclose(pFd);
	char *lpSrc = strstr(cBuf, "Bytes");

	if (lpSrc != 0) {
		if ((lpSrc = praseFields(lpSrc, "Bytes", bandwidth)) != 0) {
			printf("Get the BandWidth [%s]:\n",server_ip);
			printf("BandWidth: [%s]\n\n", bandwidth);
		}
		else
		{
			printf("Invalid field. [%s]\n",server_ip);
			printf("Check the fields:\n");
			printf("BandWidth: [%s]\n", bandwidth);
		}
	}
	else
	{
		printf("Can not find [%s]!!!\n",server_ip);
	}
}




/*
MySQL数据库表的插入、更新与删除
*/

MYSQL conn;

void connection(const char* host, const char* user, const char* password, const char* database, int* status) {
	unsigned int timeout = 3;
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

void insertAll_nodes(const char *IP_addr, const char *Mac_addr, const char* RX_rate, const char* TX_rate, double ber, double plr) {
	unsigned int timeout = 3;
	mysql_options(&conn, MYSQL_OPT_WRITE_TIMEOUT, (const char *)&timeout);

	char cmd[255] = { 0 };
	sprintf(cmd, "INSERT INTO NodesAll(IP_addr,Mac_addr,RX_rate,TX_rate,BER,PLR) VALUES('%s','%s','%s','%s',%f,%f)", IP_addr, Mac_addr, RX_rate, TX_rate, ber, plr);
	int res = mysql_query(&conn, cmd);
	if (!res) {
		printf("Insert into Table NodesALL: %lu rows\n", (unsigned long)mysql_affected_rows(&conn));
	}
	else {
		fprintf(stderr, "Insert into Table NodesAll: error %d: %s\n", mysql_errno(&conn), mysql_error(&conn));
	}
}

void insert_nodes(const char *IP_addr, const char *Mac_addr, const char* RX_rate, const char* TX_rate, double ber, double plr) {
	unsigned int timeout = 3;
	mysql_options(&conn, MYSQL_OPT_WRITE_TIMEOUT, (const char *)&timeout);

	char cmd[255] = { 0 };
	sprintf(cmd, "INSERT INTO node(IP_addr,Mac_addr,RX_rate,TX_rate,BER,PLR) VALUES('%s','%s','%s','%s',%f,%f)", IP_addr,Mac_addr, RX_rate, TX_rate, ber, plr);
	int res = mysql_query(&conn, cmd);
	if (!res) {
		printf("Insert into Table node: %lu rows\n", (unsigned long)mysql_affected_rows(&conn));
	}
	else {
		fprintf(stderr, "Insert into Table node: error %d: %s\n", mysql_errno(&conn), mysql_error(&conn));
	}
}


void update_nodes(const char *IP_addr, const char *Mac_addr, const char* RX_rate, const char* TX_rate, double ber, double plr) {
	unsigned int timeout = 3;
	mysql_options(&conn, MYSQL_OPT_WRITE_TIMEOUT, (const char *)&timeout);

	char cmd[255] = { 0 };
	sprintf(cmd, "UPDATE node SET RX_rate='%s',TX_rate='%s',BER=%f,PLR=%f  WHERE IP_addr='%s' AND Mac_addr='%s'", RX_rate,TX_rate, ber, plr, IP_addr, Mac_addr);
	int res = mysql_query(&conn, cmd);
	if (!res) {
		printf("Update Table node: %lu rows\n", (unsigned long)mysql_affected_rows(&conn));
	}
	else {
		fprintf(stderr, "Update Table node: error %d: %s\n", mysql_errno(&conn), mysql_error(&conn));
	}
}


//void delete_columns(const char* IP_addr) {
//	char cmd[255] = { 0 };
//	sprintf(cmd, "delete from node where ID in (select ID from (select  min(ID) as ID, count(IP_addr='%s') \
//					as ucount from node group by IP_addr having ucount >2 order by ucount desc) as tab);",IP_addr);
//	int res = mysql_query(&conn, cmd);
//	if (!res) {
//		printf("Delete %lu rows\n", (unsigned long)mysql_affected_rows(&conn));
//	}
//	else {
//		fprintf(stderr, "Delete error %d: %s\n", mysql_errno(&conn), mysql_error(&conn));
//	}
//}

MYSQL_RES *res_ptr;
MYSQL_ROW sqlrow;

int get_count(const char* IP_addr) {
	char cmd[255] = { 0 };
	sprintf(cmd, "select count(*) from node where IP_addr = '%s' group by IP_addr", IP_addr);
	int res = mysql_query(&conn, cmd);
	int count;

	if (res) {
		fprintf(stderr, "SELECT error: %s\n", mysql_error(&conn));
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
				fprintf(stderr, "Retrive error: %s\n", mysql_error(&conn));
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

		char addr[][50] = { "error","error" };	//ip地址
		double mes[2] = {};

		char rate[][20] = { "error","error" };

		get_addr(argv[1],addr);		//获取主机IP地址
		const char *mac_addr = addr[0];
		const char *ip_addr = addr[1];

		get_rate(argv[1], rate);	//获取传输速度
		const char *RX_rate = rate[0];
		const char *TX_rate = rate[1];

		ifconfig_messages(argv[1],mes);	//获取丢包率，误码率
		double BER = mes[0], PLR = mes[1];
		const char *test = { "error" };
		const char *sumeipai_ipaddr4 = { "192.168.4.2" };
		int rc;
		FILE* fp = fopen("output.txt", "w");
		if ((strcmp(ip_addr, test)) && (strcmp(RX_rate, test))) {	//剔除值为“error”
			if (rc = fprintf(fp,"%s %s %s %s %f %f",ip_addr,mac_addr,RX_rate,TX_rate,BER,PLR) < 0)
				fprintf(stderr, "written to output.txt error!!!");
		}
		fclose(fp);

		const int N = 6;
		char passwd[] = "shujuku1";
		char* sumeipai_ipaddr[] = { "192.168.1.2","192.168.2.2","192.168.3.2" ,"192.168.4.2","192.168.5.2", "192.168.6.2" };

		int* status = (int *)malloc(sizeof(int));
		//写入表node
		int i;
		for (i = 0; i < N; ++i) {
			connection(sumeipai_ipaddr[i], "root", passwd, "linjiejuzhen", status);
			if (*status) {
				if ((!strcmp(ip_addr, test)) || (!strcmp(RX_rate, test))) //剔除值“error”
					continue;
				if(get_count(ip_addr))
					update_nodes(ip_addr, mac_addr, RX_rate, TX_rate, BER, PLR);
				else 
					insert_nodes(ip_addr, mac_addr, RX_rate, TX_rate, BER, PLR);
				/*delete_columns(ip_addr);*/
				//if(!strcmp(sumeipai_ipaddr[i],sumeipai_ipaddr4))	//将数据插入树霉派192.168.4.2的表nodeAll中
				//	insertAll_nodes(ip_addr, mac_addr, RX_rate, TX_rate, BER, PLR);
				//update_nodes(ip_addr, mac_addr, RX_rate, TX_rate, BER, PLR);
			}
			mysql_close(&conn);
		}

		free(status);
		if ((end = times(&tmsend)) == -1)
			unix_error("times error");
		pr_times(end - start, &tmsstart, &tmsend);
		
		return 0;
}