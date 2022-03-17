//客户端代码
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAX_BUF_SIZE 2048

int checkPork(const char* port)
{
    int i = 0;
    while(port[i] != '\0' && port[i] >= '0' && port[i] <= '9')
        ++i;

    if(port[i] != '\0')
        return -1;

    int ret = atoi(port);
    if(ret >= 5001 && ret <= 65535)
        return ret;

    return -1;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("please input server address.\n");
		return -1;
	}
    

	char* server_ip = argv[1];
	int server_port = checkPork(argv[2]);
    if(server_port == -1)
    {
        printf("port is invalid.\n");
        return -1;
    }


	//创建socket---用于和服务端进行通信
    int cfd = socket(AF_INET, SOCK_STREAM, 0);
	if (cfd < 0)
	{
		perror("socket error");
		return -1;
	}


	// 绑定服务器地址
	struct sockaddr_in serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(server_port);
	inet_pton(AF_INET, server_ip, &serv.sin_addr.s_addr);
	printf("[%x]\n", serv.sin_addr.s_addr);

	// 连接服务端
	int ret = connect(cfd, (struct sockaddr *)&serv, sizeof(serv));
	if (ret < 0)
	{
		perror("connect error");
		return -1;
	}

	int n = 0;
	char buf[MAX_BUF_SIZE];
	while (1)
	{
		// 打印提示符
		write(STDOUT_FILENO, "> ", strlen("> "));

		// 读取用户输入的命令
		memset(buf, 0x00, sizeof(buf));
		n = read(STDIN_FILENO, buf, sizeof(buf));
        buf[n-1] = '\0';//去掉后面的换行符
		
        if(strcmp(buf,"exit") == 0 || strcmp(buf, "EXIT") == 0 || strcmp(buf, "quit") == 0 || strcmp(buf, "QUIT") == 0)
			break;

		// 发送命令到服务器
		write(cfd, buf, n-1);

		// 读服务器发来的数据
		memset(buf, 0x00, sizeof(buf));
		n = read(cfd, buf, sizeof(buf));

		if (n <= 0)
		{
			printf("read error or server closed, n==[%d]\n", n);
			break;
		}

		printf("%s\n", buf);
	}

	//关闭套接字cfd
	close(cfd);
	
    return 0;
}
