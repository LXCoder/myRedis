#include <iostream>
#include <vector>
#include <string>
using namespace std;

class SkipListNode;//ǰ������

class SkipListLevel {
public:
	// ǰ��ָ��
	SkipListNode* forward;

	// ���
	int span;
};


class SkipListNode
{
public:
	// ��
	vector<SkipListLevel> level;

	// ����ָ��
	SkipListNode* backward;

	SkipListNode() = default;

	SkipListNode(int levels, const string& ele, double score);

	~SkipListNode();

	string get_value();

	double get_score();

	void set_value(string v);

	void set_score(double score);
private:

	// ��ֵ
	double m_score;

	// ֵ
	string m_val;
};
