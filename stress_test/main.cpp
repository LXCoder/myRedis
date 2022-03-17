// SkipListTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "SkipList.h"
#include <chrono>
#define DATA_NUMS 1000000
#define UNIT 10000
#define STR_LEN 10
#define MAX_SCORE 10000
using namespace std;

string getRandomString(int len)
{
	static string str = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ0123456789";
	int n = str.size();
	string ans = "";
	for (int i = 0; i < len; ++i)
	{
		ans.push_back(str[rand() % n]);
	}

	return ans;
}

void generateTestData()
{
	// 创建输出流
	ofstream ofs;
	// 打开文件
	ofs.open("data/test_data_50.txt", ios::out);


	for (int i = 0; i < DATA_NUMS; ++i)
	{
		ofs << getRandomString((rand() % STR_LEN) + 1) + DELIMITER + (to_string(rand() % MAX_SCORE)) << endl;
	}

	ofs.close();

	cout << "finished" << endl;
}

typedef struct Node {
public:
	
	Node() = default;
	Node(string _ele,double _score):ele(_ele),score(_score){}

	string ele;
	double score;
}Node;

bool get_ele_score_from_string(const string& str, string& ele, double& score)
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

int main(int argc,char *argv[])
{
    if(argc < 2)
    {
        cout<<"请输入测试的数目（W）：10  50  100"<<endl;
        return -1;
    }

    int n = stoi(argv[1]);

    string dataFile = "data/test_data_";
    dataFile.append(argv[1]);
    dataFile.append(".txt");
    ifstream ifs;


	ifs.open(dataFile, ios::in);

	if (!ifs.is_open())
	{
		cout << "文件打开失败..." << endl;
		return -1;
	}

	string line;
	string ele = "";
	double score = 0.0;
	vector<Node> arr(n*UNIT);
	int idx = 0;
	// 每行读取数据
	while (getline(ifs, line))
	{
		if (get_ele_score_from_string(line, ele, score))
		{
			arr[idx++] = Node(ele, score);
		}
	}

	// 关闭流
	ifs.close();

    cout << "开始插入" << endl;
	SkipList skipList;
	int i = 0;
	auto start = chrono::high_resolution_clock::now();
	for (int i = 0,len = n * UNIT; i < len; ++i)
	{
		skipList.zslInsert(arr[i].ele, arr[i].score);
	}

	auto finish = chrono::high_resolution_clock::now();
	chrono::duration<double> elapsed = finish - start;
	cout << "insert " << n*UNIT <<" nodes,elapsed:" << elapsed.count() << endl;

	start = chrono::high_resolution_clock::now();

	for (int i = 0,len = n*UNIT; i < len; ++i)
	{
		skipList.zslGetRank("", rand() % MAX_SCORE);
	}
	finish = chrono::high_resolution_clock::now();
	elapsed = finish - start;
	cout << "search " << n*UNIT << " nodes,elapsed:" << elapsed.count() << endl;

    return 0;

}
