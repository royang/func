#define LOG_TAG "utils"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <signal.h>
#include <sys/sysinfo.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <resolv.h>
#include <net/route.h>
#include <linux/if_ether.h>
#include <linux/sockios.h>

#include "utils.h"
#include "firmupg.h"
#include "debug.h"


static void _reap(int sig)
{
	pid_t pid;

	while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
		usleep(1000);
	signal(SIGCHLD, _reap);
}

int _evalpid(char * const argv[], char *path, int timeout, int *ppid)
{
	pid_t pid, retpid;
	int stat_val = 0, status = 0;
	int sig;
	int i;
	int fd;

	signal(SIGCHLD, _reap);

	if ((pid = fork()) < 0) {
		/* error */
		LOGD("Error in forking");
		return errno;

	} else if (pid == 0) {
		/* Child */
		/* Clean up */
		setsid();
		closelog();

		/* Reset signal handlers set for parent process */
		for (sig = 0; sig < (_NSIG - 1); sig++)
			signal(sig, SIG_DFL );

		/* Clearup sockets and reset stdin/stdout/stderr */
		for(i = 3; i < 32; i ++) close(i);
		if ((fd = open("/dev/null", O_RDWR | O_NONBLOCK | O_NOCTTY)) >= 0) {
			dup2(fd, STDIN_FILENO);
			if (fd != STDIN_FILENO && fd != STDOUT_FILENO && fd != STDERR_FILENO)
				close(fd);
		}
		if ((fd = open("/dev/console", O_RDWR | O_NONBLOCK | O_NOCTTY)) >= 0) {
			dup2(fd, STDOUT_FILENO);
			dup2(fd, STDERR_FILENO);
			if (fd != STDIN_FILENO && fd != STDOUT_FILENO && fd != STDERR_FILENO)
				close(fd);
		}

		/* execute command */
		setenv("PATH", SHELL_ENV_PATH, 1);
		setenv("LD_LIBRARY_PATH", SHELL_ENV_LIB, 1);

		if(timeout > 0) alarm(timeout);
		execvp(argv[0], argv);

		exit(errno);
	}

	/* Parent */
	if (ppid) {
		*ppid = pid;
		return 0;
	} else {
		do {
			retpid = waitpid(pid, &stat_val, 0);
		} while ((retpid == -1) && (errno == EINTR));
		status = WEXITSTATUS(stat_val);
		return status;
	}
}



