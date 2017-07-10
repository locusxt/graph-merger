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
void MergedGraph::init(vector<SingleGraph> *graphs)
{
	int g_sz = graphs->size();
	map<string, int>::iterator it;
	cluster_list.clear();

	//统计各type的节点的个数
	for (int i = 0; i < g_sz; ++i)
	{
		vector<Node> *node_list = &((*graphs)[i].node_list);
		int n_sz = node_list->size();
		for (int j = 0; j < n_sz; ++j)
		{
			string type = (*node_list)[j].type;
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
		int *occupy = new int(c_sz);
		memset(occupy, 0, sizeof(occupy));
		vector<Node> *node_list = &((*graphs)[i].node_list);
		int n_sz = node_list->size();
		for (int j = 0; j < n_sz; ++j)
		{
			string type = (*node_list)[j].type;
			int cnt = type_cnt[type];
			int start = type_start[type];

			while (true)
			{
				int tmp = rand0n(cnt);
				if (occupy[start + tmp])
					continue;
				occupy[start + tmp] = j + 1;
				Cluster *c = &(cluster_list[start + tmp]);
				c->node_list.push_back(&((*node_list)[j]));
			}
		}
		//根据occupy，初始化link_list
	}
}

//用于link_list的排序
bool cmp_lk(Link const &l1, Link const &l2)
{
	if (l1.source->cid != l2.source->cid)
		return l1.source->cid < l2.source->cid;
	else if (l1.target->cid != l2.target->cid)
		return l1.target->cid < l2.target->cid;
	else
		return l1.edge->type < l2.edge->type;
}

int MergedGraph::cal_node_x_edge()
{
	//需要先对link_list排序
	sort(link_list.begin(), link_list.end(), cmp_lk);
	int sz = link_list.size();
	int link_cnt = 1; //只计算不一样的link
	Link *l1, *l2;
	l1 = &(link_list[0]);
	for (int i = 1; i < sz; ++i)
	{
		l2 = &(link_list[i]);
		if (l1->source->cid == l2->source->cid &&
			l1->target->cid == l2->target->cid &&
			l1->edge->type == l2->edge->type)
			continue;
		++link_cnt;
		l1 = l2;
	}
	return cluster_list.size() * link_cnt;
}