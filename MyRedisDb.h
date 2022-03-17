#include "CommandTable.h"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>

#define AOF_FILE "data/aof.txt"
#define AOF_REWRITE_FILE "data/aof_rewrite.txt"

using funcptr = void(CommandTable::*)(unordered_map<string, ZSet*>&, const CommandArg&, string&);

class MyRedisDb
{

public:
	MyRedisDb();

	~MyRedisDb();

	// 工作函数
	string doWork(string command);

private:
	// 键空间 <键名,数据>
	unordered_map<string, ZSet*> m_key_dict;
	
	// 命令表 <命令名字,命令处理函数>
	unordered_map<string, funcptr> m_command_table;

	// 命令信息
	unordered_map<string, CommandInfo> m_command_info;

	// 命令处理函数
	CommandTable* m_comm_proc;

	// AOF 缓冲区
	static string m_aof_buf;

	// AOF 重写缓冲区
	static string m_aof_rewrite_buf;

	// AOF 重写状态 : 0 表示开始重写，-1 表示未在重写
	static int m_aof_state;

	// 初始化工作
	void initWork();

	// 初始化命令表
	void initCommandTable();

	// 注册信号
	void initSignal();

	// 设计定时器
	void initTimer();

	// 载入 AOF 文件
	void loadAOFFile();

	// AOF 持久化
	void flushAofFile();

	// AOF 重写
	void aofRewrite();

	// 把重写缓冲区的内容写入到重写文件
	static void signalHandler(int signo);

	// 执行命令语句
	string runCommand(const string & command);

	// 检查命令语句
	bool checkCommand(const string& command, CommandArg& comm_arg, string& reply);

	// 分割命令语句
	bool splitCommand(const string& command, vector<string>& args);

	// 检查参数
	bool checkArgument(const vector<string>& args, CommandArg& comm_arg, string& reply);

	// 检测是否合法的 long 类型
	bool checkLongType(const string& val);

	// 检测是否合法的 double 类型
	bool checkDoubleType(const string& val);

	// 把命令转化为小写
	bool downcase(string& comm);
};



