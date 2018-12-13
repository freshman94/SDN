#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char * praseFields(char *pSrc, char *pField, char *pValue, char *bound)
{
	char *lpReturn = 0;

	if (strstr(pSrc, pField) == pSrc)
	{
		pSrc += strlen(pField);

		while (*(++pSrc) == ' ');

		char *lpSpace = strstr(pSrc, bound);
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
void get_rate(char* iface,char *rate[]) {
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
	char *bound = " ";
	if (lpSrc != 0)
	{
		if ((lpSrc = praseFields(lpSrc, "RX", rate[0], bound)) != 0
			&& (lpSrc = praseFields(lpSrc, "TX", rate[1], bound)) != 0)
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
