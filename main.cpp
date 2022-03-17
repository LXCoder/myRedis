#include "MyRedisServer.h"

string MyRedisDb::m_aof_rewrite_buf = "";
string MyRedisDb::m_aof_buf = "";
int MyRedisDb::m_aof_state = -1;

int main()
{
	MyRedisServer server;
	server.doWork();

	return 0;
}
