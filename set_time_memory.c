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
#define MIN_MEM_LIMIT 4
#define MAX_MEM_LIMIT 1024

struct rlimit tLimit, mLimit;

int main(int argc, char** argv)
{
	int timeLimit, memLimit, ret;
	pid_t pid;
	int s;
	struct rusage rUsage;
	double passTime;
	char** childCmd;

	if (argc < 4)
	{
		printf("argument number error!\n");
		printf("usage: timeLimt(%d~%ds) memoryLimit(%d~%dMb) commandLine\n",
			MIN_TIME_LIMIT, MAX_TIME_LIMIT, MIN_MEM_LIMIT, MAX_MEM_LIMIT);
		return SYSTEM_ERROR;
	}
	//获取时间限制和内存限制
	if (isdigit(argv[1][0]))
		timeLimit = atoi(argv[1]);
	if (isdigit(argv[2][0]))
		memLimit = atoi(argv[2]);
	if (timeLimit < MIN_TIME_LIMIT || timeLimit > MAX_TIME_LIMIT)
	{
		printf("time limit argument error!(%d~%ds)\n", MIN_TIME_LIMIT, MAX_TIME_LIMIT);
		return SYSTEM_ERROR;
	}
	if (memLimit < MIN_MEM_LIMIT || memLimit > MAX_MEM_LIMIT)
	{
		printf("memory limit argument error!(%d~%dMb)\n", MIN_MEM_LIMIT, MAX_MEM_LIMIT);
		return SYSTEM_ERROR;
	}
	memLimit = memLimit * 1024 * 1024;

	if ((pid = fork()) < 0)
	{
		printf("fork error!\n");
		return SYSTEM_ERROR;
	}
	else if (pid == 0)        /*child process*/
	{
		getrlimit(RLIMIT_CPU, &tLimit);
		getrlimit(RLIMIT_AS, &mLimit);

		tLimit.rlim_cur = timeLimit;
		mLimit.rlim_cur = memLimit;

		if (setrlimit(RLIMIT_CPU, &tLimit) == -1)     /*设置时间限制*/
			printf("setrlimit time limit error!\n");
		if (setrlimit(RLIMIT_AS, &mLimit) == -1)      /*设置内存限制*/
			printf("setrlimit memory limit error!\n");

		childCmd = argv + 3;
		execvp(childCmd[0], childCmd);
	}
	else
	{
		waitpid(pid, &s, 0);        //等待子进程结束，并获取信号
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




