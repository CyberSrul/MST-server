#include "mst.hpp"
#include <stdexcept>
#include <functional>




Graph kruskal(const Graph& graph){return graph;};
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