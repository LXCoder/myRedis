#include "SkipListNode.h"


SkipListNode::SkipListNode(int levels, const string& v, double score) : level(levels), m_val(v), m_score(score),backward(nullptr)
{
}

SkipListNode::~SkipListNode()
{
}

string SkipListNode::get_value()
{
	return m_val;
}

double SkipListNode::get_score()
{
	return m_score;
}

void SkipListNode::set_value(string v)
{
	m_val = v;
}

void SkipListNode::set_score(double score)
{
	m_score = score;
}
