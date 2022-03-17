#include "SkipList.h"

SkipList::SkipList()
{
	srand(time(0));
	m_length = 0;
	m_level = 1;//当前最高层数为 1
	// 创建头节点，它的层数是最大层数
	m_head = zslCreateNode(ZSKIPLIST_MAXLEVEL, 0, "nil");
	// 把头节点每一层的前进指针置为 null,跨度赋值为 0
	for (int i = 0; i < ZSKIPLIST_MAXLEVEL; ++i)
	{
		m_head->level[i].forward = nullptr;
		m_head->level[i].span = 0;
	}
	m_head->backward = nullptr;//头节点的后退指针为 null
	m_tail = nullptr;//当前节点数为 0，尾指针指向 null
}

SkipList::~SkipList()
{
	// 把跳表中的每个节点释放
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
	SkipListNode* update[ZSKIPLIST_MAXLEVEL];	// 存储搜索路径
	int rank[ZSKIPLIST_MAXLEVEL] = { 0 };		// 存储经过的节点跨度

	// 找该节点的插入位置，并记录该插入位置每一层的前一个节点和跨度
	SkipListNode* p = m_head;
	for (int i = m_level - 1; i >= 0; --i)
	{
		// rank[i] 存储 update[i] 的在跳表中的排名，正常情况下应该加上上一层的排名
		// 若是最高层，初始为 0
		rank[i] = i == m_level - 1 ? 0 : rank[i + 1];
		// score 按升序排序，若 score 相等 ，val 按字典序排序
		while (p->level[i].forward && (p->level[i].forward->get_score() < score ||
			(p->level[i].forward->get_score() == score && p->level[i].forward->get_value() < ele)))
		{
			rank[i] += p->level[i].span;
			p = p->level[i].forward;
		}
		update[i] = p;//第 i 层插入位置的前一个节点是 p
	}

	p = p->level[0].forward;
	// 节点 (ele,score) 已存在，无需插入，返回 false;
	if (p && p->get_score() == score && p->get_value() == ele)
		return false;

	int level = zslRandomLevel();//获得随机层数

	// 如果 level 大于当前最大层数，则更新 新的层的插入位置前一个节点
	if (level > m_level)
	{
		for (int i = m_level; i < level; ++i)
		{
			//新的层的插入位置的前一个节点是头节点，跨度为 m_length
			rank[i] = 0;
			update[i] = m_head;
			update[i]->level[i].span = m_length;
		}
		m_level = level;//更新最大层数
	}

	SkipListNode* node = zslCreateNode(level, score, ele);
	// 插入该节点
	for (int i = 0; i < level; ++i)
	{
		node->level[i].forward = update[i]->level[i].forward;// 该节点在第 i 层的前进指针指向 update[i] 的前进指针
		update[i]->level[i].forward = node;// update[i] 第 i 层的前进指针指向该节点

		node->level[i].span = update[i]->level[i].span - (rank[0] - rank[i]);
		update[i]->level[i].span = (rank[0] - rank[i]) + 1;
	}

	// 如果新节点的层数小于当时的最大层数，则上层未连接的层的跨度要 +1
	for (int i = level; i < m_level; ++i)
		update[i]->level[i].span++;

	// 更新 新节点的后退指针
	node->backward = update[0] == m_head ? nullptr : update[0];

	// 如果新节点第 1 层的前进指针不为 null,则该指针指向的对象的后退指针需要指向新节点
	if (node->level[0].forward)
		node->level[0].forward->backward = node;
	else
		m_tail = node;//如果未 null,则表示它是最后一个节点，更新尾指针

	++m_length;//节点数量+1

	return true;
}

bool SkipList::zslInsert(const string& ele, double score, int l)
{
	SkipListNode* update[ZSKIPLIST_MAXLEVEL];	// 存储搜索路径
	int rank[ZSKIPLIST_MAXLEVEL] = { 0 };		// 存储经过的节点跨度

	// 找该节点的插入位置，并记录该插入位置每一层的前一个节点和跨度
	SkipListNode* p = m_head;
	for (int i = m_level - 1; i >= 0; --i)
	{
		// rank[i] 存储 update[i] 的在跳表中的排名，正常情况下应该加上上一层的排名
		// 若是最高层，初始为 0
		rank[i] = i == m_level - 1 ? 0 : rank[i + 1];
		// score 按升序排序，若 score 相等 ，val 按字典序排序
		while (p->level[i].forward && (p->level[i].forward->get_score() < score ||
			(p->level[i].forward->get_score() == score && p->level[i].forward->get_value() < ele)))
		{
			rank[i] += p->level[i].span;
			p = p->level[i].forward;
		}
		update[i] = p;//第 i 层插入位置的前一个节点是 p
	}

	p = p->level[0].forward;
	// 节点 (ele,score) 已存在，无需插入，返回 false;
	if (p && p->get_score() == score && p->get_value() == ele)
		return false;

	//int level = zslRandomLevel();//获得随机层数
	int level = l;
	// 如果 level 大于当前最大层数，则更新 新的层的插入位置前一个节点
	if (level > m_level)
	{
		for (int i = m_level; i < level; ++i)
		{
			//新的层的插入位置的前一个节点是头节点，跨度为 m_length
			rank[i] = 0;
			update[i] = m_head;
			update[i]->level[i].span = m_length;
		}
		m_level = level;//更新最大层数
	}

	SkipListNode* node = zslCreateNode(level, score, ele);
	// 插入该节点
	for (int i = 0; i < level; ++i)
	{
		node->level[i].forward = update[i]->level[i].forward;// 该节点在第 i 层的前进指针指向 update[i] 的前进指针
		update[i]->level[i].forward = node;// update[i] 第 i 层的前进指针指向该节点

		node->level[i].span = update[i]->level[i].span - (rank[0] - rank[i]);
		update[i]->level[i].span = (rank[0] - rank[i]) + 1;
	}

	// 如果新节点的层数小于当时的最大层数，则上层未连接的层的跨度要 +1
	for (int i = level; i < m_level; ++i)
		update[i]->level[i].span++;

	// 更新 新节点的后退指针
	node->backward = update[0] == m_head ? nullptr : update[0];

	// 如果新节点第 1 层的前进指针不为 null,则该指针指向的对象的后退指针需要指向新节点
	if (node->level[0].forward)
		node->level[0].forward->backward = node;
	else
		m_tail = node;//如果未 null,则表示它是最后一个节点，更新尾指针

	++m_length;//节点数量+1

	return true;
}

bool SkipList::zslDelete(const string& ele, double score)
{
	SkipListNode* update[ZSKIPLIST_MAXLEVEL];	// 存储搜索路径

	SkipListNode* p = m_head;
	// 查找结点
	for (int i = m_level - 1; i >= 0; --i)
	{
		while (p->level[i].forward && (p->level[i].forward->get_score() < score ||
			p->level[i].forward->get_score() == score && p->level[i].forward->get_value() < ele))
		{
			p = p->level[i].forward;
		}
		update[i] = p;
	}

	// 上面查找结束后，如果待删除节点存在，那么 p 是待删除节点的前一个节点
	p = p->level[0].forward;
	// 节点分值和节点值相等的话，那 p 就是待删除节点
	if (p && p->get_score() == score && p->get_value() == ele)
	{
		zslDeleteNode(p, update);// 调整跳表的结构
		delete p;				// 释放资源

		return true;
	}

	return false;
}

long SkipList::zslGetRank(const string& ele, double score)
{
	long rank = 0;
	SkipListNode* p = m_head;
	// 查找结点
	for (int i = m_level - 1; i >= 0; --i)
	{
		while (p->level[i].forward && (p->level[i].forward->get_score() < score ||
			p->level[i].forward->get_score() == score && p->level[i].forward->get_value() <= ele))
		{
			rank += p->level[i].span;
			p = p->level[i].forward;
		}

		// p 可能是头节点
		/*if (p->get_score() == score && p->get_value() == ele)
			return rank;*/
		if (p->get_score() == score)
			return rank;
	}

	return 0;
}

bool SkipList::zslUpdateScore(const string& ele, double old_score, double new_score)
{
	SkipListNode* update[ZSKIPLIST_MAXLEVEL];	// 存储搜索路径

	SkipListNode* p = m_head;
	// 查找结点
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
	// 不存在该节点
	if (p == nullptr || p->get_score() != old_score || p->get_value() != ele)
		return false;

	// 如果新的分值还是在原来位置，直接修改分值即可
	if ((p->backward == nullptr || p->backward->get_score() < new_score) &&
		(p->level[0].forward == nullptr || p->level[0].forward->get_score() > new_score))
	{
		p->set_score(new_score);
		return true;
	}

	// 新的分值不在原来位置，则删除原节点，再插入新节点
	zslDeleteNode(p, update);
	zslInsert(ele, new_score, p->level.size());
	delete p;

	return true;
}

SkipListNode* SkipList::zslGetElementByRank(long rank)
{
	// 排名大于节点总数，返回 nullptr
	if (rank > m_length)
		return nullptr;

	long m_rank = 0;
	SkipListNode* p = m_head;
	// 查找结点
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
	// 查找该范围内的第一个元素，若返回 nullptr ，即该范围内不存在元素
	SkipListNode* ret = zslFirstInRange(start, end);

	if (ret == nullptr)
		return false;

	return true;

}

SkipListNode* SkipList::zslFirstInRange(double start, double end)
{
	// 范围内不存在元素，返回 nullptr
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
	// 范围内不存在元素，返回 nullptr
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
	// 查找结点
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
	SkipListNode* update[ZSKIPLIST_MAXLEVEL];	// 存储搜索路径
	SkipListNode* p = m_head;
	// 查找结点
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
		zslDeleteNode(p, update);	// 删除跳表中的节点
		dict.erase(p->get_value());	// 删除字典中的节点
		delete p;					// 释放资源
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

	SkipListNode* update[ZSKIPLIST_MAXLEVEL];	// 存储搜索路径
	SkipListNode* p = m_head;
	// 查找结点
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
		zslDeleteNode(p, update);	// 删除跳表中的节点
		dict.erase(p->get_value());	// 删除字典中的节点
		delete p;					// 释放资源
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
	// 创建输出流
	ofstream ofs;
	// 打开文件
	ofs.open(STORE_FILE_NAME, ios::out);

	cout << "Dumping Data......." << endl;
	SkipListNode* p = m_head->level[0].forward;

	while (p)
	{
		string str = p->get_value() + DELIMITER + to_string(p->get_score());//拼接数据
		ofs << str << endl;//写入硬盘
		p = p->level[0].forward;
	}

	ofs.close();

	cout << "Dump Data Finished." << endl;
}

void SkipList::zslLoadFile()
{
	// 创建输入流
	ifstream ifs;
	// 打开文件
	ifs.open(STORE_FILE_NAME, ios::in);
	cout << "Loadding Data........" << endl;
	if (!ifs.is_open())
	{
		cout << "文件打开失败..." << endl;
		return;
	}

	string line;
	string ele = "";
	double score = 0.0;
	// 每行读取数据
	while (getline(ifs, line))
	{
		if (get_ele_score_from_string(line, ele, score))
			zslInsert(ele, score);
	}

	// 关闭流
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
	// 更新每一层的连接结构
	for (int i = 0; i < m_level; ++i)
	{
		// 第 i 层连接 p
		if (update[i]->level[i].forward == p)
		{
			update[i]->level[i].forward = p->level[i].forward;	// update[i]的第 i 层前进指针指向 p 第 i 层前进指针指向的节点
			update[i]->level[i].span += p->level[i].span - 1;	// update[i] 与 p 第 i 层跨度和减去 1，就是新跨度
		}
		else//第 i 层没连接 p
		{
			update[i]->level[i].span -= 1;//跨度 -1
		}

	}
	if (p->level[0].forward)
		p->level[0].forward->backward = p->backward;	//更新后退指针
	else
		m_tail = p->backward;							// 更新尾指针

	// 如果删除的节点中某些层只有它一个节点，则需要更新 m_level
	while (m_level > 1 && m_head->level[m_level - 1].forward == nullptr)
		--m_level;

	--m_length;		//节点数量减 1
}

bool SkipList::zslQuickCheckRange(double start, double end)
{
	// start > end 返回 false
	if (start > end)
		return false;

	// 跳表中的最大分值小于范围的起点，那跳表中不存在该范围的分值
	if (m_tail == nullptr || m_tail->get_score() < start)
		return false;

	// 跳表中的最小分值大于范围的终点，那跳表中不存在该范围的分值
	SkipListNode* p = m_head->level[0].forward;
	if (p == nullptr || p->get_score() > end)
		return false;

	return true;
}

bool SkipList::get_ele_score_from_string(const string& str, string& ele, double& score)
{
	// 字符串为空，返回 false
	if (str.empty())
		return false;

	// 字符串不存在分隔符或分隔符在字符串末尾，返回 false
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

