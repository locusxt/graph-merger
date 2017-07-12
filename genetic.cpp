#include "genetic.h"
#include "utils.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <iostream>
#include <vector>
#define INF 10000000
#define MAXN 100
using namespace std;

void Individual::init(vector<SingleGraph> *graphs_p)
{
	mg.init(graphs_p);
	return;
}

void Individual::cal_fitness()
{
	//最基本的点乘边
	fitness = mg.cal_node_x_edge();
	return;
}

const int size = MAXN;
bool mymap[size][size];		 // 二分图的相等子图, mymap[i][j] = true 代表Xi与Yj有边
bool xckd[size], yckd[size]; // 标记在一次DFS中，Xi与Yi是否在交错树上
int match[size];			 // 保存匹配信息，其中i为Y中的顶点标号，match[i]为X中顶点标号

bool DFS(int, const int);

void KM_Perfect_Match(const int n, const int edge[][size])
{
	int i, j;
	int lx[size], ly[size]; // KM算法中Xi与Yi的标号
	for (i = 0; i < n; i++)
	{
		lx[i] = -INF;
		ly[i] = 0;
		for (j = 0; j < n; j++)
			lx[i] = max(lx[i], edge[i][j]);
	}
	bool perfect = false;
	while (!perfect)
	{
		// 初始化邻接矩阵
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				if (lx[i] + ly[j] == edge[i][j])
					mymap[i][j] = true;
				else
					mymap[i][j] = false;
			}
		}
		// 匹配过程
		int live = 0;
		memset(match, -1, sizeof(match));
		for (i = 0; i < n; i++)
		{
			memset(xckd, false, sizeof(xckd));
			memset(yckd, false, sizeof(yckd));
			if (DFS(i, n))
				live++;
			else
			{
				xckd[i] = true;
				break;
			}
		}
		if (live == n)
			perfect = true;
		else
		{
			// 修改标号过程
			int ex = INF;
			for (i = 0; i < n; i++)
			{
				for (j = 0; xckd[i] && j < n; j++)
				{
					if (!yckd[j])
						ex = min(ex, lx[i] + ly[j] - edge[i][j]);
				}
			}
			for (i = 0; i < n; i++)
			{
				if (xckd[i])
					lx[i] -= ex;
				if (yckd[i])
					ly[i] += ex;
			}
		}
	}
}

// 寻找是否有以Xp为起点的增广路径，返回值为是否含有增广路
bool DFS(int p, const int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		if (!yckd[i] && mymap[p][i])
		{
			yckd[i] = true;
			int t = match[i];
			match[i] = p;
			if (t == -1 || DFS(t, n))
				return true;
			match[i] = t;
			if (t != -1)
				xckd[t] = true;
		}
	}
	return false;
}

Individual Individual::crossover(Individual *idv_p)
{
	int n = mg.cluster_list.size();
	int edge[size][size]; // 连接Xi与Yj的边的权值
	memset(edge, 0, sizeof(edge));

	for (int i = 0; i < n; ++i)
	{
		string type = mg.cluster_list[i].type;
		int start = idv_p->mg.type_start[type]; // idv中该type的开始位置
		int cnt = idv_p->mg.type_cnt[type];

		for (int j = 0; j < cnt; ++j)
		{
			int tmp = start + j;
			int sz1 = mg.cluster_list[i].node_list.size();
			int sz2 = idv_p->mg.cluster_list[j].node_list.size();
			int same_cnt = 0;
			for (int k = 0; k < sz1; ++k)
			{
				for (int l = 0; l < sz2; ++l)
				{
					if (mg.cluster_list[i].node_list[k] ==
						idv_p->mg.cluster_list[j].node_list[l])
						++same_cnt;
				}
			}
			edge[i][j] = same_cnt;
		}
	}

	// 读取二分图每两点间边的权并保存在edge[][]中,若X与Y数目不等，应添加配合的顶点
	// 保存二分图中X与Y的顶点数n,若上一步不等应保存添加顶点完毕后的n
	KM_Perfect_Match(n, edge);
	int cost = 0; // cost 为最大匹配的总和, match[]中保存匹配信息
	for (int i = 0; i < n; i++)
		cost += edge[match[i]][i];
	return Individual();
}

void Individual::mutate()
{
	MergedGraph *merged_graph_p = &mg;
	int c_sz = merged_graph_p->cluster_list.size();
	Cluster *c_p = &(merged_graph_p->cluster_list[rand0n(c_sz)]);
	int n_sz = c_p->node_list.size();

	int pos = rand0n(n_sz);
	Node *n_p = c_p->node_list[pos];
	c_p->node_list.erase(c_p->node_list.begin() + pos);
	// links

	string type = c_p->type; // check
	int tp_cnt = merged_graph_p->type_cnt[type];
	int tp_start = merged_graph_p->type_start[type];

	Cluster *new_c_p = &(merged_graph_p->cluster_list[tp_start + rand0n(tp_cnt)]);
	new_c_p->node_list.push_back(n_p);

	return;
}

void Individual::print_info() { cout << fitness << endl; }

Population::Population(Config *conf_p)
{
	int sz = conf_p->individual_num;
	this->cfg_p = conf_p;
	idv_list_p = new vector<Individual>;
	this->evolve_cnt = 0;
	for (int i = 0; i < sz; ++i)
		idv_list_p->push_back(Individual());
}

void Population::init()
{
	int i_sz = cfg_p->individual_num;
	string g_dir = cfg_p->graph_dir;

	//初始化SingleGraph
	graphs.clear();
	vector<string> fname = get_files(g_dir);
	int g_sz = fname.size();
	for (int i = 0; i < g_sz; ++i)
	{
		SingleGraph sg;
		sg.init_from_json(fname[i]);
		graphs.push_back(sg);
	}

	for (int i = 0; i < i_sz; ++i)
	{
		(*idv_list_p)[i].init(&graphs);
	}
}

//???
bool cmp_idv(Individual const &a, Individual const &b)
{
	return a.fitness > b.fitness;
}

void Population::sort_idv_list(vector<Individual> *l_p)
{
	sort(l_p->begin(), l_p->end(), cmp_idv);
}

void Population::fitness_step()
{
	int sz = cfg_p->individual_num;
	for (int i = 0; i < sz; ++i)
		(*idv_list_p)[i].cal_fitness();
	// //目前ratio按fitness所占比例分配
	// for (int i = 0; i < sz; ++i)
	//     (*idv_list)[i].ratio = (*idv_list)[i].fitness / fitness_sum;

	sort_idv_list(idv_list_p);
	// sort(idv_list.begin(), idv_list.end(), cmp_idv);
	best_idv = (*idv_list_p)[0];
	return;
}

void Population::ratio_step()
{
	int sz = cfg_p->individual_num;
	float fitness_sum = 0.0;
	for (int i = 0; i < sz; ++i)
		fitness_sum += (*idv_list_p)[i].fitness;
	for (int i = 0; i < sz; ++i)
		(*idv_list_p)[i].ratio = (*idv_list_p)[i].fitness / fitness_sum;
}

int Population::select()
{
	int sz = cfg_p->individual_num;
	float rand_num = rand01();
	float sum_num = 0.0;
	for (int i = 0; i < sz; ++i)
	{
		sum_num += (*idv_list_p)[i].fitness;
		if (sum_num >= rand_num)
		{
			return i;
			break;
		}
	}
	return sz - 1;
}

void Population::crossover_step()
{
	int sz = cfg_p->individual_num;
	vector<Individual> *new_idv_list_p = new vector<Individual>;
	// new_idv_list->push_back((*idv_list)[0]);//保存最优个体
	for (int i = 0; i < sz; ++i)
	{
		Individual *a_p = &(*idv_list_p)[select()];
		Individual *b_p = &(*idv_list_p)[select()];
		new_idv_list_p->push_back(a_p->crossover(b_p));
	}
	// sort_idv_list(&new_idv_list);
	delete (idv_list_p);
	idv_list_p = new_idv_list_p; //更新种群
}

void Population::mutate_step()
{
	int sz = cfg_p->individual_num;
	//最优个体不变异
	for (int i = 0; i < sz; ++i)
	{
		if (rand01() < cfg_p->mutate_ratio)
			(*idv_list_p)[i].mutate();
	}
	return;
}

bool Population::check_limit() { return evolve_cnt < cfg_p->evolve_limit; }

void Population::evolve()
{
	init(); //初代种群
	fitness_step();
	// print info
	while (check_limit())
	{
		++evolve_cnt;
		ratio_step();
		crossover_step();
		mutate_step();

		idv_list_p->push_back(best_idv);
		fitness_step();
		idv_list_p->pop_back();
		// print info
	}
}