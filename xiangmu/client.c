#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define SERVER_PORT 9999
#define SERVER_IP "127.0.0.1"

int main(int argc, const char *argv[])
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
	{
		perror("socket error");
		exit(1);
	}

	struct sockaddr_in serv_addr;

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr.s_addr);
	int ret = connect(fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (ret == -1)
	{
		perror("connect error");
		exit(1);
	}

	printf("please print your username...\n");
	while(1){
		char buf[512];	
		fgets(buf, sizeof(buf), stdin);
		write(fd, buf, strlen(buf) + 1);
		int len = read(fd, buf, sizeof(buf));
		if (len == -1)
		{
			perror("read error\n");
			exit(1);
		}else if(len == 0){
			printf("server close\n");
			break;
		}else{
			printf("server: %s\n", buf);
		}
	}

	close(fd);
	return 0;
}