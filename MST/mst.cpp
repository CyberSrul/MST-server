#include "mst.hpp"
#include <stdexcept>
#include <functional>
#include <queue>
#include <limits>




/* Data structures */


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


/* Solvers */


Graph kruskal(const Graph& graph)
{
    Graph forest;
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
    Graph tree;
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


/* Factory */


unordered_map<string, function<Graph(const Graph&)>> algos =
{
    {"kruskal", kruskal},
    {"prim", prim}
};


Graph MST(const string& algo, const Graph& graph)
{
    if (! algos.count(algo)) throw invalid_argument("algo is not supported");

    if (! graph.connected()) throw invalid_argument("the graph is disconnected");

    return algos[algo](graph);
}


/* Statistics */


float total_weight(const Graph& tree)
{
    float total = 0;

    for (int src : tree)
        for (auto [dst, w] : tree.Neighbors_of(src))
            if (dst > src) total += w;

    return total;
}


float min_edge(const Graph& tree)
{
    float lowest = numeric_limits<float>::max();

    for (int src : tree)
        for (auto [dst, w] : tree.Neighbors_of(src))
            if (w < lowest) lowest = w;

    return lowest;
}


unordered_map<int, float> distances(int src, const Graph& tree)
{
    unordered_map<int, float> dists;
    function<void(int, int, float)> propegate;

    propegate = [&dists, &tree, &propegate](int parent, int node, float dist)
    {
        dists[node] = dist;

        for (auto [nei, w] : tree.Neighbors_of(node))
            if (nei != parent) propegate(node, nei, dist + w);
    };

    propegate(src, src, 0);

    return dists;
}


float max_distance(const Graph& tree)
{
    unordered_map<int, float> dists;
    int max_node;
    float max_dist;

    auto find_max = [&max_node, &max_dist, &dists]()
    {
        max_node = 0; max_dist = 0;

        for (auto [node, dist] : dists)
            if (dist > max_dist)
            {
                max_node = node;
                max_dist = dist;
            }
    };

    dists = distances(* tree.begin(), tree);
    find_max();
    dists = distances(max_node, tree);
    find_max();

    return max_dist;
}


unordered_map<int, int> sub_trees_sizes(int src, const Graph& tree)
{
    unordered_map<int, int> sizes;
    function<void(int, int)> propegate;

    propegate = [&sizes, &tree, &propegate](int parent, int node)
    {
        sizes[node] = 1;

        for (auto [nei, w] : tree.Neighbors_of(node))
            if (nei != parent)
            {
                propegate(node, nei);
                sizes[node] += sizes[nei];
            }
    };

    propegate(src, src);

    return sizes;
}


float avg_distance(const Graph& tree)
{
    int root = * tree.begin();

    // node -> sum of distances from node to all others
    auto distsums = distances(root, tree);
    for (auto [node, dist] : distsums)
        if (node != root) distsums[root] += dist;

    // node -> size of sub-tree rooted in node
    auto sizes = sub_trees_sizes(root, tree);

    // filling the rest of distsums according to a formula
    function<void(int, int, float)> propegate;
    propegate = [&distsums, &sizes, &tree, &propegate](int parent, int node, float weight)
    {
        distsums[node] = distsums[parent] + weight * ((float) tree.vx_count() - 2 * sizes[node]);

        for (auto [nei, w] : tree.Neighbors_of(node))
            if (nei != parent) propegate(node, nei, w);
    };

    propegate(root, root, 0);

    // avg distance = sum of distances of all paths / num of paths
    int path_count = tree.vx_count() * tree.vx_count();
    float sum = 0;
    for (auto [node, dist] : distsums) sum += dist / path_count;

    return sum;
}