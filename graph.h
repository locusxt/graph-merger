#include <cstring>
#include <map>
#include <string>
#include <vector>

using namespace std;

class SingleGraph;

class Node
{
public:
	int nid;
	SingleGraph *graph_p;
	string type;
	string name;

	Node(int id, string tp, string n, SingleGraph *g)
			: nid(id), type(tp), name(n), graph_p(g)
	{
	}
};

class Edge
{
public:
	int eid;
	Node *source_p;
	Node *target_p;
	SingleGraph *graph_p;
	string type;
	string name;

	Edge(int id, Node *s, Node *t, string tp, string n, SingleGraph *g)
			: eid(id), source_p(s), target_p(t), type(tp), name(n), graph_p(g)
	{
	}
};

//表示单个图
class SingleGraph
{
public:
	bool directed;
	bool typed;
	int gid;
	string gname;

	vector<Node> node_list;
	vector<Edge> edge_list;

	SingleGraph(){}; // do nothing

	SingleGraph(bool d, bool t, int id, string n)
			: directed(d), typed(t), gid(id), gname(n)
	{
		// node_num = edge_num = 0;
	}

	void init_from_json(string fname); //从json文件中载入
};

class MergedGraph;

class Cluster
{
public:
	int cid;
	string type;
	MergedGraph *graph_p;
	vector<Node *> node_list; //融合的点的集合

	Cluster() {}
	Cluster(string tp) : type(tp) {}
	Cluster(int id, MergedGraph *g) : cid(id), graph_p(g) {}
};

class Link
{
public:
	int lid; //在link_list中的编号
	MergedGraph *graph_p;
	Edge *edge_p;			 //对应的边
	Cluster *source_p; //起点的cluster
	Cluster *target_p;

	Link(MergedGraph *g, Edge *e, Cluster *s, Cluster *t)
			: graph_p(g), edge_p(e), source_p(s), target_p(t)
	{
	}

	Link(int id, MergedGraph *g, Edge *e, Cluster *s, Cluster *t)
			: lid(id), graph_p(g), edge_p(e), source_p(s), target_p(t)
	{
	}
};

//融合图
class MergedGraph
{
public:
	// int mid;
	vector<Cluster> cluster_list;
	vector<Link> link_list;
	map<string, int> type_cnt;	 //各type节点的个数
	map<string, int> type_start; //各type，在cluster_list中开始的位置

	// MergedGraph(int id):mid(id){}
	int cal_node_x_edge();

	void init(vector<SingleGraph> *graphs_p);
};