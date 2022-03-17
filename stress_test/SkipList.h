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

	// ����ڵ�
	bool zslInsert(const string& ele, double score);

	// ����ڵ�
	bool zslInsert(const string& ele, double score, int l);

	// ɾ���ڵ�
	bool zslDelete(const string& ele, double score);

	// ����Ԫ�� (ele,old_score) Ϊ (ele,new_score)
	bool zslUpdateScore(const string& ele, double old_score, double new_score);

	// ���ظ�����Ա�ͷ�ֵ�Ľڵ��������е���λ,���� 0 ��ʾ�ڵ㲻����
	long zslGetRank(const string& ele, double score);

	// ���������ڸ�����λ�ϵĽڵ�
	SkipListNode* zslGetElementByRank(long rank);

	// ����һ����ֵ��Χ[start,end]������÷�Χ�����ٴ���һ���ڵ㣬���� true,���� false
	bool zslIsInRange(double start, double end);

	// ����һ����ֵ��Χ[start,end],������Ծ���е�һ�����������Χ�Ľڵ�
	SkipListNode* zslFirstInRange(double start, double end);

	// ����һ����ֵ��Χ[start,end],������Ծ�������һ�����������Χ�Ľڵ�
	SkipListNode* zslLastInRange(double start, double end);

	// ����һ��������Χ[start,end],���������Χ�Ľڵ�,�ӵ͵���
	void zslRangeByRank(long start, long end, vector<SkipListNode*>& arr);

	// ����һ��������Χ[start,end],���������Χ�Ľڵ�,�Ӹߵ���
	void zslRevRangeByRank(long start, long end, vector<SkipListNode*>& arr);

	// ����һ����ֵ��Χ[start,end],���������Χ�Ľڵ�,�ӵ͵���
	void zslRangeByScore(double start, double end, vector<SkipListNode*>& arr);

	// ����һ����ֵ��Χ[start,end],ɾ�������������������Χ�ڵĽڵ�,���سɹ�ɾ��������
	long zslDeleteRangeByScore(double start, double end, unordered_map<string, double>& dict);

	// ����һ����λ��Χ[start,end],ɾ�������������������Χ�ڵĽڵ�,���سɹ�ɾ��������
	long zslDeleteRangeByRank(int start, int end, unordered_map<string, double>& dict);

	// ͳ�Ʒ�ֵ��Χ[start,end]�еĽڵ����
	long zslCountRangByScore(double start, double end);

	// �����е�����д�뵽Ӳ��
	void zslDumpFile();

	// �ӳ־û��ļ��м�������
	void zslLoadFile();

	// ˳�����
	void traverse();

	// �������
	void reverseTraverse();

	// ��ӡ����ṹ
	void displaySkipList();

	// �����еĽڵ���
	inline long zslSize() {
		return m_length;
	}

	//
	inline long maxLevel(){
		return m_level;
	}
private:
	// ��ͷ�ڵ�
	SkipListNode* m_head;

	// ��β�ڵ�
	SkipListNode* m_tail;

	// ���нڵ�����
	long m_length;

	// ���в������Ľڵ�Ĳ���
	int m_level;

	// ����һ���ڵ�
	SkipListNode* zslCreateNode(int level, double score, const string& ele);

	// �������Ĳ���
	int zslRandomLevel();

	// ɾ���ڵ㲢��������ṹ
	void zslDeleteNode(SkipListNode* p, SkipListNode** update);

	// ���ٴ��Լ��÷�Χ�Ƿ����Ԫ��
	bool zslQuickCheckRange(double start, double end);

	// ���ַ����ж�ȡ ele �� score
	bool get_ele_score_from_string(const string& str, string& ele, double& score);


};

