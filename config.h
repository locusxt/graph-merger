#define MAXN 100000000
#include <string>
using namespace std;

class Config
{
  public:
	bool directed;
	bool typed;

	int individual_num;
	int evolve_limit;
	float mutate_ratio;
	float crossover_ratio;
	float output_frequency;

	string graph_dir;

	Config(bool b, bool t, int idvn = 200, int evl = MAXN, float m_rat = 0.1,
		   float c_rat = 1.0, float frq = 0.1)
	{
		directed = b, typed = t;
		individual_num = idvn, evolve_limit = evl;
		mutate_ratio = m_rat, crossover_ratio = c_rat, output_frequency = frq;
		graph_dir;
	}

	void parse_cmd();
};