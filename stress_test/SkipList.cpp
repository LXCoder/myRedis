#include "SkipList.h"

SkipList::SkipList()
{
	srand(time(0));
	m_length = 0;
	m_level = 1;//��ǰ��߲���Ϊ 1
	// ����ͷ�ڵ㣬���Ĳ�����������
	m_head = zslCreateNode(ZSKIPLIST_MAXLEVEL, 0, "nil");
	// ��ͷ�ڵ�ÿһ���ǰ��ָ����Ϊ null,��ȸ�ֵΪ 0
	for (int i = 0; i < ZSKIPLIST_MAXLEVEL; ++i)
	{
		m_head->level[i].forward = nullptr;
		m_head->level[i].span = 0;
	}
	m_head->backward = nullptr;//ͷ�ڵ�ĺ���ָ��Ϊ null
	m_tail = nullptr;//��ǰ�ڵ���Ϊ 0��βָ��ָ�� null
}

SkipList::~SkipList()
{
	// �������е�ÿ���ڵ��ͷ�
	SkipListNode* p = m_head->level[0].forward;
	SkipListNode* t = nullptr;
	while (p)
	{
		t = p->level[0].forward;
		delete p;
		p = t;
	}
	delete m_head;
}

bool SkipList::zslInsert(const string& ele, double score)
{
	SkipListNode* update[ZSKIPLIST_MAXLEVEL];	// �洢����·��
	int rank[ZSKIPLIST_MAXLEVEL] = { 0 };		// �洢�����Ľڵ���

	// �Ҹýڵ�Ĳ���λ�ã�����¼�ò���λ��ÿһ���ǰһ���ڵ�Ϳ��
	SkipListNode* p = m_head;
	for (int i = m_level - 1; i >= 0; --i)
	{
		// rank[i] �洢 update[i] ���������е����������������Ӧ�ü�����һ�������
		// ������߲㣬��ʼΪ 0
		rank[i] = i == m_level - 1 ? 0 : rank[i + 1];
		// score ������������ score ��� ��val ���ֵ�������
		while (p->level[i].forward && (p->level[i].forward->get_score() < score ||
			(p->level[i].forward->get_score() == score && p->level[i].forward->get_value() < ele)))
		{
			rank[i] += p->level[i].span;
			p = p->level[i].forward;
		}
		update[i] = p;//�� i �����λ�õ�ǰһ���ڵ��� p
	}

	p = p->level[0].forward;
	// �ڵ� (ele,score) �Ѵ��ڣ�������룬���� false;
	if (p && p->get_score() == score && p->get_value() == ele)
		return false;

	int level = zslRandomLevel();//����������

	// ��� level ���ڵ�ǰ������������� �µĲ�Ĳ���λ��ǰһ���ڵ�
	if (level > m_level)
	{
		for (int i = m_level; i < level; ++i)
		{
			//�µĲ�Ĳ���λ�õ�ǰһ���ڵ���ͷ�ڵ㣬���Ϊ m_length
			rank[i] = 0;
			update[i] = m_head;
			update[i]->level[i].span = m_length;
		}
		m_level = level;//����������
	}

	SkipListNode* node = zslCreateNode(level, score, ele);
	// ����ýڵ�
	for (int i = 0; i < level; ++i)
	{
		node->level[i].forward = update[i]->level[i].forward;// �ýڵ��ڵ� i ���ǰ��ָ��ָ�� update[i] ��ǰ��ָ��
		update[i]->level[i].forward = node;// update[i] �� i ���ǰ��ָ��ָ��ýڵ�

		node->level[i].span = update[i]->level[i].span - (rank[0] - rank[i]);
		update[i]->level[i].span = (rank[0] - rank[i]) + 1;
	}

	// ����½ڵ�Ĳ���С�ڵ�ʱ�������������ϲ�δ���ӵĲ�Ŀ��Ҫ +1
	for (int i = level; i < m_level; ++i)
		update[i]->level[i].span++;

	// ���� �½ڵ�ĺ���ָ��
	node->backward = update[0] == m_head ? nullptr : update[0];

	// ����½ڵ�� 1 ���ǰ��ָ�벻Ϊ null,���ָ��ָ��Ķ���ĺ���ָ����Ҫָ���½ڵ�
	if (node->level[0].forward)
		node->level[0].forward->backward = node;
	else
		m_tail = node;//���δ null,���ʾ�������һ���ڵ㣬����βָ��

	++m_length;//�ڵ�����+1

	return true;
}

bool SkipList::zslInsert(const string& ele, double score, int l)
{
	SkipListNode* update[ZSKIPLIST_MAXLEVEL];	// �洢����·��
	int rank[ZSKIPLIST_MAXLEVEL] = { 0 };		// �洢�����Ľڵ���

	// �Ҹýڵ�Ĳ���λ�ã�����¼�ò���λ��ÿһ���ǰһ���ڵ�Ϳ��
	SkipListNode* p = m_head;
	for (int i = m_level - 1; i >= 0; --i)
	{
		// rank[i] �洢 update[i] ���������е����������������Ӧ�ü�����һ�������
		// ������߲㣬��ʼΪ 0
		rank[i] = i == m_level - 1 ? 0 : rank[i + 1];
		// score ������������ score ��� ��val ���ֵ�������
		while (p->level[i].forward && (p->level[i].forward->get_score() < score ||
			(p->level[i].forward->get_score() == score && p->level[i].forward->get_value() < ele)))
		{
			rank[i] += p->level[i].span;
			p = p->level[i].forward;
		}
		update[i] = p;//�� i �����λ�õ�ǰһ���ڵ��� p
	}

	p = p->level[0].forward;
	// �ڵ� (ele,score) �Ѵ��ڣ�������룬���� false;
	if (p && p->get_score() == score && p->get_value() == ele)
		return false;

	//int level = zslRandomLevel();//����������
	int level = l;
	// ��� level ���ڵ�ǰ������������� �µĲ�Ĳ���λ��ǰһ���ڵ�
	if (level > m_level)
	{
		for (int i = m_level; i < level; ++i)
		{
			//�µĲ�Ĳ���λ�õ�ǰһ���ڵ���ͷ�ڵ㣬���Ϊ m_length
			rank[i] = 0;
			update[i] = m_head;
			update[i]->level[i].span = m_length;
		}
		m_level = level;//����������
	}

	SkipListNode* node = zslCreateNode(level, score, ele);
	// ����ýڵ�
	for (int i = 0; i < level; ++i)
	{
		node->level[i].forward = update[i]->level[i].forward;// �ýڵ��ڵ� i ���ǰ��ָ��ָ�� update[i] ��ǰ��ָ��
		update[i]->level[i].forward = node;// update[i] �� i ���ǰ��ָ��ָ��ýڵ�

		node->level[i].span = update[i]->level[i].span - (rank[0] - rank[i]);
		update[i]->level[i].span = (rank[0] - rank[i]) + 1;
	}

	// ����½ڵ�Ĳ���С�ڵ�ʱ�������������ϲ�δ���ӵĲ�Ŀ��Ҫ +1
	for (int i = level; i < m_level; ++i)
		update[i]->level[i].span++;

	// ���� �½ڵ�ĺ���ָ��
	node->backward = update[0] == m_head ? nullptr : update[0];

	// ����½ڵ�� 1 ���ǰ��ָ�벻Ϊ null,���ָ��ָ��Ķ���ĺ���ָ����Ҫָ���½ڵ�
	if (node->level[0].forward)
		node->level[0].forward->backward = node;
	else
		m_tail = node;//���δ null,���ʾ�������һ���ڵ㣬����βָ��

	++m_length;//�ڵ�����+1

	return true;
}

bool SkipList::zslDelete(const string& ele, double score)
{
	SkipListNode* update[ZSKIPLIST_MAXLEVEL];	// �洢����·��

	SkipListNode* p = m_head;
	// ���ҽ��
	for (int i = m_level - 1; i >= 0; --i)
	{
		while (p->level[i].forward && (p->level[i].forward->get_score() < score ||
			p->level[i].forward->get_score() == score && p->level[i].forward->get_value() < ele))
		{
			p = p->level[i].forward;
		}
		update[i] = p;
	}

	// ������ҽ����������ɾ���ڵ���ڣ���ô p �Ǵ�ɾ���ڵ��ǰһ���ڵ�
	p = p->level[0].forward;
	// �ڵ��ֵ�ͽڵ�ֵ��ȵĻ����� p ���Ǵ�ɾ���ڵ�
	if (p && p->get_score() == score && p->get_value() == ele)
	{
		zslDeleteNode(p, update);// ��������Ľṹ
		delete p;				// �ͷ���Դ

		return true;
	}

	return false;
}

long SkipList::zslGetRank(const string& ele, double score)
{
	long rank = 0;
	SkipListNode* p = m_head;
	// ���ҽ��
	for (int i = m_level - 1; i >= 0; --i)
	{
		while (p->level[i].forward && (p->level[i].forward->get_score() < score ||
			p->level[i].forward->get_score() == score && p->level[i].forward->get_value() <= ele))
		{
			rank += p->level[i].span;
			p = p->level[i].forward;
		}

		// p ������ͷ�ڵ�
		/*if (p->get_score() == score && p->get_value() == ele)
			return rank;*/
		if (p->get_score() == score)
			return rank;
	}

	return 0;
}

bool SkipList::zslUpdateScore(const string& ele, double old_score, double new_score)
{
	SkipListNode* update[ZSKIPLIST_MAXLEVEL];	// �洢����·��

	SkipListNode* p = m_head;
	// ���ҽ��
	for (int i = m_level - 1; i >= 0; --i)
	{
		while (p->level[i].forward && (p->level[i].forward->get_score() < old_score ||
			p->level[i].forward->get_score() == old_score && p->level[i].forward->get_value() < ele))
		{
			p = p->level[i].forward;
		}
		update[i] = p;
	}

	p = p->level[0].forward;
	// �����ڸýڵ�
	if (p == nullptr || p->get_score() != old_score || p->get_value() != ele)
		return false;

	// ����µķ�ֵ������ԭ��λ�ã�ֱ���޸ķ�ֵ����
	if ((p->backward == nullptr || p->backward->get_score() < new_score) &&
		(p->level[0].forward == nullptr || p->level[0].forward->get_score() > new_score))
	{
		p->set_score(new_score);
		return true;
	}

	// �µķ�ֵ����ԭ��λ�ã���ɾ��ԭ�ڵ㣬�ٲ����½ڵ�
	zslDeleteNode(p, update);
	zslInsert(ele, new_score, p->level.size());
	delete p;

	return true;
}

SkipListNode* SkipList::zslGetElementByRank(long rank)
{
	// �������ڽڵ����������� nullptr
	if (rank > m_length)
		return nullptr;

	long m_rank = 0;
	SkipListNode* p = m_head;
	// ���ҽ��
	for (int i = m_level - 1; i >= 0; --i)
	{
		while (p->level[i].forward && (m_rank + p->level[i].span) <= rank)
		{
			m_rank += p->level[i].span;
			p = p->level[i].forward;
		}

		if (m_rank == rank)
			return p;
	}

	return nullptr;
}

bool SkipList::zslIsInRange(double start, double end)
{
	// ���Ҹ÷�Χ�ڵĵ�һ��Ԫ�أ������� nullptr �����÷�Χ�ڲ�����Ԫ��
	SkipListNode* ret = zslFirstInRange(start, end);

	if (ret == nullptr)
		return false;

	return true;

}

SkipListNode* SkipList::zslFirstInRange(double start, double end)
{
	// ��Χ�ڲ�����Ԫ�أ����� nullptr
	if (!zslQuickCheckRange(start, end))
		return nullptr;

	SkipListNode* p = m_head;
	for (int i = m_level - 1; i >= 0; --i)
	{
		while (p->level[i].forward && p->level[i].forward->get_score() < start)
			p = p->level[i].forward;
	}

	p = p->level[0].forward;

	if (p->get_score() > end)
		return nullptr;

	return p;
}

SkipListNode* SkipList::zslLastInRange(double start, double end)
{
	// ��Χ�ڲ�����Ԫ�أ����� nullptr
	if (!zslQuickCheckRange(start, end))
		return nullptr;

	SkipListNode* p = m_head;
	for (int i = m_level - 1; i >= 0; --i)
	{
		while (p->level[i].forward && p->level[i].forward->get_score() <= end)
			p = p->level[i].forward;
	}

	if (p->get_score() < start)
		return nullptr;

	return p;
}

void SkipList::zslRangeByRank(long start, long end, vector<SkipListNode*>& arr)
{
	if (start <= 0)
		start = 1;

	if (start > end || start > m_length)
		return;

	SkipListNode* p = m_head;
	long rank = 0;
	for (int i = m_level - 1; i >= 0; --i)
	{
		while (p->level[i].forward && (rank + p->level[i].span) < start)
		{
			rank += p->level[0].span;
			p = p->level[0].forward;
		}
	}

	++rank;
	p = p->level[0].forward;

	while (p && rank <= end)
	{
		arr.push_back(p);
		p = p->level[0].forward;
		++rank;
	}
}

void SkipList::zslRevRangeByRank(long start, long end, vector<SkipListNode*>& arr)
{
	if (start <= 0)
		start = 1;

	if (start > end || start > m_length)
		return;

	SkipListNode* p = m_tail;
	int idx = 1;
	while (p && idx < start)
	{
		p = p->backward;
		++idx;
	}

	while (p && idx <= end)
	{
		arr.push_back(p);
		p = p->backward;
		++idx;
	}

}

void SkipList::zslRangeByScore(double start, double end, vector<SkipListNode*>& arr)
{
	SkipListNode* p = m_head;
	// ���ҽ��
	for (int i = m_level - 1; i >= 0; --i)
	{
		while (p->level[i].forward && p->level[i].forward->get_score() < start)
			p = p->level[i].forward;
	}

	p = p->level[0].forward;

	while (p && p->get_score() <= end)
	{
		arr.push_back(p);
		p = p->level[0].forward;
	}
}

long SkipList::zslDeleteRangeByScore(double start, double end, unordered_map<string, double>& dict)
{
	long removed = 0;
	SkipListNode* update[ZSKIPLIST_MAXLEVEL];	// �洢����·��
	SkipListNode* p = m_head;
	// ���ҽ��
	for (int i = m_level - 1; i >= 0; --i)
	{
		while (p->level[i].forward && p->level[i].forward->get_score() < start)
			p = p->level[i].forward;

		update[i] = p;
	}

	p = p->level[0].forward;

	while (p && p->get_score() <= end)
	{
		SkipListNode* next = p->level[0].forward;
		zslDeleteNode(p, update);	// ɾ�������еĽڵ�
		dict.erase(p->get_value());	// ɾ���ֵ��еĽڵ�
		delete p;					// �ͷ���Դ
		p = next;
		++removed;
	}

	return removed;
}

long SkipList::zslDeleteRangeByRank(int start, int end, unordered_map<string, double>& dict)
{
	if (start <= 0)
		start = 1;

	if (start > end || start > m_length)
		return 0;

	long removed = 0;
	long rank = 0;

	SkipListNode* update[ZSKIPLIST_MAXLEVEL];	// �洢����·��
	SkipListNode* p = m_head;
	// ���ҽ��
	for (int i = m_level - 1; i >= 0; --i)
	{
		while (p->level[i].forward && (rank + p->level[i].span) < start)
		{
			rank += p->level[i].span;
			p = p->level[i].forward;
		}

		update[i] = p;
	}

	++rank;
	p = p->level[0].forward;

	while (p && rank <= end)
	{
		SkipListNode* next = p->level[0].forward;
		zslDeleteNode(p, update);	// ɾ�������еĽڵ�
		dict.erase(p->get_value());	// ɾ���ֵ��еĽڵ�
		delete p;					// �ͷ���Դ
		p = next;
		++removed;
		++rank;
	}

	return removed;
}

long SkipList::zslCountRangByScore(double start, double end)
{
	SkipListNode* p = zslFirstInRange(start, end);

	if (p == nullptr)
		return 0;

	long cnt = 0;
	while (p && p->get_score() <= end)
	{
		++cnt;
		p = p->level[0].forward;
	}

	return cnt;
}

void SkipList::zslDumpFile()
{
	// ���������
	ofstream ofs;
	// ���ļ�
	ofs.open(STORE_FILE_NAME, ios::out);

	cout << "Dumping Data......." << endl;
	SkipListNode* p = m_head->level[0].forward;

	while (p)
	{
		string str = p->get_value() + DELIMITER + to_string(p->get_score());//ƴ������
		ofs << str << endl;//д��Ӳ��
		p = p->level[0].forward;
	}

	ofs.close();

	cout << "Dump Data Finished." << endl;
}

void SkipList::zslLoadFile()
{
	// ����������
	ifstream ifs;
	// ���ļ�
	ifs.open(STORE_FILE_NAME, ios::in);
	cout << "Loadding Data........" << endl;
	if (!ifs.is_open())
	{
		cout << "�ļ���ʧ��..." << endl;
		return;
	}

	string line;
	string ele = "";
	double score = 0.0;
	// ÿ�ж�ȡ����
	while (getline(ifs, line))
	{
		if (get_ele_score_from_string(line, ele, score))
			zslInsert(ele, score);
	}

	// �ر���
	ifs.close();

	cout << "Load Data Finished." << endl;
}

void SkipList::traverse()
{
	SkipListNode* p = m_head->level[0].forward;
	while (p)
	{
		cout << "[score:" << p->get_score() << ",ele:" << p->get_value() << "]" << endl;
		p = p->level[0].forward;
	}
	cout << endl;
}

void SkipList::reverseTraverse()
{
	SkipListNode* p = m_tail;
	while (p)
	{
		cout << "[score:" << p->get_score() << ",ele:" << p->get_value() << "]" << endl;
		p = p->backward;
	}
	cout << endl;
}

void SkipList::displaySkipList()
{
	cout << "\n*****Skip List*****" << "\n";
	for (int i = m_level - 1; i >= 0; --i)
	{
		cout << "Level " << i << ":";
		cout << "  " << m_head->level[i].span << "  ";
		SkipListNode* p = m_head->level[i].forward;
		while (p)
		{
			cout << "[score:" << p->get_score() << ",ele:" << p->get_value() << "]  ";
			cout << "  " << p->level[i].span << "  ";
			p = p->level[i].forward;
		}
		cout << endl;
	}
}

SkipListNode* SkipList::zslCreateNode(int level, double score, const string& ele)
{
	return new SkipListNode(level, ele, score);
}

int SkipList::zslRandomLevel()
{
	/*int level = 1;
	float t = (rand() % (ZSKIPLIST_RANDOM + 1)) / (float)(ZSKIPLIST_RANDOM + 1);
	while (t < ZSKIPLIST_P &&
		level < ZSKIPLIST_MAXLEVEL)
	{
		level += 1;
		//cout << t << endl;
		t = (rand() % (ZSKIPLIST_RANDOM + 1)) / (float)(ZSKIPLIST_RANDOM + 1);
	}
	//cout << endl;
	return level;*/

	int k = 1;
	while (rand() % 2) {
		k++;
	}
	k = (k < ZSKIPLIST_MAXLEVEL) ? k : ZSKIPLIST_MAXLEVEL;
	return k;
}

void SkipList::zslDeleteNode(SkipListNode* p, SkipListNode** update)
{
	// ����ÿһ������ӽṹ
	for (int i = 0; i < m_level; ++i)
	{
		// �� i ������ p
		if (update[i]->level[i].forward == p)
		{
			update[i]->level[i].forward = p->level[i].forward;	// update[i]�ĵ� i ��ǰ��ָ��ָ�� p �� i ��ǰ��ָ��ָ��Ľڵ�
			update[i]->level[i].span += p->level[i].span - 1;	// update[i] �� p �� i ���Ⱥͼ�ȥ 1�������¿��
		}
		else//�� i ��û���� p
		{
			update[i]->level[i].span -= 1;//��� -1
		}

	}
	if (p->level[0].forward)
		p->level[0].forward->backward = p->backward;	//���º���ָ��
	else
		m_tail = p->backward;							// ����βָ��

	// ���ɾ���Ľڵ���ĳЩ��ֻ����һ���ڵ㣬����Ҫ���� m_level
	while (m_level > 1 && m_head->level[m_level - 1].forward == nullptr)
		--m_level;

	--m_length;		//�ڵ������� 1
}

bool SkipList::zslQuickCheckRange(double start, double end)
{
	// start > end ���� false
	if (start > end)
		return false;

	// �����е�����ֵС�ڷ�Χ����㣬�������в����ڸ÷�Χ�ķ�ֵ
	if (m_tail == nullptr || m_tail->get_score() < start)
		return false;

	// �����е���С��ֵ���ڷ�Χ���յ㣬�������в����ڸ÷�Χ�ķ�ֵ
	SkipListNode* p = m_head->level[0].forward;
	if (p == nullptr || p->get_score() > end)
		return false;

	return true;
}

bool SkipList::get_ele_score_from_string(const string& str, string& ele, double& score)
{
	// �ַ���Ϊ�գ����� false
	if (str.empty())
		return false;

	// �ַ��������ڷָ�����ָ������ַ���ĩβ������ false
	size_t idx = str.find(DELIMITER);
	if (idx == string::npos || idx == str.size() - 1)
		return false;

	ele = str.substr(0, idx);

	try {
		score = stod(str.substr(idx + 1));
	}
	catch (std::invalid_argument) {
		return false;
	}

	return true;
}

