#include<string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#define SYSTEM_ERROR        10

#define MIN_TIME_LIMIT 1
#define MAX_TIME_LIMIT 100

struct rlimit tLimit;

typedef void handler_t(int);

handler_t *Signal(int signum, handler_t *handler)  //�źŴ�����sigaction�İ�װ����
{
	struct sigaction action, old_action;

	action.sa_handler = handler;
	sigemptyset(&action.sa_mask); /* Block sigs of type being handled */
	action.sa_flags = SA_RESTART; /* Restart syscalls if possible */

	if (sigaction(signum, &action, &old_action) < 0)
		unix_error("Signal error");
	return (old_action.sa_handler);
}

void sigxcpu_handler(int sig) {		//�����ź�SIGXCPU
	printf("Caught SIGXCPU!\n");
	exit(100);
}

void unix_error(char *msg)  //��������
{
	fprintf(stderr, "%s: %s\n", msg, strerror(errno));
	exit(10);
}

int main(int argc, char** argv)
{
	int timeLimit;
	pid_t pid;
	int s;
	struct rusage rUsage;
	double passTime;
	char** childCmd;

	if (argc < 3)
	{
		printf("argument number error!\n");
		printf("usage: timeLimt(%d~%ds) commandLine\n",
			MIN_TIME_LIMIT, MAX_TIME_LIMIT);
		return SYSTEM_ERROR;
	}
	//��ȡʱ�����ƺ��ڴ�����
	if (isdigit(argv[1][0]))
		timeLimit = atoi(argv[1]);
	if (timeLimit < MIN_TIME_LIMIT || timeLimit > MAX_TIME_LIMIT)
	{
		printf("time limit argument error!(%d~%ds)\n", MIN_TIME_LIMIT, MAX_TIME_LIMIT);
		return SYSTEM_ERROR;
	}

	if ((pid = fork()) < 0)
	{
		printf("fork error!\n");
		return SYSTEM_ERROR;
	}
	else if (pid == 0)        /*child process*/
	{
		getrlimit(RLIMIT_CPU, &tLimit);

		tLimit.rlim_cur = timeLimit;

		if (setrlimit(RLIMIT_CPU, &tLimit) == -1)     /*����ʱ������*/
			printf("setrlimit time limit error!\n");
		
		if (Signal(SIGXCPU, sigxcpu_handler) < 0)	//�����ź�SIGXCPU��Ĭ����Ϊ
			unix_error("signal error");

		childCmd = argv + 2;
		execvp(childCmd[0], childCmd);

	}
	else
	{
		waitpid(pid, &s, 0);        //�ȴ��ӽ��̽���������ȡ�ź�
		printf("================parent process=====================\n");
		printf("child exit status: %d\n", s);
		if (WIFEXITED(s))
			printf("exit normally.\n");
		else
			printf("exit abnormally.\n");
		if (WIFSIGNALED(s))
			printf("exit by signal: %d | %d\n", s, WTERMSIG(s));
		printf("parent process is finished.\n");
		getrusage(RUSAGE_CHILDREN, &rUsage);
		passTime = (rUsage.ru_utime.tv_sec + rUsage.ru_stime.tv_sec) * 1000 + (float)(rUsage.ru_stime.tv_usec + rUsage.ru_utime.tv_usec) / 1000;
		printf("child process time: %d ms\n", (int)passTime);
		printf("===================================================\n\n");
		return 0;
	}
}




