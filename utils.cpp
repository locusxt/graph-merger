#include <cstdlib>
#include <ctime>
#include <random>
#include "utils.h"
using namespace std;

//返回一个01之间的随机小数
float rand01()
{
    return rand() / (float)RAND_MAX;
}