#include "utils.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <dirent.h>
#include <random>
#include <vector>
using namespace std;

//返回一个01之间的随机小数
float rand01() { return rand() / (float)RAND_MAX; }

Json::Value get_json_from_file(string fname)
{
	Json::Reader reader;
	ifstream file(fname);
	assert(file.is_open()); //"file is open fail!"
	Json::Value root;
	if (!reader.parse(file, root, false))
	{
		assert(false); //"Json::Reader Parse error!"
	}
	return root;
}

vector<string> get_files(string dir_name)
{
	vector<string> files;
	DIR *dir;

	struct dirent *ent;
	if ((dir = opendir(dir_name.c_str())) != NULL)
	{
		// print all the files
		while ((ent = readdir(dir)) != NULL)
		{
			if (ent->d_type & DT_REG)
			{
				files.push_back(dir_name + "/" + ent->d_name);
				// printf("%s\n", str.c_str());
			}
		}
		closedir(dir);
	}
	else
	{
		/* could not open directory */
		perror("");
	}
	return files;
}