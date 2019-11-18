#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define MAX_EVENTS 10
#define INVALID_FD -1

/* 
 * every 5 seconds, send and recv from server 
 * usage: socket_aio <server_ip> <server_port> 
 *  
*/
int main(int argc, char *argv[])
{
	int sock_fd = INVALID_FD;
	struct sockaddr_in server;
	int ret = 0;
	int i = 0;

	char recvbuf[1024];

	// epoll 
	int efd = INVALID_FD;
	struct epoll_event event;
	struct epoll_event events[MAX_EVENTS];

	// for dfx
	struct timeval tv;
	struct tm* tm_info;
	char timedisplay[50];

	if (argc < 3)
	{
		printf("Usage: socket_aio <server_ip> <server_port>");
		return -1;
	}
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == INVALID_FD)
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
////////////////////////////// 
/// level triggered  
	efd = epoll_create(MAX_EVENTS);
	if (efd == INVALID_FD)
	{
		printf("epoll_create failed");
		goto clean;
	}

	event.events = EPOLLOUT | EPOLLIN;
	event.data.fd = sock_fd;
	epoll_ctl(efd, EPOLL_CTL_ADD, sock_fd, &event);

	while (1) {
		int event_count = 0;
		int i = 0;
		struct epoll_event ev;

		gettimeofday(&tv, NULL);
		tm_info = localtime(&tv.tv_sec);
		strftime(timedisplay, 50, "%Y:%m:%d %H:%M:%S", tm_info);
		printf("%s\n", timedisplay);

		event_count = epoll_wait(efd, events,  MAX_EVENTS, -1);
		printf("event count : %d\n", event_count);

		for (i = 0; i < event_count; i++)
		{
			if (events[i].events & EPOLLOUT)
			{
				int len = strlen(timedisplay);
				ret = send(events[i].data.fd, timedisplay, len, 0);
				if (ret == -1)
				{
					printf("send failed: %d\n", ret);
					goto clean;
				}
				printf("send message: is %s\n", timedisplay);

				#if 1
				// add relevalt socket read event
				ev.data.fd = events[i].data.fd;
				ev.events = EPOLLIN;
				epoll_ctl(efd, EPOLL_CTL_MOD, events[i].data.fd, &ev);
				#endif 
			}
			else if (events[i].events & EPOLLIN)
			{
				ret = recv(sock_fd, recvbuf, sizeof(recvbuf), 0);
				if (ret == -1)
				{
					printf("recv failed: %d\n", ret);
					goto clean;
				}
				printf("recv message: is %s\n", recvbuf);

				#if 1
				// add relevalt socket write event
				ev.data.fd = events[i].data.fd;
				ev.events = EPOLLOUT;
				epoll_ctl(efd, EPOLL_CTL_MOD, events[i].data.fd, &ev);
				#endif
			}
			else
			{
				printf("unknown events: %d\n", events[i].events);
			}
		}

		sleep(5);
	}


	close(sock_fd);
	close(efd);
	return 0;

clean:
	if (sock_fd != INVALID_FD)
	{
		close(sock_fd);
	}
	if (efd != INVALID_FD)
	{
		close(efd);
	}
	return -1;
}


