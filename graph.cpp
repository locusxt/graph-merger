#include "graph.h"
#include "utils.h"
#include <assert.h>
#include <cstring>
#include <fstream>
#include <json/json.h>
#include <map>
#include <string>
#include <vector>
using namespace std;

void SingleGraph::init_from_json(string fname)
{
	node_list.clear();
	edge_list.clear();

	Json::Value root = get_json_from_file(fname);
	Json::Value nodes = root["nodes"];
	int node_sz = nodes.size();
	for (int i = 0; i < node_sz; ++i)
	{
		int nid = nodes[i]["nid"].asInt();
		string type = nodes[i]["type"].asString();
		string name = nodes[i]["name"].asString();

		node_list.push_back(Node(nid, type, name, this));
	}

	Json::Value edges = root["edges"];
	int edge_sz = edges.size();
	for (int i = 0; i < edge_sz; ++i)
	{
		int eid = edges[i]["eid"].asInt();
		string type = edges[i]["type"].asString();
		string name = edges[i]["name"].asString();
		int sid = edges[i]["source"].asInt();
		int tid = edges[i]["target"].asInt();

		edge_list.push_back(
			Edge(eid, &(node_list[sid]), &(node_list[tid]), type, name, this));
	}

	directed = root["directed"].asBool();
	typed = root["typed"].asBool();
	gname = root["gname"].asString();
	gid = root["gid"].asInt();
	return;
}

//依据SingleGraph数组，初始化
void MergedGraph::init(vector<SingleGraph> *graphs_p)
{
	int g_sz = graphs_p->size();
	map<string, int>::iterator it;
	cluster_list.clear();

	//统计各type的节点的个数
	for (int i = 0; i < g_sz; ++i)
	{
		vector<Node> *node_list_p = &((*graphs_p)[i].node_list);
		int n_sz = node_list_p->size();
		for (int j = 0; j < n_sz; ++j)
		{
			string type = (*node_list_p)[j].type;
			it = type_cnt.find(type);
			if (it != type_cnt.end())
			{
				int tmp = it->second;
				type_cnt[type] = tmp + 1;
			}
			else
				type_cnt[type] = 1;
		}
	}

	//为cluster_list 开辟空间
	it = type_cnt.begin();
	int pos = 0;
	while (it != type_cnt.end())
	{
		cluster_list.insert(cluster_list.end(), it->second, Cluster(it->first));
		type_start[it->first] = pos;
		pos += it->second;
		it++;
	}

	//将node 随机分配到cluster
	int c_sz = cluster_list.size();
	for (int i = 0; i < g_sz; ++i)
	{
		int *occupy_p = new int(c_sz);
		memset(occupy_p, 0, sizeof(occupy_p));
		vector<Node> *node_list_p = &((*graphs_p)[i].node_list);
		int n_sz = node_list_p->size();
		for (int j = 0; j < n_sz; ++j)
		{
			string type = (*node_list_p)[j].type;
			int cnt = type_cnt[type];
			int start = type_start[type];

			while (true)
			{
				int tmp = rand0n(cnt);
				if (occupy_p[start + tmp])
					continue;
				occupy_p[start + tmp] = j + 1;
				Cluster *c_p = &(cluster_list[start + tmp]);
				c_p->node_list.push_back(&((*node_list_p)[j]));
			}
		}
		//根据occupy_p，初始化link_list
	}
}

//用于link_list的排序
bool cmp_lk(Link const &l1, Link const &l2)
{
	if (l1.source_p->cid != l2.source_p->cid)
		return l1.source_p->cid < l2.source_p->cid;
	else if (l1.target_p->cid != l2.target_p->cid)
		return l1.target_p->cid < l2.target_p->cid;
	else
		return l1.edge_p->type < l2.edge_p->type;
}

int MergedGraph::cal_node_x_edge()
{
	//需要先对link_list排序
	sort(link_list.begin(), link_list.end(), cmp_lk);
	int sz = link_list.size();
	int link_cnt = 1; //只计算不一样的link
	Link *l1_p, *l2_p;
	l1_p = &(link_list[0]);
	for (int i = 1; i < sz; ++i)
	{
		l2_p = &(link_list[i]);
		if (l1_p->source_p->cid == l2_p->source_p->cid &&
			l1_p->target_p->cid == l2_p->target_p->cid &&
			l1_p->edge_p->type == l2_p->edge_p->type)
			continue;
		++link_cnt;
		l1_p = l2_p;
	}
	return cluster_list.size() * link_cnt;
}