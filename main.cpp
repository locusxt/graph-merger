#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include "utils.h"
#include "graph.h"

using namespace std;

int main()
{
    //srand 执行一次
    srand(time(0));
    cout << rand01() << endl;
    cout << rand01() << endl;
    return 0;
}