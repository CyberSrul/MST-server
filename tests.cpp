#include "graph.hpp"
#include "mst.hpp"
#include <iostream>




template <typename type>
void check(type expected, type actuall)
{
    const string status = expected == actuall ? "pass" : "fail";
    cout << status << endl;
}

void check_close(float expected, float actuall)
{
    const string status = abs(expected - actuall) < 1e-2 ? "pass" : "fail";
    cout << status << endl;
}


int main(void)
{
    cout << "Graph" << endl;

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





    cout << "MST" << endl;

    Graph graph1 = Graph(6);
    graph1.connect(0, 1, 1);
    graph1.connect(0, 3, 5);
    graph1.connect(1, 2, 4);
    graph1.connect(1, 3, 2);
    graph1.connect(2, 3, 3);
    graph1.connect(2, 5, 6);
    graph1.connect(3, 4, 7);
    graph1.connect(4, 5, 8);

    Graph graph2 = Graph(5);
    graph2.connect(0, 1, 1);
    graph2.connect(1, 2, 5);
    graph2.connect(2, 3, 2);
    graph2.connect(3, 4, 3);
    graph2.connect(4, 0, 4);
    graph2.connect(1, 3, 6);

    // graph with a bridge
    Graph graph3 = Graph(6);
    graph3.connect(0, 1, 1);
    graph3.connect(0, 2, 1);
    graph3.connect(1, 2, 2);
    graph3.connect(2, 3, 1);
    graph3.connect(3, 4, 1);
    graph3.connect(3, 5, 1);
    graph3.connect(4, 5, 2);
    
    auto MSTtests = [&graph1, &graph2, &graph3](string algo)
    {
        cout << algo << endl;

        Graph tree = MST(algo, graph1);
        check(graph1.vx_count(), tree.vx_count());
        check(graph1.vx_count() - 1, tree.edge_count());
        check(true, tree.connected(0, 1));
        check(true, tree.connected(1, 3));
        check(true, tree.connected(2, 3));
        check(true, tree.connected(2, 5));
        check(true, tree.connected(3, 4));

        tree = MST(algo, graph2);
        check(graph2.vx_count(), tree.vx_count());
        check(graph2.vx_count() - 1, tree.edge_count());
        check(true, tree.connected(0, 1));
        check(true, tree.connected(2, 3));
        check(true, tree.connected(3, 4));
        check(true, tree.connected(4, 0));

        tree = MST(algo, graph3);
        check(graph3.vx_count(), tree.vx_count());
        check(graph3.vx_count() - 1, tree.edge_count());
        check(true, tree.connected(0, 1));
        check(true, tree.connected(0, 2));
        check(true, tree.connected(2, 3));
        check(true, tree.connected(3, 4));
        check(true, tree.connected(3, 5));

        // Big graph for runtime testing
        int size = 1500;
        float density = 0.5;
        Graph graph = RandomGraph(size, density);

        // Making sure the graph is connected
        graph.addNode(size);
        for (int node = 0; node < size; node++)
            graph.connect(node, size, 1);

        tree = MST(algo, graph);
        check(graph.vx_count(), tree.vx_count());
        check(graph.vx_count() - 1, tree.edge_count());
    };

    MSTtests("kruskal");
    MSTtests("prim");

    cout << "statistics" << endl;

    Graph tree;

    tree = MST("kruskal", graph1);
    check(19.0f, total_weight(tree));
    check(1.0f, min_edge(tree));
    check(16.0f, max_distance(tree));
    check_close(6.11f, avg_distance(tree));

    tree = MST("kruskal", graph2);
    check(10.0f, total_weight(tree));
    check(1.0f, min_edge(tree));
    check(10.0f, max_distance(tree));
    check_close(4.32f, avg_distance(tree));

    tree = MST("kruskal", graph3);
    check(5.0f, total_weight(tree));
    check(1.0f, min_edge(tree));
    check(4.0f, max_distance(tree));
    check_close(1.77f, avg_distance(tree));



    return 0;
}