
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

int myAtoi(string str) {
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

void main(void)
{
	system("ifconfig ens33 > ifconfig_out.txt");
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

	char *lpSrc = strstr(cBuf, "RX packets:");
	if (lpSrc != 0)
	{
		if ((lpSrc = praseFields(lpSrc, "RX packets:", cPackets)) != 0
			&& (lpSrc = praseFields(lpSrc, "errors:", cErrors)) != 0
			&& (lpSrc = praseFields(lpSrc, "dropped:", cDropped)) != 0)
		{
			printf("Get the RX packets fields:\n");
			//printf("RX packets: [%s]\n", cPackets);
			//printf("    errors: [%s]\n", cErrors);
			//printf("   dropped: [%s]\n", cDropped);
			long Packets = myAtoi(cPackets);
			long Errors = myAtoi(cErrors);
			long Dropped = myAtoi(cDropped);
			double BER = (double)Errors / (double)Packets;
			double PLR = (double)Dropped / (double)Packets;
			printf("Bit Error Rate:[%d]\n", BER);
			printf("Packet Loss Rate:[%d]\n", PLR);
			
		}
		else
		{
			printf("Invalid RX packets field.\n");

			printf("Check the RX packets fields:\n");
			printf("RX packets: [%s]\n", cPackets);
			printf("    errors: [%s]\n", cErrors);
			printf("   dropped: [%s]\n", cDropped);
			printf("  overruns: [%s]\n", cOverruns);
			printf("     frame: [%s]\n", cFrame);
		}
	}
	else
	{
		printf("Can not find RX packets field.\n");
	}
}

