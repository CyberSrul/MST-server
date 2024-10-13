#include "mst.hpp"
#include <stdexcept>
#include <functional>
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


class UnionFind
{
public:

    UnionFind(const Graph& graph)
    {
        for (int node : graph)
        {
            parents[node] = node;
            ranks[node] = 0;
        }
    }


    int find(int node)
    {
        if (node != parents[node])
            // path compression
            parents[node] = find(parents[node]);

        return parents[node];
    }


    void merge(int v, int u)
    {
        int rootv = find(v), rootu = find(u);

        if (rootu == rootv) return;

        ranks[rootv] > ranks[rootu] ? parents[rootu] = rootv : parents[rootv] = rootu;
       
        if (ranks[rootv] == ranks[rootu]) ranks[rootu]++;
    }


private:

    unordered_map<int, int> parents;
    unordered_map<int, int> ranks;
};


Graph kruskal(const Graph& graph)
{
    Graph forest = Graph(0);
    UnionFind trees = UnionFind(graph);
    vector<Edge> edges;

    // initiating and sorting edges
    edges.reserve(graph.edge_count());
    for (int src : graph)
        for (auto [dst, w] : graph.Neighbors_of(src))
            if (dst > src) edges.emplace_back(src, dst, w);
    sort(edges.begin(), edges.end());

    for (Edge e : edges)
    {
        // do not close a cycle
        if (trees.find(e.src) == trees.find(e.dst)) continue;

        forest.addNode(e.src); forest.addNode(e.dst);
        forest.connect(e.src, e.dst, e.weight);
        trees.merge(e.src, e.dst);

        // forest connected into a tree
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
    if (algos.count(algo)) return algos[algo](graph);

   throw invalid_argument("algo is not supported");
}