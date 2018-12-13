
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

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

int main(int argc, char *argv[])
{
	char cmd[255] = { 0 };
	sprintf(cmd, "iftop -i %s -t -s 1 > iftop_out.txt", argv[1]);
	system(cmd);

	FILE* pFd;
	pFd = fopen("iftop_out.txt", "rb");
	char cBuf[1024 * 10] = { 0 };
	fread(cBuf, 1, 1024 * 10, pFd);
	fclose(pFd);

	// debug:
	// printf("out.txt\n%s\n\n", cBuf);

	char sendRate[10] = { "error" };
	char receiveRate[10] = { "error" };

	// example:
	// 

	char *lpSrc_send = strstr(cBuf, "Total send rate:");
	char *lpSrc_receive = strstr(cBuf, "Total receive rate:");
	if (lpSrc_send != 0 && lpSrc_receive != 0)
	{
		if ((lpSrc_send = praseFields(lpSrc_send, "Total send rate", sendRate)) != 0
			&& (lpSrc_receive = praseFields(lpSrc_receive, "Total receive rate", receiveRate)) != 0)
		{
			printf("Get the Total rate fields:\n");
			printf("Total send rate(last 2 secs): [%s]\n", sendRate);
			printf("Total receive rate(last 2 sec2): [%s]\n", receiveRate);
		}
		else
		{
			if (lpSrc_send == 0)
				printf("Invalid Total send rate field.\n");
			if (lpSrc_receive = 0)
				printf("Invalid Total receive rate field.\n");

		}
	}
	else
	{
		if (lpSrc_send == 0)
			printf("Can not find Total send rate field.\n");
		if (lpSrc_receive = 0)
			printf("Can not find Total receive rate field.\n");
	}
}

/*
获取传输速率
*/

//long int getCurrentDownloadRates(long int * save_rate)
//{
//	FILE * net_dev_file;    //文件指针  
//	char buffer[1024];  //文件中的内容暂存在字符缓冲区里  
//	size_t bytes_read;  //实际读取的内容大小   
//	char * match;    //用以保存所匹配字符串及之后的内容  
//	char inf[20] = "br-lan:";
//	if ((net_dev_file = fopen("/proc/net/dev", "r")) == NULL) //打开文件/pro/net/dev/，我们要读取的数据就是它啦  
//	{
//		printf("open file /proc/net/dev/ error!\n");
//		exit(EXIT_FAILURE);
//	}
//	bytes_read = fread(buffer, 1, sizeof(buffer), net_dev_file);//将文件中的1024个字符大小的数据保存到buffer里  
//	fclose(net_dev_file); //关闭文件  
//	if (bytes_read == 0)//如果文件内容大小为０，没有数据则退出  
//	{
//		exit(EXIT_FAILURE);
//	}
//	buffer[bytes_read] = '\0';
//	match = strstr(buffer,inf);//匹配eth0第一次出现的位置，返回值为第一次出现的位置的地址  
//	if (match == NULL)
//	{
//		printf("no %s keyword to find!\n",inf);
//		exit(EXIT_FAILURE);
//	}
//	sscanf(match, "%s%ld", inf,save_rate);//从字符缓冲里读取数据，这个值就是当前的流量啦。呵呵。  
//	return *save_rate;
//}
//  
//
//double get_speed()
//{
//	long int start_download_rates;  //保存开始时的流量计数  
//	long int end_download_rates;    //保存结果时的流量计数
//	double dspeed = 0.0;
//	
//		getCurrentDownloadRates(&start_download_rates);//获取当前流量，并保存在start_download_rates里  
//		sleep(WAIT_SECOND); //休眠多少秒，这个值根据宏定义中的WAIT_SECOND的值来确定  
//							//sleep的头文件在unistd.h  
//		getCurrentDownloadRates(&end_download_rates);//获取当前流量，并保存在end_download_rates里
//		dspeed = (double)((end_download_rates - start_download_rates) / WAIT_SECOND);
//		printf("download is : %.2lf Bytes/s\n", dspeed);//打印结果  
//	
//	return dspeed;
//	exit(EXIT_SUCCESS);
//}

/*
获取主机号
*/

//char * get_hostid(char* IP)
//{
//	//char IP[] = { "192.168.9.12" };
//	char *host_id = 0;
//	host_id = strrchr(IP, '.');
//	return (host_id + 1);
//}

////获取带宽
//const int M = 6;
//char* routes_ipaddr[] = { "192.168.9.1","192.168.9.2","192.168.9.3","192.168.9.4","192.168.9.5","192.168.9.6" };
//char bandwidth[][20] = { "error","error" ,"error" ,"error" ,"error" ,"error" };

//for (int i = 0; i < M; ++i) {
//	if (client_ipaddr == routes_ipaddr[i])
//		continue;
//	get_bandwidth(routes_ipaddr[i], bandwidth[i]);
//}

//

////写入表Links
//for (int i = 0; i < N; ++i) {		
//	connection(sumeipai_ipaddr[i], "root", passwd, "NSInfo");
//	insert_links(client_ipaddr, routes_ipaddr[0], bandwidth[0]);	
//	//update_links(client_ipaddr,routes_ipaddr[i],bandwidth[i]);
//	mysql_close(&conn);
//}	

void insert_links(char *IP_src, char* IP_des, char* BandWidth) {
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

void update_links(char *IP_src, char* IP_des, char* BandWidth) {
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
