#include "graph.h"
#include <vector>
#include <json/json.h>
#include <assert.h>
#include <fstream>
using namespace std;


Json::Value get_json_from_file(string fname)
{
    Json::Reader reader;
    ifstream file(fname);
    assert(file.is_open());//"file is open fail!"  
    Json::Value root;  
    if  (!reader.parse(file, root, false )) {  
        assert(false);//"Json::Reader Parse error!"
    }
    return root;
}

void SingleGraph::init_from_json(string fname)
{
    Json::Value root = get_json_from_file(fname);
    
    return;
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
    sort(link_list.begin(),link_list.end(), cmp_lk);
    int sz = link_list.size();
    int link_cnt = 1;//只计算不一样的link
    Link* l1, *l2;
    l1 = &(link_list[0]);
    for (int i = 1; i < sz; ++i)
    {
        l2 = &(link_list[i]);
        if(l1->source->cid == l2->source->cid && l1->target->cid == l2->target->cid && l1->edge->type == l2->edge->type)
            continue;
        ++link_cnt;
        l1 = l2;
    }
    return cluster_list.size() * link_cnt;
}