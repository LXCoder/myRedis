#include "MyRedisDb.h"

MyRedisDb::MyRedisDb()
{
	initWork();//初始化工作
}

MyRedisDb::~MyRedisDb()
{
	if (m_comm_proc != nullptr)
	{
		delete m_comm_proc;
		m_comm_proc = nullptr;
	}
}

string MyRedisDb::doWork(string command)
{
	if (command == "bgrewriteaof" || command == "BGREWRITEAOF")
	{
		if (m_aof_state == -1)
		{
			m_aof_state = 0;
			pid_t pid = fork();
			if (pid < 0)		// fork 失败
			{
				return "bgrewriteaof faild!";
			}
			else if (pid == 0)	// 子进程
			{
				aofRewrite();//AOF 重写
				exit(-1);
			}
			else				// 父进程
			{
				return "OK";
			}
		}
		else if(m_aof_state == 0)// 重写正在进行,拒绝命令
		{
			return "The command was rejected because a rewrite is in progress in the background.";
		}
	}

	return runCommand(command);
}

void MyRedisDb::initWork()
{
	initCommandTable();	// 初始化命令表

	initSignal();

	loadAOFFile();		// 载入 AOF 文件

	initTimer();		// 初始化定时器
}

void MyRedisDb::initCommandTable()
{
	m_comm_proc = new CommandTable();//声明一个命令表
	
	// 把命令名字与相关的命令信息绑定起来
	for (int i = 0, n = command_name.size();i < n;++i)
	{
		CommandInfo info(command_name[i], command_err_reply[i], 1 << i, command_write[i]);
		m_command_info.insert(make_pair(command_name[i], info));
	}

	// 把命令名字与相关的处理函数绑定
	m_command_table[command_name[ZADD]] = &CommandTable::addCommand;
	m_command_table[command_name[ZCARD]] = &CommandTable::cardCommand;
	m_command_table[command_name[ZCOUNT]] = &CommandTable::countCommand;
	m_command_table[command_name[ZINCRBY]] = &CommandTable::incrbyCommand;
	m_command_table[command_name[ZRANGE]] = &CommandTable::rangeCommand;
	m_command_table[command_name[ZREVRANGE]] = &CommandTable::revRangeCommand;
	m_command_table[command_name[ZRANGEBYSCORE]] = &CommandTable::rangeByScoreCommand;
	m_command_table[command_name[ZREVRANGEBYSCORE]] = &CommandTable::revRangeByScoreCommand;
	m_command_table[command_name[ZRANK]] = &CommandTable::rankCommand;
	m_command_table[command_name[ZREVRANK]] = &CommandTable::revRankCommand;
	m_command_table[command_name[ZREM]] = &CommandTable::remCommand;
	m_command_table[command_name[ZREMRANGEBYRANK]] = &CommandTable::remRangeByRankCommand;
	m_command_table[command_name[ZREMRANGEBYSCORE]] = &CommandTable::remRangeByScoreCommand;
	m_command_table[command_name[ZSCORE]] = &CommandTable::scoreCommand;
	m_command_table[command_name[DEL]] = &CommandTable::delCommand;
	m_command_table[command_name[EXISTS]] = &CommandTable::existsCommand;
	m_command_table[command_name[KEYS]] = &CommandTable::keysCommand;
}

void MyRedisDb::initSignal()
{
	signal(SIGCHLD, &MyRedisDb::signalHandler);
	signal(SIGUSR1, &MyRedisDb::signalHandler);
	signal(SIGALRM, &MyRedisDb::signalHandler);
}

void MyRedisDb::initTimer()
{
	struct itimerval tm;
	// 周期性时间赋值
	tm.it_interval.tv_sec = 1;
	tm.it_interval.tv_usec = 0;

	// 第一次触发的时间
	tm.it_value.tv_sec = 2;
	tm.it_value.tv_usec = 0;

	setitimer(ITIMER_REAL, &tm, NULL);
}

void MyRedisDb::loadAOFFile()
{
	// 创建输入流
	ifstream ifs;
	// 打开文件
	ifs.open(AOF_FILE, ios::in);
	
	if (!ifs.is_open())
	{
		cout << "failed to open file!" << endl;
		return;
	}

	cout << "Loadding Data......" << endl;
	string line = "";
	// 每行读取命令
	while (getline(ifs, line))
	{
		if(!line.empty())
			runCommand(line);	// 执行命令语句
	}

	// 关闭流
	ifs.close();
	m_aof_buf.clear();
	cout << "Load Data Finished." << endl;
}

void MyRedisDb::aofRewrite()
{
	// 创建输出流
	ofstream ofs;
	// 打开文件
	ofs.open(AOF_REWRITE_FILE, ios::out);

	if (!ofs.is_open())
		cout << "file open failed" << endl;

	cout << "bgrewrite AOF start......" << endl;
	for (auto it = m_key_dict.begin();it != m_key_dict.end();++it)
	{
		ZSet* zset = it->second;
		vector<string> arr = zset->zrwRange(AOF_REWRITE_ITEMS_PER_CMD);
		for (int i = 0, n = arr.size(); i < n; i++)
		{
			string temp = "zadd ";
			temp.append(it->first);
			temp.push_back(' ');
			temp.append(arr[i]);
			temp.append("\n");
			ofs << temp << endl;//写入硬盘
		}
	}

	ofs.close();

	kill(getppid(), SIGUSR1);//发送信号给主线程，表示重写完成
}

void MyRedisDb::signalHandler(int signo)
{
	if(signo == SIGALRM)
	{
		// AOF 持久化
		if (!m_aof_buf.empty())
		{
			ofstream ofs;
			ofs.open(AOF_FILE, ios::out | ios::app);//打开 AOF 文件

			if (!ofs.is_open())
				ofs.open(AOF_FILE, ios::out | ios::app);

			ofs << m_aof_buf << endl;//写入硬盘
			m_aof_buf.clear();

			ofs.close();
		}
	}
	else if (signo == SIGCHLD)
	{
		pid_t wpid;
		//回收子进程
		while (1)
		{
			wpid = waitpid(-1, NULL, WNOHANG);
			if (wpid > 0)
			{
				cout << "The bgrewrite child process sends a signal to notify\
						 the main thread that the rewrite AOF is complete" << endl;
			}
			else if (wpid == 0 || wpid == -1)
			{
				break;
			}
		}
	}
	else if (signo == SIGUSR1)
	{
		// AOF 重写后把重写缓冲区的内容写入文件，在改名 aof.txt
		if (m_aof_state == 0)
		{
			if (!m_aof_rewrite_buf.empty())
			{
				// 创建输出流
				ofstream ofs;
				// 打开文件
				ofs.open(AOF_REWRITE_FILE, ios::out | ios::app);

				ofs << m_aof_rewrite_buf << endl;//写入硬盘
				m_aof_rewrite_buf.clear();

				ofs.close();//关闭文件
			}

			// 创建子进程改名
			pid_t pid = fork();
			if (pid < 0) //fork失败的情况
			{
				perror("fork error");
				return;
			}
			else if (pid > 0)//父进程
			{
				m_aof_state = -1;
				cout << "bgrewrite AOF succeed." << endl;
			}
			else if (pid == 0) //子进程
			{
				execlp("mv", "mv", AOF_REWRITE_FILE, AOF_FILE, NULL);
				perror("execl error");
			}
		}
	}
}

string MyRedisDb::runCommand(const string & command)
{
	CommandArg argObj;
	string reply = "";
	// 检查命令语句是否合法
	bool ret = checkCommand(command, argObj, reply);

	// 语法错误
	if (!ret)
		return reply;

	// 取得相应指令的处理函数指针
	funcptr proc = m_command_table[argObj.comm_name];
	// 执行函数
	(m_comm_proc->*proc)(m_key_dict, argObj, reply);
	// 把写指令写入 AOF 缓冲区
	if (m_command_info[argObj.comm_name].get_isWrite())
	{
		m_aof_buf.append(command);
		m_aof_buf.append("\n");
		// AOF 重写时，需要把写指令写入重写缓冲区
		if (m_aof_state == 1)
		{
			m_aof_rewrite_buf.append(command);
			m_aof_rewrite_buf.append("\n");
		}
	}

	// 返回执行完毕后的回复
	return reply;
}

bool MyRedisDb::checkCommand(const string & command, CommandArg& comm_arg, string& reply)
{
	// 把命令按空格分割
	vector<string> args;
	bool ret = splitString(command, ' ', args);

	if (!ret || args.size() < 2)
	{
		reply = "ERROR: Invalid Command!";
		return false;
	}

	// 检查参数
	ret = checkArgument(args, comm_arg, reply);

	return ret;
}

bool MyRedisDb::splitCommand(const string & command, vector<string>& args)
{
	int n = command.size();
	int left = 0, right = n - 1;

	// 去掉前后空格
	while (left < n && command[left] == ' ')
		++left;
	while (right >= 0 && command[right] == ' ')
		--right;

	if (left > right)
		return false;

	string temp = "";

	// 按空格分割命令
	while (left <= right)
	{
		if (command[left] == ' ')
		{
			if (!temp.empty())
			{
				args.push_back(temp);
				temp.clear();
			}
		}
		else
			temp.push_back(command[left]);

		++left;
	}
	if(!temp.empty())
		args.push_back(temp);

	return true;
}

bool MyRedisDb::checkArgument(const vector<string>& args, CommandArg & comm_arg, string & reply)
{
	int argc = args.size();
	// 把命令转化为小写
	string comm = args[0];
	bool ret = downcase(comm);

	// 检查该命令是否存在,若命令不全是字母，也是不存在
	if (!ret || m_command_info.count(comm) == 0)
	{
		reply = "ERROR: Invalid Command!";
		return false;
	}

	comm_arg.comm_name = comm;		// 命令名字

	// 处理命令参数
	int mask = m_command_info[comm].get_mask();//命令标识
	if (mask & EXISTS_ID || mask & KEYS_ID || mask & ZCARD_ID)
	{
		if (argc != 2)
		{
			reply = "ERROR: Invalid Command!";
			return false;
		}
	}
	else if (mask&DEL_ID)
	{
		for (int i = 1;i < argc;++i)
			comm_arg.key_names.push_back(args[i]);

		return true;
	}
	else if ((mask&ZADD_ID) || (mask&ZREM_ID))
	{
		if (argc < 3)
		{
			reply = "ERROR: Invalid Command!";
			return false;
		}

		if ((mask&ZADD_ID))
		{
			if (argc % 2 != 0)
			{
				reply = "ERROR: Invalid Argument!";
				return false;
			}

			for (int i = 2;i < argc;i += 2)
			{
				if (!checkDoubleType(args[i]))
				{
					reply = "ERROR: Invalid Argument!";
					return false;
				}
			}

			for (int i = 2;i < argc;i += 2)
				comm_arg.eles.push_back(make_pair(args[i + 1], stod(args[i])));
		}
		else
		{
			for (int i = 2;i < argc;++i)
				comm_arg.eles.push_back(make_pair(args[i], 0));
		}
	}
	else if ((mask&ZRANK_ID) || (mask&ZREVRANK_ID) || (mask&ZSCORE_ID))
	{
		if (argc != 3)
		{
			reply = "ERROR: Invalid Command!";
			return false;
		}

		comm_arg.eles.push_back(make_pair(args[2], 0));
	}
	else if ((mask&ZCOUNT_ID) || (mask&ZRANGEBYSCORE_ID) || (mask&ZREVRANGEBYSCORE_ID) || (mask&ZREMRANGEBYSCORE_ID))
	{
		if ((mask&ZCOUNT_ID) || (mask&ZREMRANGEBYSCORE_ID))
		{
			if (argc != 4)
			{
				reply = "ERROR: Invalid Command!";
				return false;
			}
		}
		else
		{
			if (argc < 4 || argc > 5)
			{
				reply = "ERROR: Invalid Command!";
				return false;
			}

			if (argc == 5)
			{
				string str = args[argc - 1];
				ret = downcase(str);
				if (!ret || str != "withscores")
				{
					reply = "ERROR: Invalid Command!";
					return false;
				}

				comm_arg.withscores = true;
			}
		}

		if (!checkDoubleType(args[2]) || !checkDoubleType(args[3]))
		{
			reply = "ERROR: Invalid Argument!";
			return false;
		}

		comm_arg.min = stod(args[2]);
		comm_arg.max = stod(args[3]);
	}
	else if ((mask&ZRANGE_ID) || (mask&ZREVRANGE_ID) || (mask&ZREMRANGEBYRANK_ID))
	{
		if (mask&ZREMRANGEBYRANK_ID)
		{
			if (argc != 4)
			{
				reply = "ERROR: Invalid Command!";
				return false;
			}
		}
		else
		{
			if (argc < 4 || argc > 5)
			{
				reply = "ERROR: Invalid Command!";
				return false;
			}

			if (argc == 5)
			{
				string str = args[argc - 1];
				ret = downcase(str);
				if (!ret || str != "withscores")
				{
					reply = "ERROR: Invalid Command!";
					return false;
				}

				comm_arg.withscores = true;
			}
		}

		if (!checkLongType(args[2]) || !checkLongType(args[3]))
		{
			reply = "ERROR: Invalid Argument!";
			return false;
		}

		comm_arg.start = stol(args[2]);
		comm_arg.stop = stol(args[3]);
	}
	else if ((mask&ZINCRBY_ID))
	{
		if (argc != 4)
		{
			reply = "ERROR: Invalid Command!";
			return false;
		}

		if (!checkDoubleType(args[2]))
		{
			reply = "ERROR: Invalid Argument!";
			return false;
		}

		comm_arg.eles.push_back(make_pair(args[3], stod(args[2])));
	}

	comm_arg.key_names.push_back(args[1]);	// 键名

	return true;
}

bool MyRedisDb::checkLongType(const string & val)
{
	int idx = 0, n = val.size();
	if (val[0] == '-')
		++idx;

	for (;idx < n;++idx)
	{
		if (val[idx] < '0' || val[idx] > '9')
			return false;
	}
	return true;
}

bool MyRedisDb::checkDoubleType(const string & val)
{
	int pt = 0;
	int idx = 0, n = val.size();

	if (val[0] == '-')
		++idx;

	for (;idx < n;++idx)
	{
		if (val[idx] == '.')
			++pt;
		else if (val[idx] < '0' || val[idx] > '9')
			return false;
	}
	return pt < 2;
}

bool MyRedisDb::downcase(string & comm)
{
	int commLen = comm.size();
	for (int i = 0;i < commLen;++i)
	{
		if (comm[i] >= 'A' && comm[i] <= 'Z')
			comm[i] = comm[i] - 'A' + 'a';
		else if (comm[i] >= 'a' && comm[i] <= 'z')
			continue;
		else
			return false;
	}

	return true;
}

