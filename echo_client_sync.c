#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

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

	for (i = 0; i < IO_NR; i++)
	{
		int len = strlen(msg);
		ret = send(sock_fd, msg, len, 0);
		if (ret == -1)
		{
			printf("send failed: %d\n", ret);
			goto clean;
		}
	}

	ret = recv(sock_fd, recvbuf, sizeof(recvbuf), 0);
	if (ret == -1)
	{
		printf("recv failed: %d\n", ret);
		goto clean;
	}

	printf("recv message: is %s\n", recvbuf);

	close(sock_fd);
	return 0;

clean:
	close(sock_fd);
	return -1;
}

