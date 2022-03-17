#include "ZSet.h"

ZSet::ZSet()
{
	m_zsl = new SkipList();
}

ZSet::~ZSet()
{
	delete m_zsl;
}

bool ZSet::zadd(const string & ele, double score)
{
	// 如果 ele 存在
	if (m_dict.count(ele) == 1)
	{
		// 如果分值不同，就把 score 当作新分值
		if (score != m_dict[ele])
		{
			m_zsl->zslUpdateScore(ele, m_dict[ele], score);
			m_dict[ele] = score;
		}
		return false;
	}

	if (m_zsl->zslInsert(ele, score))
	{
		m_dict.insert(make_pair(ele, score));
		return true;
	}

	return false;
}

long ZSet::zcard()
{
	return m_zsl->zslSize();
}

long ZSet::zcount(double min, double max)
{
	return m_zsl->zslCountRangByScore(min,max);
}

bool ZSet::zincrby(const string & ele, double increment)
{
	if (m_dict.count(ele) == 0)
	{
		if (!m_zsl->zslInsert(ele, increment))
			return false;

		m_dict.insert(make_pair(ele, increment));
	}
	else
	{
		if (!m_zsl->zslUpdateScore(ele, m_dict[ele], m_dict[ele] + increment))
			return false;

		m_dict[ele] += increment;
	}
	return true;
}

vector<SkipListNode*> ZSet::zrange(long start, long stop)
{
	if (stop == -1)
		stop = m_zsl->zslSize()-1;
	vector<SkipListNode*> arr;
	m_zsl->zslRangeByRank(start+1, stop+1, arr);
	return arr;
}

vector<SkipListNode*> ZSet::zrevRange(long start, long stop)
{
	if (stop == -1)
		stop = m_zsl->zslSize()-1;

	vector<SkipListNode*> arr;
	m_zsl->zslRevRangeByRank(start+1, stop+1, arr);
	return arr;
}

vector<SkipListNode*> ZSet::zrangeByScore(double min, double max)
{
	vector<SkipListNode*> arr;
	m_zsl->zslRangeByScore(min, max, arr);
	return arr;
}

vector<SkipListNode*> ZSet::zrevRangeByScore(double min, double max)
{
	vector<SkipListNode*> arr;
	m_zsl->zslRangeByScore(min, max, arr);
	reverse(arr.begin(), arr.end());
	return arr;
}

long ZSet::zrank(const string & ele)
{
	return m_zsl->zslGetRank(ele,m_dict[ele]);
}

long ZSet::zrevRank(const string & ele)
{
	return m_zsl->zslSize() - m_zsl->zslGetRank(ele, m_dict[ele]) + 1;
}

bool ZSet::zrem(const string& ele)
{
	if (m_zsl->zslDelete(ele, m_dict[ele]))
	{
		m_dict.erase(ele);
		return true;
	}
	return false;
}

long ZSet::zremRangeByRank(long start, long stop)
{
	if (stop == -1)
		stop = m_zsl->zslSize()-1;
	return m_zsl->zslDeleteRangeByRank(start+1, stop+1, m_dict);
}

long ZSet::zremRangeByScore(double min, double max)
{
	return m_zsl->zslDeleteRangeByScore(min, max, m_dict);
}

double ZSet::zscore(const string & ele)
{
	if (m_dict.count(ele) == 0)
		return -1;
	return m_dict[ele];
}

vector<string> ZSet::zrwRange(int splitCount)
{
	vector<string> arr;
	vector<SkipListNode*> list = zrange(0, -1);
	int n = list.size();
	string str = "";
	for (int i = 0;i < n;++i)
	{
		if (i%splitCount == 0)
		{
			if (!str.empty())
			{
				arr.push_back(str);
				str.clear();
			}
		}

		str.append(to_string(list[i]->get_score()));
		str.push_back(' ');
		str.append(list[i]->get_value());
		str.push_back(' ');
	}

	if (!str.empty())
		arr.push_back(str);

	return arr;
}

void ZSet::zslDisplay()
{
	m_zsl->displaySkipList();
}
