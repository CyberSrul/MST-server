#include "mst.hpp"
#include <stdexcept>
#include <functional>
#include <set>
#include <queue>




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


Graph prim(const Graph& graph)
{
    Graph tree = Graph(0);
    int src = * graph.begin();
    tree.addNode(src);

    priority_queue<Edge> heap;

    while (tree.edge_count() < graph.vx_count() - 1)
    {
        for (auto [dst, w] : graph.Neighbors_of(src))
            if (! tree.hasNode(dst))
                heap.push(Edge(src, dst, -w));

        Edge e = heap.top();
        while (tree.hasNode(e.dst)) { heap.pop(); e = heap.top(); }

        tree.addNode(e.dst);
        tree.connect(e.src, e.dst, -e.weight);

        src = e.dst;
    }

    return tree;
}


unordered_map<string, function<Graph(const Graph&)>> algos =
{
    {"kruskal", kruskal},
    {"prim", prim}
};


Graph MST(const string& algo, const Graph& graph)
{
    if (algos.count(algo))
        return algos[algo](graph);

   throw invalid_argument("algo is not supported");
}