#define ZSKIPLIST_MAXLEVEL 32
#define ZSKIPLIST_P 0.25
#define ZSKIPLIST_RANDOM 999
#define STORE_FILE_NAME "data/skiplist_data.txt"
#define DELIMITER ":"

#include "SkipListNode.h"
#include <time.h>
#include <fstream>
#include <unordered_map>
using namespace std;


class SkipList
{
public:

	SkipList();

	~SkipList();

	// 插入节点
	bool zslInsert(const string& ele, double score);

	// 插入节点
	bool zslInsert(const string& ele,double score,int l);

	// 删除节点
	bool zslDelete(const string& ele, double score);

	// 更改元素 (ele,old_score) 为 (ele,new_score)
	bool zslUpdateScore(const string& ele, double old_score, double new_score);

	// 返回给定成员和分值的节点在跳表中的排位,返回 0 表示节点不存在
	long zslGetRank(const string& ele, double score);

	// 返回跳表在给定排位上的节点
	SkipListNode* zslGetElementByRank(long rank);

	// 给定一个分值范围[start,end]，如果该范围中至少存在一个节点，返回 true,否则 false
	bool zslIsInRange(double start, double end);

	// 给定一个分值范围[start,end],返回跳跃表中第一个符合这个范围的节点
	SkipListNode* zslFirstInRange(double start, double end);

	// 给定一个分值范围[start,end],返回跳跃表中最后一个符合这个范围的节点
	SkipListNode* zslLastInRange(double start, double end);

	// 给定一个排名范围[start,end],返回这个范围的节点,从低到高
	void zslRangeByRank(long start, long end, vector<SkipListNode*>& arr);

	// 给定一个排名范围[start,end],返回这个范围的节点,从高到低
	void zslRevRangeByRank(long start, long end, vector<SkipListNode*>& arr);

	// 给定一个分值范围[start,end],返回这个范围的节点,从低到高
	void zslRangeByScore(double start, double end, vector<SkipListNode*>& arr);

	// 给定一个分值范围[start,end],删除跳表中所有在这个范围内的节点,返回成功删除的数量
	long zslDeleteRangeByScore(double start, double end, unordered_map<string, double>& dict);

	// 给定一个排位范围[start,end],删除跳表中所有在这个范围内的节点,返回成功删除的数量
	long zslDeleteRangeByRank(int start, int end, unordered_map<string, double>& dict);

	// 统计分值范围[start,end]中的节点个数
	long zslCountRangByScore(double start, double end);

	// 跳表中的节点数
	long zslSize();

	// 跳表中的数据写入到硬盘
	void zslDumpFile();

	// 从持久化文件中加载数据
	void zslLoadFile();

	// 顺序遍历
	void traverse();

	// 逆序遍历
	void reverseTraverse();

	// 打印跳表结构
	void displaySkipList();
private:
	// 表头节点
	SkipListNode* m_head;

	// 表尾节点
	SkipListNode* m_tail;

	// 表中节点数量
	long m_length;

	// 表中层数最大的节点的层数
	int m_level;

	// 创建一个节点
	SkipListNode* zslCreateNode(int level, double score, const string& ele);

	// 获得随机的层数
	int zslRandomLevel();

	// 删除节点并调整跳表结构
	void zslDeleteNode(SkipListNode* p, SkipListNode** update);

	// 快速粗略检测该范围是否存在元素
	bool zslQuickCheckRange(double start, double end);
	
	// 从字符串中读取 ele 和 score
	bool get_ele_score_from_string(const string& str, string& ele, double& score);


};

