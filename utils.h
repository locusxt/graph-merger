#include <assert.h>
#include <cstdio>
#include <fstream>
#include <json/json.h>
#include <vector>
using namespace std;

float rand01();

Json::Value get_json_from_file(string fname);

vector<string> get_files(string dir_name);