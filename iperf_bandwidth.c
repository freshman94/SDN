
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

void get_bandwidth(char* server_ip,char* bandwidth)
{
	//char server_ip[] = { "192.168.9.115" };
	//首先以理想状态下最大带宽10000M测出实际带宽
	//char bandwidth[10] = { "10000M" };
	char cmd[255] = { 0 };
	sprintf(cmd, "iperf -c %s -u -t 10 -i 1 -b %s | grep -A2 'Server Report' > iperf_out.txt ", server_ip, bandwidth);
	system(cmd);
	sleep(10);	// wait for iperf cmd done.

	FILE* pFd;
	pFd = fopen("iperf_out_ori.txt", "rb");
	char cBuf[1024 * 10] = { 0 };
	fread(cBuf, 1, 1024 * 10, pFd);
	fclose(pFd);

	char *lpSrc = strstr(cBuf, "MBytes");
	int N = 5;	//测试带宽的次数

				//然后测出实际带宽，并存入bandwidth中，再根据实际带宽，测出其稳定的带宽值
	for (int i = 0; i < N; ++i) {
		if (lpSrc != 0 && (lpSrc = praseFields(lpSrc, "MBytes", bandwidth)) != 0) {
			bandwidth[strlen(bandwidth)] = 'M';
			sprintf(cmd, "iperf -c %s -u -t 10 -i 1 -b %s | grep -A2 'Server Report' > iperf_out.txt ", server_ip, bandwidth);
			system(cmd);
			sleep(10);	// wait for iperf cmd done
			pFd = fopen("iperf_out.txt", "rb");
			fread(cBuf, 1, 1024 * 10, pFd);
			fclose(pFd);

			lpSrc = strstr(cBuf, "MBytes");
		}
		else
		{
			printf("Can not find!!!\n");
		}
	}

	if ((lpSrc = praseFields(lpSrc, "MBytes", bandwidth)) != 0)
	{
		printf("Get the BandWidth:\n");
		printf("BandWidth: [%s Mbits/sec]\n", bandwidth);
	}
	else
	{
		printf("Invalid field.\n");

		printf("Check the fields:\n");
		printf("BandWidth: [%s Mbits/sec]\n", bandwidth);
	}
}

