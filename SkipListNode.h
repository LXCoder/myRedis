#include <iostream>
#include <vector>
#include <string>
using namespace std;

class SkipListNode;//前置声明

class SkipListLevel {
public:
	// 前进指针
	SkipListNode* forward;

	// 跨度
	int span;
};


class SkipListNode
{
public:
	// 层
	vector<SkipListLevel> level;

	// 后退指针
	SkipListNode* backward;

	SkipListNode();

	SkipListNode(int levels, const string& ele, double score);

	~SkipListNode();

	string get_value();

	double get_score();

	void set_value(string v);

	void set_score(double score);
private:
	
	// 分值
	double m_score;

	// 值
	string m_val;
};
