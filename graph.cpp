#include "graph.hpp"
#include <random>
#include <stdexcept>




/* Getters */

unsigned int Graph::vx_count()   const { return vertecies.size(); }
unsigned int Graph::edge_count() const { return edge_num; }


unordered_set<int>::const_iterator Graph::begin() const { return vertecies.begin(); }
unordered_set<int>::const_iterator Graph::end()   const { return vertecies.end();   }


const unordered_map<int, float>& Graph::Neighbors_of(int node) const
{
    if (hasNode(node)) return edges.at(node);

    else throw invalid_argument("node was not found");
}


/* predicates */


bool Graph::hasNode(int node) const
{
    return vertecies.count(node);
}


bool Graph::connected(int src, int dst) const
{
    return hasNode(src) && Neighbors_of(src).count(dst);
}


/* Setters */


void Graph::addNode(int node)
{
    if (vertecies.count(node)) return;

    vertecies.insert(node);
    edges[node] = unordered_map<int, float>();
}


void Graph::removeNode(int node)
{
    if (! hasNode(node)) throw invalid_argument("node not found");
    
    edge_num -= Neighbors_of(node).size();

    for (auto [nei, w] : Neighbors_of(node))
        edges[nei].erase(node);

    vertecies.erase(node);
    edges.erase(node);
}


void Graph::connect(int src, int dst, float weight)
{
    if (! hasNode(src) || ! hasNode(dst))
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


/* Builders */


Graph::Graph(unsigned int size)
{
    for (unsigned int node = 0; node < size; this -> addNode(node++));
}


Graph RandomGraph(int n, float p)
{
    Graph graph(n);

    random_device seeder;
    mt19937 PRG(seeder());
    uniform_real_distribution distr(0., 1.);

    for (int src : graph)
        for (int dst = src+1; dst < n; dst++)
            if (distr(PRG) < p)
                graph.connect(src, dst, distr(PRG));

    return graph;
}