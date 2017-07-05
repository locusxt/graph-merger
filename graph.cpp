#include "graph.h"
#include <vector>
using namespace std;


void SingleGraph::init_from_json(string fname)
{
    //do nothing
    return;
}

//用于link_list的排序
bool cmp_lk(Link* const l1, Link* const l2)
{
    if (l1->source->cid != l2->source->cid)
        return l1->source->cid < l2->source->cid;
    else if (l1->target->cid != l2->target->cid)
        return l1->target->cid < l2->target->cid;
    else
        return l1->edge->type < l2->edge->type;
}

int MergedGraph::cal_node_x_edge()
{
    //需要先对link_list排序
    return cluster_list.size() * link_list.size();
}