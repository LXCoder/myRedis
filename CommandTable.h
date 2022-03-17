#define EMPTYSET "(empty list or set)"
#define ZEROELE "(integer) 0"
#define ONEELE "(integer) 1"
#define INTSTR "(integer) "
#define UNEXIST "(nil)"
#define SPACE "   "
#define AOF_REWRITE_ITEMS_PER_CMD 64

#include "ZSet.h"
#include <regex>




class CommandArg{
public:

	CommandArg():min(0.0),max(0.0),start(0),stop(-1),withscores(false)
	{
		eles.clear();
	}

	string comm_name;//命令名字
	vector<string> key_names;//键名

	vector<pair<string,double>> eles;
	double min;
	double max;
	long start;
	long stop;
	bool withscores;
};

class CommandInfo {
public:

	CommandInfo() {}

	CommandInfo(string name, string error, int mask, bool isWrite) :m_name(name), m_error(error), m_mask(mask), m_isWrite(isWrite)
	{

	}

	string get_name(){	return m_name;	}

	string get_error(){	return m_error;	}

	int get_mask(){	return m_mask;	}

	bool get_isWrite(){	return m_isWrite;	}

private:
	string m_name;
	string m_error;
	int m_mask;
	bool m_isWrite;
};

enum Command {
	ZADD,
	ZCARD,
	ZCOUNT,
	ZINCRBY,
	ZRANGE,
	ZREVRANGE,
	ZRANGEBYSCORE,
	ZREVRANGEBYSCORE,
	ZRANK,
	ZREVRANK,
	ZREM,
	ZREMRANGEBYRANK,
	ZREMRANGEBYSCORE,
	ZSCORE, 
	DEL,
	EXISTS,
	KEYS
};

enum CommandId {
	ZADD_ID = 1,
	ZCARD_ID = 2,
	ZCOUNT_ID = 4,
	ZINCRBY_ID = 8,
	ZRANGE_ID = 16,
	ZREVRANGE_ID = 32,
	ZRANGEBYSCORE_ID = 64,
	ZREVRANGEBYSCORE_ID = 128,
	ZRANK_ID = 256,
	ZREVRANK_ID = 512,
	ZREM_ID = 1024,
	ZREMRANGEBYRANK_ID = 2048,
	ZREMRANGEBYSCORE_ID = 4096,
	ZSCORE_ID = 8192,
	DEL_ID = 16384,
	EXISTS_ID = 32768,
	KEYS_ID = 65536,
};

static const vector<string> command_name = {
	"zadd",
	"zcard",
	"zcount",
	"zincrby",
	"zrange",
	"zrevrange",
	"zrangebyscore",
	"zrevrangebyscore",
	"zrank",
	"zrevrank",
	"zrem",
	"zremrangebyrank",
	"zremrangebyscore",
	"zscore",
	"del",
	"exists",
	"keys"
};

static const vector<string> command_err_reply = {
	SPACE,
	ZEROELE,
	ZEROELE,
	SPACE,
	EMPTYSET,
	EMPTYSET,
	EMPTYSET,
	EMPTYSET,
	UNEXIST,
	UNEXIST,
	ZEROELE,
	ZEROELE,
	ZEROELE,
	UNEXIST,
	SPACE,
	SPACE,
	SPACE
};

static const vector<bool> command_write = {
	true,
	false,
	false,
	true,
	false,
	false,
	false,
	false,
	false,
	false,
	true,
	true,
	true,
	false,
	true,
	false,
	false,
};

bool splitString(const string& str, char separator, vector<string>& arr);

class CommandTable
{
public:

	void addCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string& reply);

	void cardCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string& reply);

	void countCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string& reply);

	void incrbyCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string& reply);

	void rangeCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string& reply);

	void revRangeCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string& reply);

	void rangeByScoreCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string& reply);

	void revRangeByScoreCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string& reply);

	void rankCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string& reply);

	void revRankCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string& reply);

	void remCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string& reply);

	void remRangeByRankCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string& reply);

	void remRangeByScoreCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string& reply);

	void scoreCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string& reply);

	void delCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string& reply);

	void existsCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string& reply);

	void keysCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string& reply);


private:

	void buildReply(const vector<SkipListNode*>& arr, string& reply, bool withscore);

	bool matchKey(const string & s, const string & p);
};


