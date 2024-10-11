#include "graph.hpp"
#include "mst.hpp"
#include <iostream>




template <typename type>
void check(type expected, type actuall)
{
    const string status = expected == actuall ? "pass" : "fail";
    cout << status << endl;
}


int main(void)
{
    cout << "start" << endl;
    Graph graph(0);
    check(0u, graph.vx_count());
    check(0u, graph.edge_count());

    cout << "addNode" << endl;
    for (int node = 0; node < 5; graph.addNode(node++));
    check(5u, graph.vx_count());

    cout << "connected" << endl;
    check(false, graph.connected(0, 4));
    graph.connect(0, 4, 1);
    check(true, graph.connected(0, 4));
    check(1u, graph.edge_count());

    cout << "connect" << endl;
    for (int src : graph)
        for (int dst : graph)
            if (src != dst)
                graph.connect(src, dst, 1);

    check(10u, graph.edge_count());

    for (int node : graph)
        check(4lu, graph.Neighbors_of(node).size());

    cout << "removeNode" << endl;
    graph.removeNode(2);
    check(4u, graph.vx_count());
    check(6u, graph.edge_count());

    for (int node : graph)
        check(3lu, graph.Neighbors_of(node).size());

    cout << "clear" << endl;
    graph = Graph(6);
    check(6u, graph.vx_count());
    check(0u, graph.edge_count());

    cout << "kruskal" << endl;
    graph.connect(0, 1, 1);
    graph.connect(0, 3, 5);
    graph.connect(1, 2, 4);
    graph.connect(1, 3, 2);
    graph.connect(2, 3, 3);
    graph.connect(2, 5, 6);
    graph.connect(3, 4, 7);
    graph.connect(4, 5, 8);

    Graph tree = MST("kruskal", graph);
    check(graph.vx_count(), tree.vx_count());
    check(graph.vx_count() - 1, tree.edge_count());
    check(true, tree.connected(0, 1));
    check(true, tree.connected(1, 3));
    check(true, tree.connected(2, 3));
    check(true, tree.connected(2, 5));
    check(true, tree.connected(3, 4));

    graph.disconnect(2, 3);
    tree = MST("kruskal", graph);
    check(graph.vx_count(), tree.vx_count());
    check(graph.vx_count() - 1, tree.edge_count());
    check(true, tree.connected(0, 1));
    check(true, tree.connected(1, 3));
    check(true, tree.connected(1, 2));
    check(true, tree.connected(2, 5));
    check(true, tree.connected(3, 4));



    return 0;
}