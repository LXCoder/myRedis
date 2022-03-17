#include "SkipList.h"

#include <algorithm>

using namespace std;

class ZSet
{
public:
	ZSet();

	~ZSet();

	// 插入成员
	bool zadd(const string& ele,double score);

	// 获取成员数量
	long zcard();

	// 计算在有序集合中指定区间分数的成员数
	long zcount(double start, double end);

	// 有序集合中对指定成员的分数加上增量 increment
	bool zincrby(const string& ele, double increment);

	// 通过索引区间返回有序集合指定区间内的成员，分数从低到高
	vector<SkipListNode*> zrange(long start, long stop);

	//返回有序集中指定区间内的成员，通过索引，分数从高到低
	vector<SkipListNode*> zrevRange(long start, long stop);

	// 通过索引区间返回有序集合指定区间内的成员，分数从低到高
	vector<SkipListNode*> zrangeByScore(double min, double max);

	//返回有序集中指定区间内的成员，通过索引，分数从高到低
	vector<SkipListNode*> zrevRangeByScore(double min, double max);

	// 返回有序集中指定成员的排名,按分数值递增(从小到大)顺序排列。
	long zrank(const string& ele);

	// 返回有序集中指定成员的排名,按分数值递增(从大到小)顺序排列
	long zrevRank(const string& ele);

	// 移除有序集中的一个或多个成员，不存在的成员将被忽略
	bool zrem(const string& ele);

	// 移除有序集合中给定的排名区间的所有成员
	long zremRangeByRank(long start, long stop);

	// 移除有序集合中给定的分数区间的所有成员
	long zremRangeByScore(double min, double max);

	// 返回有序集中，成员的分数值
	double zscore(const string& ele);

	// 返回重写用的元素,每 splitCount 个元素一行
	vector<string> zrwRange(int splitCount);

	// 显示跳表结构
	void zslDisplay();


private:
	// 字典
	unordered_map<string, double> m_dict;
	
	// 跳表
	SkipList* m_zsl;
};

