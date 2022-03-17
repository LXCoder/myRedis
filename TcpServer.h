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

	// ���������ü���
	int Listen(unsigned short port);

	// �ȴ������ܿͻ�����������
	int Accept(struct sockaddr *sa, socklen_t *salenptr);

	// ��ȡ��Ϣ
	int Read(int fd, void *buf);

	// ������Ϣ
	void Write(int fd, const void *buf, int len);

	//���� epoll ��
	void createEpoll();

	// �����¼������� fd ������������ -1 ,��ʾ�������󣬷��� -2 ��ʾ continue
	int epollWait();

	// �����±�Ϊ idx �¼��� fd ,������ -1����ʾ�ͻ������ӷ�����
	int procEvent(int idx);

	// �ر�����
	void Closed();

private:
	// ���ڼ������ļ�������
	int m_lfd;

	// epoll �����ļ�������
	int m_epfd;

	char m_buf[MAX_BUF_SIZE];

	struct epoll_event m_ev;

	// epoll �¼�����
	struct epoll_event m_events[MAX_EPOLL_SIZE];
};

