#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <libaio.h>

#define IO_NR 5

int main (int argc, char *argv[])
{
	io_context_t ctx;
	int ret = 0;
	struct iocb iocbs[IO_NR];
	struct iocb *ios[IO_NR];
	struct io_event events[IO_NR];
	struct timespec timeout;
	int fd; 							  
	const char *msg = "hello";
	int i;
	long long offset = 0;

	fd = open("aiotest", O_WRONLY|O_CREAT);
	if (fd < 0)
	{
		printf("open file faild\n");
		return -1;
	}

	memset(&ctx, 0, sizeof(ctx));
	if (io_setup(IO_NR, &ctx) != 0)
	{
		printf("io_setup failed\n");
		return -1;
	}
	for (i = 0; i < IO_NR; i++)
	{
		int len = strlen(msg);
		io_prep_pwrite(&iocbs[i], fd, (void*)msg, strlen(msg), offset);
		iocbs[i].data = (void*)msg;
		ios[i] = &iocbs[i];
		offset += len; 
	}
   
	if (io_submit(ctx, IO_NR,ios) < IO_NR)
	{
		printf("io_submit failed\n");
		io_destroy(ctx);
		return -1;
	}

	while (1) {
		timeout.tv_sec = 0;
		timeout.tv_nsec = 500000000;
		if (io_getevents(ctx, 0, IO_NR, events, &timeout) == IO_NR)
		{
			close(fd);
			break;
		}
		printf("not done yet\n");
		sleep(1);
	}
	io_destroy(ctx);

	return (0);
}

