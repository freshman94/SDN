
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
��ȡ��������
*/

//long int getCurrentDownloadRates(long int * save_rate)
//{
//	FILE * net_dev_file;    //�ļ�ָ��  
//	char buffer[1024];  //�ļ��е������ݴ����ַ���������  
//	size_t bytes_read;  //ʵ�ʶ�ȡ�����ݴ�С   
//	char * match;    //���Ա�����ƥ���ַ�����֮�������  
//	char inf[20] = "br-lan:";
//	if ((net_dev_file = fopen("/proc/net/dev", "r")) == NULL) //���ļ�/pro/net/dev/������Ҫ��ȡ�����ݾ�������  
//	{
//		printf("open file /proc/net/dev/ error!\n");
//		exit(EXIT_FAILURE);
//	}
//	bytes_read = fread(buffer, 1, sizeof(buffer), net_dev_file);//���ļ��е�1024���ַ���С�����ݱ��浽buffer��  
//	fclose(net_dev_file); //�ر��ļ�  
//	if (bytes_read == 0)//����ļ����ݴ�СΪ����û���������˳�  
//	{
//		exit(EXIT_FAILURE);
//	}
//	buffer[bytes_read] = '\0';
//	match = strstr(buffer,inf);//ƥ��eth0��һ�γ��ֵ�λ�ã�����ֵΪ��һ�γ��ֵ�λ�õĵ�ַ  
//	if (match == NULL)
//	{
//		printf("no %s keyword to find!\n",inf);
//		exit(EXIT_FAILURE);
//	}
//	sscanf(match, "%s%ld", inf,save_rate);//���ַ��������ȡ���ݣ����ֵ���ǵ�ǰ�����������Ǻǡ�  
//	return *save_rate;
//}
//  
//
//double get_speed()
//{
//	long int start_download_rates;  //���濪ʼʱ����������  
//	long int end_download_rates;    //������ʱ����������
//	double dspeed = 0.0;
//	
//		getCurrentDownloadRates(&start_download_rates);//��ȡ��ǰ��������������start_download_rates��  
//		sleep(WAIT_SECOND); //���߶����룬���ֵ���ݺ궨���е�WAIT_SECOND��ֵ��ȷ��  
//							//sleep��ͷ�ļ���unistd.h  
//		getCurrentDownloadRates(&end_download_rates);//��ȡ��ǰ��������������end_download_rates��
//		dspeed = (double)((end_download_rates - start_download_rates) / WAIT_SECOND);
//		printf("download is : %.2lf Bytes/s\n", dspeed);//��ӡ���  
//	
//	return dspeed;
//	exit(EXIT_SUCCESS);
//}

/*
��ȡ������
*/

//char * get_hostid(char* IP)
//{
//	//char IP[] = { "192.168.9.12" };
//	char *host_id = 0;
//	host_id = strrchr(IP, '.');
//	return (host_id + 1);
//}

////��ȡ����
//const int M = 6;
//char* routes_ipaddr[] = { "192.168.9.1","192.168.9.2","192.168.9.3","192.168.9.4","192.168.9.5","192.168.9.6" };
//char bandwidth[][20] = { "error","error" ,"error" ,"error" ,"error" ,"error" };

//for (int i = 0; i < M; ++i) {
//	if (client_ipaddr == routes_ipaddr[i])
//		continue;
//	get_bandwidth(routes_ipaddr[i], bandwidth[i]);
//}

//

////д���Links
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
