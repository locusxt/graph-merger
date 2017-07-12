#include "config.h"
#include "graph.h"
#include <vector>
using namespace std;

class Individual
{
  public:
	MergedGraph mg;
	float fitness;
	float ratio;

	void init(vector<SingleGraph> *graphs_p); //初代
	void cal_fitness();						  //计算适应度
	Individual crossover(Individual *idv_p);  //交叉
	void mutate();							  //变异
	void print_info();
};

class Population
{

  public:
	Individual best_idv;
	int evolve_cnt;
	vector<SingleGraph> graphs;
	vector<Individual> *idv_list_p;
	Config *cfg_p;

	Population(Config *cfg_p);
	void init();
	void fitness_step();
	void ratio_step();
	void crossover_step();
	void mutate_step();
	bool check_limit();
	int select();
	void evolve();
	void sort_idv_list(vector<Individual> *l_p);
	void print_info();
};