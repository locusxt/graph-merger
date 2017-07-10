#include "genetic.h"
#include "utils.h"
#include <algorithm>
#include <iostream>
#include <vector>
#define INF 10000000
using namespace std;

void Individual::init(vector<SingleGraph> *graphs)
{
	mg.init(graphs);
	return;
}

void Individual::cal_fitness()
{
	//最基本的点乘边
	fitness = mg.cal_node_x_edge();
	return;
}

Individual Individual::crossover(Individual *idv) { return Individual(); }

void Individual::mutate()
{
	// do nothing
	return;
}

void Individual::print_info() { cout << fitness << endl; }

Population::Population(Config *conf)
{
	int sz = conf->individual_num;
	this->cfg = conf;
	idv_list = new vector<Individual>;
	this->evolve_cnt = 0;
	for (int i = 0; i < sz; ++i)
		idv_list->push_back(Individual());
}

void Population::init()
{
	int i_sz = cfg->individual_num;
	string g_dir = cfg->graph_dir;

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
		(*idv_list)[i].init(&graphs);
	}
}

//???
bool cmp_idv(Individual const &a, Individual const &b)
{
	return a.fitness > b.fitness;
}

void Population::sort_idv_list(vector<Individual> *l)
{
	sort(l->begin(), l->end(), cmp_idv);
}

void Population::fitness_step()
{
	int sz = cfg->individual_num;
	for (int i = 0; i < sz; ++i)
		(*idv_list)[i].cal_fitness();
	// //目前ratio按fitness所占比例分配
	// for (int i = 0; i < sz; ++i)
	//     (*idv_list)[i].ratio = (*idv_list)[i].fitness / fitness_sum;

	sort_idv_list(idv_list);
	// sort(idv_list.begin(), idv_list.end(), cmp_idv);
	best_idv = (*idv_list)[0];
	return;
}

void Population::ratio_step()
{
	int sz = cfg->individual_num;
	float fitness_sum = 0.0;
	for (int i = 0; i < sz; ++i)
		fitness_sum += (*idv_list)[i].fitness;
	for (int i = 0; i < sz; ++i)
		(*idv_list)[i].ratio = (*idv_list)[i].fitness / fitness_sum;
}

int Population::select()
{
	int sz = cfg->individual_num;
	float rand_num = rand01();
	float sum_num = 0.0;
	for (int i = 0; i < sz; ++i)
	{
		sum_num += (*idv_list)[i].fitness;
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
	int sz = cfg->individual_num;
	vector<Individual> *new_idv_list = new vector<Individual>;
	// new_idv_list->push_back((*idv_list)[0]);//保存最优个体
	for (int i = 0; i < sz; ++i)
	{
		Individual *a = &(*idv_list)[select()];
		Individual *b = &(*idv_list)[select()];
		new_idv_list->push_back(a->crossover(b));
	}
	// sort_idv_list(&new_idv_list);
	delete (idv_list);
	idv_list = new_idv_list; //更新种群
}

void Population::mutate_step()
{
	int sz = cfg->individual_num;
	//最优个体不变异
	for (int i = 0; i < sz; ++i)
	{
		if (rand01() < cfg->mutate_ratio)
			(*idv_list)[i].mutate();
	}
	return;
}

bool Population::check_limit() { return evolve_cnt < cfg->evolve_limit; }

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

		idv_list->push_back(best_idv);
		fitness_step();
		idv_list->pop_back();
		// print info
	}
}