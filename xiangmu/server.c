#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <pthread.h>


#define SERVER_PORT 9999
#define PASSWORD "1234"
#define USERNAME "Adam"
#define WRITEBUFLEN 1024

typedef struct SockInfo
{
	int fd;
	struct sockaddr_in addr;
	pthread_t id;
}SockInfo;

void *worker(void *arg){
	int hasPrintUserName = 0;
	char ip[64];
	char buf[1024];
	SockInfo *info = (SockInfo *)arg;
	while(1){
		int len = read(info->fd, buf, sizeof(buf));
		if (len == -1)
		{
			perror("read error");
			pthread_exit(NULL); 
		}else if(len == 0){
			printf("client close\n"); 
			close(info->fd);
			break;
		}else{
			if (!hasPrintUserName)
			{
				char tmpBuf[sizeof(USERNAME) + 1];
				int i = 0, j = 0;
				while(!isspace(buf[i]) && j < sizeof(USERNAME)){
					tmpBuf[j++] = buf[i++];
				}
				tmpBuf[j] = '\0';
				int ret = strncmp(tmpBuf, USERNAME, sizeof(USERNAME));
				if (ret == 0)
				{
					hasPrintUserName = 1;	
					printf("USERNAME buf: %s---hasPrintUserName:%d\n", buf,hasPrintUserName);
					write(info->fd, "please print your password...", WRITEBUFLEN);
				}else{
					printf("error: USERNAME buf: %s---hasPrintUserName:%d\n", buf,hasPrintUserName);
					write(info->fd, "uername not exit...", WRITEBUFLEN);
				}
			}else{
				char tmpBuf[sizeof(USERNAME) + 1];
				int i = 0, j = 0;
				while(!isspace(buf[i]) && j < sizeof(USERNAME)){
					tmpBuf[j] = buf[i];
					i++;
					j++;
				}
				tmpBuf[j] = '\0';
				int ret = strcmp(tmpBuf, PASSWORD);
				printf("ret:%d\n", ret);
				hasPrintUserName = 0;
				if (ret == 0)
				{
					printf("PASSWORD buf: %s---hasPrintUserName:%d\n", buf,hasPrintUserName);
					write(info->fd, "login success...", WRITEBUFLEN);
					break;
				}else{
					printf("error:PASSWORD buf: %s---hasPrintUserName:%d\n", buf,hasPrintUserName);
					write(info->fd, "login failed. please print your username...", WRITEBUFLEN);
				}
			}
		}
	}
	return NULL;
}

int main(int argc, const char *argv[])
{
	struct sockaddr_in serv_addr;
	socklen_t serv_len = sizeof(serv_addr);

	int lfd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, 0, serv_len);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERVER_PORT);

	int flag = 1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

	bind(lfd, (struct sockaddr *)&serv_addr, serv_len);

	listen(lfd, 36);

	printf("Start....\n");

	int i = 0;
	SockInfo info[256];

	for (int i = 0; i < sizeof(info)/sizeof(info[0]); ++i)
	{	
		info[i].fd = -1;
	}

	socklen_t cli_len = sizeof(struct sockaddr_in);

	while(1){
		for (int i = 0; i < 256; ++i)
		{
			if (info[i].fd == -1)
			{
				break;
			}
		}
		if (i == 256)
		{
			break;
		}

		info[i].fd = accept(lfd, (struct sockaddr *)&info[i].addr, &cli_len);

		pthread_create(&info[i].id, NULL, worker, &info[i]);

		pthread_detach(info[i].id);
	}
	close(lfd);
	pthread_exit(NULL);
	return 0;
}