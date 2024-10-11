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
    Graph graph;

    cout << "start" << endl;
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



    return 0;
}