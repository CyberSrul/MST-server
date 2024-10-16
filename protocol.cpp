#include "protocol.hpp"
#include "mst.hpp"
#include <sstream>



/*
    Global variables for an MST server.
    A weighted graph, a MST and statistics:
    min, max & avg distances in the tree.
    As well as total weight.s
*/
Graph graph, tree;
float longest = 0, shortes = 0, avg = 0, total = 0;
bool updated = true;




void EditGraph(int vxnum, int ednum, bool add, istringstream& input)
{
    int src, dst;
    char delimiter;
    float weight;

    for (int itr = 0; itr < vxnum; itr++)
    {
        input >> src;
        add ? graph.addNode(src) : graph.removeNode(src);
    }
    for (int itr = 0; itr < ednum; itr++)
    {
        input >> src >> delimiter >> dst;

        if (add)
        {
            input >> delimiter >> weight;
            graph.connect(src, dst, weight);
        }
        else
        {
            if (tree.connected(src, dst)) updated = false;
            graph.disconnect(src, dst);
        }
    }

    if (vxnum) updated = false;
}


stringstream handle_request(const string& msg)
{
    stringstream response;
    istringstream request(msg);
    string cmd; request >> cmd;


    if (cmd == "NewGraph")
    {
        updated = false;

        int vxnum, ednum;
        char delimeter;
        request >> vxnum >> delimeter >> ednum;

        graph = Graph(vxnum);
        EditGraph(0, ednum, true, request);

        response << "Graph built";
    }
    else if (cmd == "AddNode")
    {
        EditGraph(1, 0, true, request);
        response << "Node added";
    }
    else if (cmd == "RemoveNode")
    {
        EditGraph(1, 0, false, request);
        response << "Node removed";
    }
    else if (cmd == "Connect")
    {
        EditGraph(0, 1, true, request);
        response << "Vertecies connected";
    }
    else if (cmd == "DisConnect")
    {
        EditGraph(0, 1, false, request);
        response << "Vertecies disconnected";
    }
    else if (cmd == "MST")
    {
        if (! updated)
        {
            request >> cmd;

            try { tree = MST(cmd, graph); }
            catch(const std::exception& e) { response << e.what(); }

            longest = max_distance(tree);
            shortes = min_edge(tree);
            avg     = avg_distance(tree);
            total   = total_weight(tree);

            updated = true;
        }

        response << "Total weight: " << total   << endl;
        response << "Min distance: " << shortes << endl;
        response << "Max distance: " << longest << endl;
        response << "Avg distance: " << avg;
    }
    else
    {
        response << "Command not recognized.";
    }

    response << endl;
    return response;
}