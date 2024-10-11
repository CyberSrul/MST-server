#include "graph.hpp"
#include <random>
#include <stdexcept>




unsigned int Graph::vx_count()   { return vertecies.size(); }
unsigned int Graph::edge_count() { return edge_num;     }


bool Graph::connected(int src, int dst)
{
    return vertecies.count(src) && edges[src].count(dst);
}


void Graph::connect(int src, int dst, float weight)
{
    if (! vertecies.count(src) || ! vertecies.count(dst))
    {
        throw invalid_argument("src or dst were not found");
    }

    if (src == dst) throw invalid_argument("a node can not point to itslef");

    if (! connected(src, dst))
    {
        edges[src][dst] = weight;
        edges[dst][src] = weight;
        edge_num++;
    }
}


void Graph::disconnect(int src, int dst)
{
    if (connected(src, dst))
    {
        edges[src].erase(dst);
        edges[dst].erase(src);
        edge_num--;
    }
}


void Graph::addNode(int node)
{
    if (vertecies.count(node)) return;

    vertecies.insert(node);
    edges[node] = unordered_map<int, float>();
}


void Graph::removeNode(int node)
{
    if (! vertecies.count(node)) throw invalid_argument("node not found");
    
    edge_num -= Neighbors_of(node).size();

    for (auto [nei, w] : Neighbors_of(node))
        edges[nei].erase(node);

    vertecies.erase(node);
    edges.erase(node);
}


unordered_set<int>::const_iterator Graph::begin() const { return vertecies.begin(); }
unordered_set<int>::const_iterator Graph::end()   const { return vertecies.end();   }


const unordered_map<int, float>& Graph::Neighbors_of(int node) const
{
    if (edges.count(node)) return edges.at(node);

    else throw invalid_argument("node was not found");
}


Graph RandomGraph(int n, float p, int s)
{
    Graph graph;

    for (int node = 0; node < n; graph.addNode(node++));

    mt19937 PRG(s);
    uniform_real_distribution distr(0., 1.);

    for (int src : graph)
        for (int dst = src+1; dst < n; dst++)
            if (distr(PRG) < p)
                graph.connect(src, dst, distr(PRG));

    return graph;
}