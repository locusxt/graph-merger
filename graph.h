#include <string>
#include <cstring>
#include <vector>

using namespace std;

class SingleGraph;

class Node
{
    public:
        int nid;
        SingleGraph* graph;
        string type;
        string name;

        Node(int id, string tp, string n, SingleGraph* g):nid(id), type(tp), name(n), graph(g){}

};

class Edge
{
    public:
        int eid;
        Node* source;
        Node* target;
        SingleGraph* graph;
        string type;
        string name;

        Edge(int id, Node* s, Node* t, string tp, string n, SingleGraph* g):eid(id), source(s), target(t), type(tp), name(n), graph(g){}

};

class SingleGraph
{
    public:
        bool directed;
        bool typed;
        int gid;
        string name;

        int node_num;
        int edge_num;
        vector<Node> node_list;
        vector<Edge> edge_list;
        
        SingleGraph(bool d, bool t, int id, string n):directed(d), typed(t), gid(id), name(n)
        {
            node_num = edge_num = 0;
        }

        void init_from_json(string fname);//从json文件中载入
};

class MergedGraph;

class Cluster
{
    public:
        int cid;
        MergedGraph* graph;
        vector<Node*> node_list; //融合的点的集合

        Cluster(int id, MergedGraph* g):cid(id), graph(g){}
};

class Link
{
    public:
        int lid; //在link_list中的编号
        MergedGraph* graph;
        Edge* edge; //对应的边
        Cluster* source;//起点的cluster
        Cluster* target;

        Link(int id, MergedGraph* g, Edge* e, Cluster* s, Cluster* t): lid(id), graph(g), source(s), target(t){}
};

class MergedGraph
{
    public:
        // int mid;
        vector<Cluster> cluster_list;
        vector<Link> link_list;

        // MergedGraph(int id):mid(id){}
        int cal_node_x_edge();

        void init(string folder_name);
};