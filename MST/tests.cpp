#include "graph.hpp"
#include "mst.hpp"
#include <iostream>
#include <functional>




static unsigned int Fails = 0, Passes = 0;


template <typename type>
void check(type expected, type actuall)
{
    const string status = expected == actuall ? "pass" : "fail";

    status == "fail" ? Fails++ : Passes++;

    cout << status << endl;

    if (status == "fail") cout << "expected: " << expected << endl << "got: " << actuall << endl;
}

void check_close(float expected, float actuall)
{
    const string status = abs(expected - actuall) < 1e-2 ? "pass" : "fail";

    status == "fail" ? Fails++ : Passes++;

    cout << status << endl;
}


void check_throws(string expected, function<void()> trigger)
{
    try{ trigger(); }
    catch(const exception& e){ check(expected, string(e.what())); }
}


void Summary()
{
    cout << endl  << "********************" << endl;
    cout << Passes << " tests were passed"  << endl;
    cout << Fails  << " tests failed"       << endl;
    cout          << "********************" << endl;
}


int main(void)
{
    cout << "Graph" << endl;

    cout << "start" << endl;
    Graph graph;
    check(0u, graph.vx_count());
    check(0u, graph.edge_count());

    cout << "addNode" << endl;
    for (int node = 0; node < 5; graph.addNode(node++));
    check(5u, graph.vx_count());
    for (int node = 0; node < 5; check(true, graph.hasNode(node++)));

    cout << "connected" << endl;
    check(false, graph.connected(0, 4));
    graph.connect(0, 4, 1);
    check(true, graph.connected(0, 4));
    check(1u, graph.edge_count());

    cout << "connect (vxs)" << endl;
    for (int src : graph)
        for (int dst : graph)
            if (src != dst)
                graph.connect(src, dst, 1);

    check(10u, graph.edge_count());

    for (int node : graph)
        check(4lu, graph.Neighbors_of(node).size());

    check_throws("src or dst were not found",       [&graph](){ graph.connect(0, 10, 0); });
    check_throws("a node can not point to itslef",  [&graph](){ graph.connect(0, 0, 0);  });

    cout << "removeNode" << endl;
    graph.removeNode(2);
    check(4u, graph.vx_count());
    check(6u, graph.edge_count());
    check(false, graph.hasNode(2));

    cout << "Neighbors_of" << endl;
    for (int node : graph)
        check(3lu, graph.Neighbors_of(node).size());

    check_throws("node was not found", [&graph](){ graph.Neighbors_of(10); });

    cout << "disconnect" << endl;
    graph.disconnect(1, 3);
    check(false, graph.connected(1, 3));
    check(4u, graph.vx_count());
    check(5u, graph.edge_count());

    cout << "clear" << endl;
    graph = Graph(7);
    check(7u, graph.vx_count());
    check(0u, graph.edge_count());

    cout << "connected (graph)" << endl;
    check(false, graph.connected());
    graph.connect(0, 1, 0);
    graph.connect(1, 2, 0);
    graph.connect(3, 6, 0);
    check(false, graph.connected());
    graph.connect(5, 3, 0);
    graph.connect(5, 4, 0);
    check(false, graph.connected());
    graph.connect(5, 6, 0);
    check(false, graph.connected());
    graph.connect(0, 6, 0);
    check(true, graph.connected());
    graph.disconnect(1, 2);
    check(false, graph.connected());
    graph.connect(3, 2, 0);
    check(true, graph.connected());



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

        total_weight(tree);
        min_edge(tree);
        max_distance(tree);
        avg_distance(tree);
    };

    MSTtests("kruskal");
    MSTtests("prim");

    check_throws("algo is not supported", [&MSTtests](){ MSTtests("not supported"); });
    check_throws("the graph is disconnected", [](){ MST("prim", Graph(10)); });





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



    Summary();
    return 0;
}