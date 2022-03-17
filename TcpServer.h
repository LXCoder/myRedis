#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>
#include <errno.h>


#define MAX_EPOLL_SIZE 1024
#define MAX_BUF_SIZE 2048
#define MAX_PENDING_CONN_SIZE 128

class TcpServer
{
public:
	TcpServer();

	~TcpServer();

	// 服务器设置监听
	int Listen(unsigned short port);

	// 等待并接受客户端连接请求
	int Accept(struct sockaddr *sa, socklen_t *salenptr);

	// 读取消息
	int Read(int fd, void *buf);

	// 发送消息
	void Write(int fd, const void *buf, int len);

	//创建 epoll 树
	void createEpoll();

	// 返回事件发生的 fd 个数，若返回 -1 ,表示发生错误，返回 -2 表示 continue
	int epollWait();

	// 处理下标为 idx 事件的 fd ,若返回 -1，表示客户端连接服务器
	int procEvent(int idx);

	// 关闭连接
	void Closed();

private:
	// 用于监听的文件描述符
	int m_lfd;

	// epoll 树的文件描述符
	int m_epfd;

	char m_buf[MAX_BUF_SIZE];

	struct epoll_event m_ev;

	// epoll 事件数组
	struct epoll_event m_events[MAX_EPOLL_SIZE];
};

