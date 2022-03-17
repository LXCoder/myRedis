#include "CommandTable.h"

void CommandTable::addCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string& reply)
{
	ZSet* zset = nullptr;
	// 即使 key 不存在，也会自动生成一个新的 key 进行操作
	if (key_dict.count(args.key_names[0]) == 0)
	{
		zset = new ZSet();
		key_dict.insert(make_pair(args.key_names[0], zset));
	}
	else
	{
		zset = key_dict[args.key_names[0]];
	}

	long cnt = 0;
	for (int i = 0, n = args.eles.size();i < n;++i)
	{
		if (zset->zadd(args.eles[i].first, args.eles[i].second))
			++cnt;
	}

	reply.append(INTSTR);
	reply.append(to_string(cnt));
}

void CommandTable::cardCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string& reply)
{
	if (key_dict.count(args.key_names[0]) == 0)
	{
		reply = command_err_reply[ZCARD];
		return;
	}

	ZSet* zset = key_dict[args.key_names[0]];
	reply.append(INTSTR);
	reply.append(to_string(zset->zcard()));
}

void CommandTable::countCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string & reply)
{
	if (key_dict.count(args.key_names[0]) == 0)
	{
		reply = command_err_reply[ZCOUNT];
		return;
	}

	ZSet* zset = key_dict[args.key_names[0]];
	reply.append(INTSTR);
	reply.append(to_string(zset->zcount(args.min, args.max)));
}

void CommandTable::incrbyCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string & reply)
{

	ZSet* zset = nullptr;
	// 即使 key 不存在，也会自动生成一个新的 key 进行操作
	if (key_dict.count(args.key_names[0]) == 0)
	{
		zset = new ZSet();
		key_dict.insert(make_pair(args.key_names[0], zset));
	}
	else
	{
		zset = key_dict[args.key_names[0]];
	}

	if (!zset->zincrby(args.eles[0].first, args.eles[0].second))
	{
		reply = UNEXIST;
		return;
	}

	reply.append(INTSTR);
	reply.append(to_string(zset->zscore(args.eles[0].first)));
}

void CommandTable::rangeCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string & reply)
{
	if (key_dict.count(args.key_names[0]) == 0)
	{
		reply = command_err_reply[ZRANGE];
		return;
	}

	ZSet* zset = key_dict[args.key_names[0]];
	vector<SkipListNode*> arr = zset->zrange(args.start, args.stop);
	if (arr.empty())
	{
		reply = EMPTYSET;
		return;
	}
	buildReply(arr, reply,args.withscores);
}

void CommandTable::revRangeCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string & reply)
{
	if (key_dict.count(args.key_names[0]) == 0)
	{
		reply = command_err_reply[ZREVRANGE];
		return;
	}

	ZSet* zset = key_dict[args.key_names[0]];
	vector<SkipListNode*> arr = zset->zrevRange(args.start, args.stop);
	if (arr.empty())
	{
		reply = EMPTYSET;
		return;
	}
	buildReply(arr, reply, args.withscores);
}

void CommandTable::rangeByScoreCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string & reply)
{
	if (key_dict.count(args.key_names[0]) == 0)
	{
		reply = command_err_reply[ZRANGEBYSCORE];
		return;
	}

	ZSet* zset = key_dict[args.key_names[0]];
	vector<SkipListNode*> arr = zset->zrangeByScore(args.min, args.max);
	if (arr.empty())
	{
		reply = EMPTYSET;
		return;
	}
	buildReply(arr, reply, args.withscores);
}

void CommandTable::revRangeByScoreCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string & reply)
{
	if (key_dict.count(args.key_names[0]) == 0)
	{
		reply = command_err_reply[ZREVRANGEBYSCORE];
		return;
	}

	ZSet* zset = key_dict[args.key_names[0]];
	vector<SkipListNode*> arr = zset->zrevRangeByScore(args.min, args.max);
	if (arr.empty())
	{
		reply = EMPTYSET;
		return;
	}

	buildReply(arr, reply, args.withscores);
}

void CommandTable::rankCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string & reply)
{
	if (key_dict.count(args.key_names[0]) == 0)
	{
		reply = command_err_reply[ZRANK];
		return;
	}

	ZSet* zset = key_dict[args.key_names[0]];
	int ret = zset->zrank(args.eles[0].first);
	if (ret != 0)
		reply = to_string(ret-1);
	else
		reply = UNEXIST;
}

void CommandTable::revRankCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string & reply)
{
	if (key_dict.count(args.key_names[0]) == 0)
	{
		reply = command_err_reply[ZREVRANK];
		return;
	}

	ZSet* zset = key_dict[args.key_names[0]];
	int ret = zset->zrevRank(args.eles[0].first);
	if (ret != 0)
		reply = to_string(ret - 1);
	else
		reply = UNEXIST;
}

void CommandTable::remCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string & reply)
{
	if (key_dict.count(args.key_names[0]) == 0)
	{
		reply = command_err_reply[ZREM];
		return;
	}

	ZSet* zset = key_dict[args.key_names[0]];
	long removed = 0;
	for (pair<string,double> ele : args.eles)
	{
		if (zset->zrem(ele.first))
			++removed;
	}
	reply.append(INTSTR);
	reply.append(to_string(removed));

	// 该键无元素，删除键
	if (zset->zcard() == 0)
		key_dict.erase(args.key_names[0]);
}

void CommandTable::remRangeByRankCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string & reply)
{
	if (key_dict.count(args.key_names[0]) == 0)
	{
		reply = command_err_reply[ZREMRANGEBYRANK];
		return;
	}

	ZSet* zset = key_dict[args.key_names[0]];
	reply.append(INTSTR);
	reply.append(to_string(zset->zremRangeByRank(args.start, args.stop)));

	// 该键无元素，删除键
	if (zset->zcard() == 0)
		key_dict.erase(args.key_names[0]);
}

void CommandTable::remRangeByScoreCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string & reply)
{
	if (key_dict.count(args.key_names[0]) == 0)
	{
		reply = command_err_reply[ZREMRANGEBYSCORE];
		return;
	}

	ZSet* zset = key_dict[args.key_names[0]];
	reply.append(INTSTR);
	reply.append(to_string(zset->zremRangeByScore(args.min, args.max)));

	// 该键无元素，删除键
	if (zset->zcard() == 0)
		key_dict.erase(args.key_names[0]);
}

void CommandTable::scoreCommand(unordered_map<string,ZSet*>& key_dict, const CommandArg& args, string & reply)
{
	if (key_dict.count(args.key_names[0]) == 0)
	{
		reply = command_err_reply[ZSCORE];
		return;
	}

	ZSet* zset = key_dict[args.key_names[0]];
	double ret = zset->zscore(args.eles[0].first);
	if (ret == -1)
	{
		reply = UNEXIST;
		return;
	}

	reply = to_string(ret);
}

void CommandTable::delCommand(unordered_map<string, ZSet*>& key_dict, const CommandArg & args, string & reply)
{
	long cnt = 0;
	for (int i = 0, n = args.key_names.size();i < n;++i)
	{
		if (key_dict.count(args.key_names[i]) == 1)
		{
			ZSet* zset = key_dict[args.key_names[i]];
			key_dict[args.key_names[i]] = nullptr;
			key_dict.erase(args.key_names[i]);		// 从键空间删除
			delete zset;							// 释放资源
			++cnt;
		}
	}

	reply.append(INTSTR);
	reply.append(to_string(cnt));
}

void CommandTable::existsCommand(unordered_map<string, ZSet*>& key_dict, const CommandArg & args, string & reply)
{
	if (key_dict.count(args.key_names[0]) == 0)
		reply = ZEROELE;
	else
		reply = ONEELE;
}

void CommandTable::keysCommand(unordered_map<string, ZSet*>& key_dict, const CommandArg & args, string & reply)
{
	long no = 1;
	if (args.key_names[0] == "*")
	{
		for (auto it = key_dict.begin();it != key_dict.end();++it)
		{
			reply.append(to_string(no++));
			reply.append(") \"");
			reply.append(it->first);
			reply.append("\"\n");
		}
	}
	else
	{
		string temp = args.key_names[0];
		string pattern = "";
		for (int i = 0, n = temp.size();i < n;++i)
		{
			if (temp[i] == '*')
				pattern.push_back('.');
			pattern.push_back(temp[i]);
		}

		for (auto it = key_dict.begin();it != key_dict.end();++it)
		{
			if (matchKey(it->first, pattern))
			{
				reply.append(to_string(no++));
				reply.append(") \"");
				reply.append(it->first);
				reply.append("\"\n");
			}
		}
	}
}

void CommandTable::buildReply(const vector<SkipListNode*>& arr, string & reply, bool withscore)
{
	long no = 1;
	if (withscore)
	{
		for (int i = 0, n = arr.size();i < n;++i)
		{
			reply.append(to_string(no++));
			reply.append(") \"");
			reply.append(arr[i]->get_value());
			reply.append("\"\n");
			reply.append(to_string(no++));
			reply.append(") ");
			reply.append(to_string(arr[i]->get_score()));
			reply.append("\n");
		}
	}
	else
	{
		for (int i = 0, n = arr.size();i < n;++i)
		{
			reply.append(to_string(no++));
			reply.append(") \"");
			reply.append(arr[i]->get_value());
			reply.append("\"\n");
		}
	}
}

bool CommandTable::matchKey(const string & s, const string & p)
{
	int m = p.size(), n = s.size();
	// dp[i][j] 表示 s 的前 j 个字符是否匹配 p 的前 i 个字符
	vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
	// 初始化
	dp[0][0] = 1;
	for (int i = 2;i <= m;++i)
	{
		// 如 c*a*，空串也能匹配
		if (p[i - 1] == '*')
			dp[i][0] = dp[i - 2][0];
	}

	for (int i = 1;i <= m;++i)
	{
		for (int j = 1;j <= n;++j)
		{
			if (p[i - 1] == '*')
			{
				// s[j-1] 能够与 * 前面的字符 p[i-2] 匹配,若 p[i-2] 为‘.’,也能匹配
				if (p[i - 2] == '.' || s[j - 1] == p[i - 2])
					dp[i][j] = dp[i - 2][j] || dp[i][j - 1];
				else
					dp[i][j] = dp[i - 2][j];//不能匹配就匹配0个p[i-2]
			}
			else
			{
				// 若 p[i-1] 为 点 或 与 s[j-1] 相等，则为true
				dp[i][j] = (p[i - 1] == '.' || s[j - 1] == p[i - 1]) && dp[i - 1][j - 1];
			}

		}
	}
	return dp[m][n];
}

bool splitString(const string & str, char separator, vector<string>& arr)
{
	int n = str.size();
	int left = 0, right = n - 1;

	// 去掉前后空格
	while (left < n && str[left] == ' ')
		++left;
	while (right >= 0 && str[right] == ' ')
		--right;

	if (left > right)
		return false;

	string temp = "";

	// 按空格分割命令
	while (left <= right)
	{
		if (str[left] == separator)
		{
			if (!temp.empty())
			{
				arr.push_back(temp);
				temp.clear();
			}
		}
		else
			temp.push_back(str[left]);

		++left;
	}
	if (!temp.empty())
		arr.push_back(temp);

	return true;
}
