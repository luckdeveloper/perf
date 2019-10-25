#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <libaio.h>

#define IO_NR 5

/*
 * usage: socket_aio <server_ip> <server_port>
*/
int main(int argc, char *argv[])
{
	int sock_fd;
	struct sockaddr_in server;
	int ret = 0;
	int i = 0;

	io_context_t ctx;
	struct iocb iocbs[IO_NR];
	struct iocb *ios[IO_NR];
	struct io_event events[IO_NR];
	struct timespec timeout;
	const char *msg = "hello";
	char recvbuf[1024];

	if (argc < 3)
	{
		printf("Usage: socket_aio <server_ip> <server_port>");
		return -1;
	}
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1)
	{
		printf("socket failed\n");
		return -1;
	}

	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_family		= AF_INET;
	server.sin_port			= htons(atol(argv[2]));

	ret = connect(sock_fd, (struct sockaddr *)&server, sizeof(server));
	if (ret != 0)
	{
		printf("connect failed: %d\n", ret);
		goto clean;

	}

	/// use libaio to write socket 
	/// 
	/// 
	memset(&ctx, 0, sizeof(ctx));
	if (io_setup(IO_NR, &ctx) != 0)
	{
		printf("io_setup failed\n");
		goto clean;
	}

	for (i = 0; i < IO_NR; i++)
	{
		int len = strlen(msg);
		io_prep_pwrite(&iocbs[i], sock_fd, (void *)msg, strlen(msg), 0);
		iocbs[i].data = (void *)msg;
		ios[i] = &iocbs[i];
	}

	if (io_submit(ctx, IO_NR, ios) < IO_NR)
	{
		printf("io_submit failed\n");
		io_destroy(ctx);
		goto clean;
	}

	while (1) {
		timeout.tv_sec = 0;
		timeout.tv_nsec = 500000000;
		if (io_getevents(ctx, 0, IO_NR, events, &timeout) == IO_NR)
		{
			break;
		}
		printf("not done yet\n");
		sleep(1);
	}

	//// use libaio to read socket 
	/// 
	/// 
	io_prep_pread(&iocbs[0], sock_fd, (void *)recvbuf, sizeof(recvbuf), 0);
	iocbs[0].data = (void *)recvbuf;
	ios[0] = &iocbs[0];


	if (io_submit(ctx, 1, ios) < 1)
	{
		printf("io_submit failed\n");
		io_destroy(ctx);
		goto clean;
	}

	while (1) {
		timeout.tv_sec = 0;
		timeout.tv_nsec = 500000000;
		if (io_getevents(ctx, 0, 1, events, &timeout) == 1)
		{
			break;
		}
		printf("not done yet\n");
		sleep(1);
	}
	printf("recv message: is %s\n", recvbuf);

	close(sock_fd);
	io_destroy(ctx);
	return 0;

clean:
	close(sock_fd);
	io_destroy(ctx);
	return -1;
}
