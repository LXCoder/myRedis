#include "MyRedisServer.h"

MyRedisServer::MyRedisServer()
{
}

MyRedisServer::~MyRedisServer()
{
}

void MyRedisServer::doWork()
{
	// 建立数据库
	m_db = new MyRedisDb();

	// 建立套接字连接
	Listen(8888);
	// 创建 epoll 树
	createEpoll();

	cout << "myredis server is started." << endl;

	while (1)
	{
		m_readyCnt = epoll_wait(m_epfd, m_events, MAX_EPOLL_SIZE, -1);
		if (m_readyCnt < 0)
		{
			if (errno == EINTR)
				continue;
            else
            {
                perror("epoll_wait error"); 
                break;//发生错误，跳出循环
            }
			
		}

		// 处理发生的事件
		for (int i = 0;i < m_readyCnt;++i)
		{
			int socketfd = m_events[i].data.fd;

			// 事件 i 是客户端连接请求事件
			if (socketfd == m_lfd)
			{
				int cfd = Accept(NULL, NULL);
				//将新的 cfd 上 epoll 树
				m_ev.data.fd = cfd;
				m_ev.events = EPOLLIN;
				epoll_ctl(m_epfd, EPOLL_CTL_ADD, cfd, &m_ev);
				continue;
			}

			// 客户端发送数据过来
			memset(m_buf, 0x00, sizeof(m_buf));
			int n = recv(socketfd, m_buf, sizeof(m_buf), 0);
			if (n <= 0)
			{
				close(socketfd);
				//将 sockfd 对应的事件就节点从 epoll 树上删除
				epoll_ctl(m_epfd, EPOLL_CTL_DEL, socketfd, NULL);
			}
			else
			{
				// 收到命令，处理它
				string reply = m_db->doWork(m_buf);
				// 给客户端发送处理结果
				send(socketfd, reply.c_str(), reply.size(), 0);
			}
		}
	}

	Closed();
}

int MyRedisServer::Listen(unsigned short port)
{
	int ret = 0;
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	m_lfd = socket(PF_INET, SOCK_STREAM, 0);//创建 socket

	if (m_lfd < 0)
	{
		perror("socket error");
		exit(-1);
	}

	//设置文件描述符为端口复用
	int opt = 1;
	setsockopt(m_lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

	// 绑定
	ret = bind(m_lfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (ret < 0)
	{
		perror("bind error");
		exit(-1);
	}

	// 监听
	ret = listen(m_lfd, MAX_PENDING_CONN_SIZE);
	if (ret < 0)
	{
		perror("bind error");
		exit(-1);
	}

	return ret;
}

int MyRedisServer::Accept(sockaddr * sa, socklen_t * salenptr)
{
	int n = 0;

	while ((n = accept(m_lfd, sa, salenptr)) < 0)
	{
		if ((errno == ECONNABORTED) || (errno == EINTR))
			continue;
		else
		{
			perror("accept error");
			exit(-1);
		}
	}
	return n;
}

void MyRedisServer::createEpoll()
{
	// 创建一棵epoll树
	m_epfd = epoll_create(MAX_EPOLL_SIZE);
	if (m_epfd < 0)
	{
		perror("create epoll error");
		exit(-1);
	}

	// 将 m_lfd 上 epoll 树
	m_ev.data.fd = m_lfd;
	m_ev.events = EPOLLIN;
	epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_lfd, &m_ev);
}

void MyRedisServer::Closed()
{
	close(m_epfd);
	close(m_lfd);
}
