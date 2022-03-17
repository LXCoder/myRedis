#include "TcpServer.h"

TcpServer::TcpServer()
{
}

TcpServer::~TcpServer()
{
}

int TcpServer::Listen(unsigned short port)
{
	int ret = 0;
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	m_lfd = socket(PF_INET, SOCK_STREAM, 0);//���� socket

	if (m_lfd < 0)
	{
		perror("socket error");
		exit(-1);
	}

	//�����ļ�������Ϊ�˿ڸ���
	int opt = 1;
	setsockopt(m_lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

	// ��
	ret = bind(m_lfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if (ret < 0)
	{
		perror("bind error");
		exit(-1);
	}

	// ����
	ret = listen(m_lfd, MAX_PENDING_CONN_SIZE);
	if (ret < 0)
	{
		perror("bind error");
		exit(-1);
	}

	return ret;
}

int TcpServer::Accept(sockaddr * sa, socklen_t * salenptr)
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

int TcpServer::Read(int fd, void *buf)
{
	//�пͻ��˷������ݹ���
	memset(buf, 0x00, sizeof(buf));
	int n = recv(fd, buf, sizeof(buf), 0);
	if (n <= 0)
	{
		close(fd);
		//��sockfd��Ӧ���¼��ͽڵ��epoll����ɾ��
		epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, NULL);
	}

	return n;
}

void TcpServer::Write(int fd, const void *buf, int len)
{
	send(fd, buf, len, 0);
}

void TcpServer::createEpoll()
{
	// ����һ��epoll��
	m_epfd = epoll_create(MAX_EPOLL_SIZE);
	if (m_epfd < 0)
	{
		perror("create epoll error");
		exit(-1);
	}

	// �� m_lfd �� epoll ��
	m_ev.data.fd = m_lfd;
	m_ev.events = EPOLLIN;
	epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_lfd, &m_ev);
}

int TcpServer::epollWait()
{
	int ret = epoll_wait(m_epfd, m_events, 1024, -1);
	if (ret < 0)
	{
		perror("epoll_wait error");
		if (errno == EINTR)
			return -2;

		return -1;//��������
	}

	return ret;
}

int TcpServer::procEvent(int idx)
{
	int socketfd = m_events[idx].data.fd;

	// �пͻ�����������
	if (socketfd == m_lfd)
	{
		int cfd = Accept(NULL, NULL);
		//���µ� cfd �� epoll ��
		m_ev.data.fd = cfd;
		m_ev.events = EPOLLIN;
		epoll_ctl(m_epfd, EPOLL_CTL_ADD, cfd, &m_ev);
		return -1;;
	}
	return socketfd;
}

void TcpServer::Closed()
{
	close(m_epfd);
	close(m_lfd);
}
