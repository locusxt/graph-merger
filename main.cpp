#include "graph.h"
#include "utils.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

int main()
{
	// srand 执行一次
	srand(time(0));
	cout << rand01() << endl;
	cout << rand01() << endl;
	vector<string> files =
	    get_files("/Users/zhuting/Projects/graph-merger");
	int sz = files.size();
	for (int i = 0; i < sz; ++i)
	{
		cout << files[i] << endl;
	}
	return 0;
}