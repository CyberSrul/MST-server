#include "mst.hpp"
#include <stdexcept>
#include <functional>
#include <set>




class Edge
{
public:

    int src, dst;
    float weight;

    Edge(int s, int d, float w) : src(s), dst(d), weight(w) {}

    bool operator<(const Edge& other) const
    {
        return weight < other.weight;
    }
};


Graph kruskal(const Graph& graph)
{
    set<Edge> sorted_edges;
    for (int src : graph)
        for (auto [dst, w] : graph.Neighbors_of(src))
            sorted_edges.insert(Edge(src, dst, w));

    Graph forest = Graph(0);

    for (Edge e : sorted_edges)
        // do not close a cycle
        if (! forest.hasNode(e.src) || ! forest.hasNode(e.dst))
        {
            forest.addNode(e.src); forest.addNode(e.dst);

            forest.connect(e.src, e.dst, e.weight);

            if (forest.edge_count() >= graph.vx_count()) break;
        }

    return forest;
}


Graph prim(const Graph& graph){return graph;};


unordered_map<string, function<Graph(const Graph&)>> algos =
{
    {"kruskal", kruskal},
    {"prim", prim}
};


Graph MST(const string& algo, const Graph& graph)
{
    if (algos.count(algo))
        return algos[algo](graph);

   throw invalid_argument("supports kruskal and prim algorithms only");
}