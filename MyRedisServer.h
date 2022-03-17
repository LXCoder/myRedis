#include "MyRedisDb.h"
#include <string.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>
#include <errno.h>


#define MAX_EPOLL_SIZE 1024
#define MAX_BUF_SIZE 2048
#define MAX_PENDING_CONN_SIZE 128


class MyRedisServer
{
public:
	MyRedisServer();

	~MyRedisServer();

	// 工作函数
	void doWork();

	// 服务器设置监听
	int Listen(unsigned short port);

	// 等待并接受客户端连接请求
	int Accept(struct sockaddr *sa, socklen_t *salenptr);

	//创建 epoll 树
	void createEpoll();

	// 关闭连接
	void Closed();

private:
	// 数据库
	MyRedisDb* m_db;

	// 用于监听的文件描述符
	int m_lfd;

	// epoll 树的文件描述符
	int m_epfd;

	// 保存当前发生时间的个数
	int m_readyCnt;

	char m_buf[MAX_BUF_SIZE];

	struct epoll_event m_ev;

	// epoll 事件数组
	struct epoll_event m_events[MAX_EPOLL_SIZE];
};

